#include <Arduino.h>
#include <M5Unified.h>
#include <SdFat.h>
#include <fcntl.h>
#include <esp_random.h>

#if ENABLE_WIFI
#include <WiFi.h>
#endif

#include <responder/IResponderApp.hpp>
#include <responder/Permissions.hpp>

extern int16_t screenW;
extern int16_t screenH;
extern int16_t navH;
extern bool sdReady;
extern SdFs SD;
bool lockSd(uint32_t timeoutMs);
void unlockSd();
void drawAppTopBar(const char *title);
void drawNavBar();
void debugPrint(const char *fmt, ...);
void requestWifiBoot();

namespace {

#ifndef FILE_READ
#define FILE_READ O_RDONLY
#endif
#ifndef FILE_WRITE
#define FILE_WRITE (O_WRONLY | O_CREAT | O_TRUNC)
#endif

constexpr uint16_t kFileTransferPort = 8080;
constexpr size_t kHttpLineMax = 192;
struct UiRect {
  int16_t x;
  int16_t y;
  int16_t w;
  int16_t h;
};
constexpr const char *kFileTransferPermissions[] = {
    responder::permissions::Display,
    responder::permissions::Input,
    responder::permissions::NetworkLocal,
    responder::permissions::StorageRead,
    responder::permissions::StorageWrite,
};

String urlDecode(const String &in) {
  String out;
  out.reserve(in.length());
  for (size_t i = 0; i < in.length(); ++i) {
    char c = in[i];
    if (c == '%' && i + 2 < in.length()) {
      char hex[3] = {in[i + 1], in[i + 2], 0};
      out += static_cast<char>(strtol(hex, nullptr, 16));
      i += 2;
    } else if (c == '+') {
      out += ' ';
    } else {
      out += c;
    }
  }
  return out;
}

String queryValue(const String &target, const char *key) {
  String needle = String(key) + "=";
  int q = target.indexOf('?');
  if (q < 0) return "";
  int pos = target.indexOf(needle, q + 1);
  if (pos < 0) return "";
  pos += needle.length();
  int end = target.indexOf('&', pos);
  if (end < 0) end = target.length();
  return urlDecode(target.substring(pos, end));
}

bool sanitizePath(String &path, bool allowRoot = false) {
  path.trim();
  if (path.length() == 0) path = "/";
  if (!path.startsWith("/")) path = "/" + path;
  path.replace("\\", "/");
  while (path.indexOf("//") >= 0) path.replace("//", "/");
  if (path.indexOf("..") >= 0) return false;
  if (!allowRoot && path == "/") return false;
  return true;
}

void sendPlain(WiFiClient &client, int code, const char *status, const String &body) {
  client.printf("HTTP/1.1 %d %s\r\n", code, status);
  client.print("Connection: close\r\nContent-Type: text/plain\r\nCache-Control: no-store\r\n");
  client.printf("Content-Length: %u\r\n\r\n", static_cast<unsigned>(body.length()));
  client.print(body);
}

String urlEncode(const String &in) {
  static const char hex[] = "0123456789ABCDEF";
  String out;
  out.reserve(in.length() + 8);
  for (size_t i = 0; i < in.length(); ++i) {
    uint8_t c = static_cast<uint8_t>(in[i]);
    if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ||
        (c >= '0' && c <= '9') || c == '-' || c == '_' || c == '.' || c == '~' ||
        c == '/') {
      out += static_cast<char>(c);
    } else {
      out += '%';
      out += hex[c >> 4];
      out += hex[c & 0x0F];
    }
  }
  return out;
}

String parentPathOf(const String &path) {
  if (path.length() <= 1) return "/";
  String p = path;
  if (p.endsWith("/") && p.length() > 1) p.remove(p.length() - 1);
  int slash = p.lastIndexOf('/');
  if (slash <= 0) return "/";
  return p.substring(0, slash);
}

String joinPath(const String &dir, const String &name) {
  if (dir == "/") return "/" + name;
  return dir + "/" + name;
}

void sendRedirect(WiFiClient &client, const String &dir = "/") {
  String safeDir = dir;
  if (!sanitizePath(safeDir, true)) safeDir = "/";
  client.print("HTTP/1.1 303 See Other\r\nLocation: /?dir=");
  client.print(urlEncode(safeDir));
  client.print("\r\nConnection: close\r\n\r\n");
}

void appendEscaped(String &out, const String &s) {
  for (size_t i = 0; i < s.length(); ++i) {
    char c = s[i];
    if (c == '&') out += F("&amp;");
    else if (c == '<') out += F("&lt;");
    else if (c == '>') out += F("&gt;");
    else if (c == '"') out += F("&quot;");
    else out += c;
  }
}

class FileTransferApp final : public responder::IResponderApp {
public:
  const responder::AppMetadata &metadata() const override { return _metadata; }

  bool onStart(responder::AppContext &context) override {
    if (context.logger()) context.logger()->info("APP", "File Transfer start");
    _status = "STARTING";
    _lastClientMs = 0;
    _needsFullRedraw = true;
    _lastRenderedStatus = "";
    _lastRenderedWifi = "";
    _lastRenderedUrl = "";
    _lastRenderedRequest = "";
#if ENABLE_WIFI
    char token[17] = {};
    snprintf(token, sizeof(token), "%08lx%08lx",
             static_cast<unsigned long>(esp_random()),
             static_cast<unsigned long>(esp_random()));
    _accessToken = token;
    _wifiBootRequested = false;
    if (WiFi.status() != WL_CONNECTED) {
      _status = "REBOOTING FOR WI-FI";
      _wifiBootRequested = true;
      debugPrint("FILEXFER: Wi-Fi not connected; requesting Wi-Fi profile boot");
      requestWifiBoot();
    }
#endif
    ensureServer();
    return true;
  }

  void onStop() override {
#if ENABLE_WIFI
    _server.stop();
#endif
    _serverStarted = false;
    _status = "STOPPED";
    _needsFullRedraw = true;
  }

  void onUpdate(uint32_t nowMs) override {
    (void)nowMs;
    ensureServer();
    serviceClient();
  }

  void onRender(responder::DisplayCanvas &canvas) override {
    (void)canvas;
    const uint16_t black = M5.Display.color565(0, 0, 0);
    const uint16_t amber = M5.Display.color565(255, 176, 0);
    const uint16_t amberDim = M5.Display.color565(150, 92, 0);
    const uint16_t red = M5.Display.color565(220, 48, 32);

#if ENABLE_WIFI
    bool connected = WiFi.status() == WL_CONNECTED;
    String wifiLine = String("Wi-Fi: ") + (connected ? WiFi.SSID() : "NOT CONNECTED");
    String url = connected ? String("http://") + WiFi.localIP().toString() + ":" +
                                 String(kFileTransferPort) + "/?token=" + _accessToken
                           : "Connect Wi-Fi first";
#else
    bool connected = false;
    String wifiLine = "Wi-Fi build disabled";
    String url = "";
#endif
    String statusLine = String("Status: ") + _status;
    String requestLine = String("Last request: ") + _lastRequest;
    bool statusChanged = statusLine != _lastRenderedStatus;
    bool wifiChanged = wifiLine != _lastRenderedWifi;
    bool urlChanged = url != _lastRenderedUrl;
    bool requestChanged = requestLine != _lastRenderedRequest;
    if (!_needsFullRedraw && !statusChanged && !wifiChanged && !urlChanged && !requestChanged) {
      return;
    }
    bool fullRedraw = _needsFullRedraw;
    _lastRenderedStatus = statusLine;
    _lastRenderedWifi = wifiLine;
    _lastRenderedUrl = url;
    _lastRenderedRequest = requestLine;

    int16_t top = 58;
    int16_t bottom = static_cast<int16_t>(screenH - navH - 8);
    UiRect statusRect = {24, static_cast<int16_t>(top + 70),
                         static_cast<int16_t>(screenW - 48), 34};
    UiRect wifiRect = {24, static_cast<int16_t>(top + 108),
                       static_cast<int16_t>(screenW - 48), 34};
    UiRect urlRect = {24, static_cast<int16_t>(top + 150),
                      static_cast<int16_t>(screenW - 48), 82};
    UiRect requestRect = {24, static_cast<int16_t>(top + 316),
                          static_cast<int16_t>(screenW - 48), 54};
    if (fullRedraw) {
      drawAppTopBar("FILE TRANSFER");
      M5.Display.fillRect(0, top, screenW, static_cast<int16_t>(bottom - top), black);
      M5.Display.drawRect(16, static_cast<int16_t>(top + 10),
                          static_cast<int16_t>(screenW - 32),
                          static_cast<int16_t>(bottom - top - 20), amberDim);
      M5.Display.drawRect(19, static_cast<int16_t>(top + 13),
                          static_cast<int16_t>(screenW - 38),
                          static_cast<int16_t>(bottom - top - 26), amberDim);
      M5.Display.setTextDatum(TL_DATUM);
      M5.Display.setTextSize(3);
      M5.Display.setTextColor(amber, black);
      M5.Display.drawString("SD CARD // WEB TRANSFER", 24, static_cast<int16_t>(top + 22));
      M5.Display.setTextSize(2);
      M5.Display.setTextColor(amberDim, black);
      M5.Display.drawString("Open from a PC or phone on the same network", 24,
                            static_cast<int16_t>(top + 146));
      M5.Display.drawString("Upload, download, browse, and delete SD files.", 24,
                            static_cast<int16_t>(top + 252));
      M5.Display.drawString("USB storage import remains disabled to protect SDR/OBD.", 24,
                            static_cast<int16_t>(top + 282));
      M5.Display.drawRect(requestRect.x, requestRect.y, requestRect.w, requestRect.h, amberDim);
      drawNavBar();
      statusChanged = wifiChanged = urlChanged = requestChanged = true;
    }
    M5.Display.setTextDatum(TL_DATUM);
    M5.Display.setTextSize(2);
    if (statusChanged) {
      M5.Display.fillRect(statusRect.x, statusRect.y, statusRect.w, statusRect.h, black);
      M5.Display.fillRect(statusRect.x, statusRect.y + 3, 4, statusRect.h - 6,
                          _serverStarted ? amber : red);
      M5.Display.setTextColor(_serverStarted ? amber : red, black);
      M5.Display.drawString(statusLine, statusRect.x + 12, statusRect.y + 5);
    }
#if ENABLE_WIFI
    if (wifiChanged) {
      M5.Display.fillRect(wifiRect.x, wifiRect.y, wifiRect.w, wifiRect.h, black);
      M5.Display.setTextColor(connected ? amber : red, black);
      M5.Display.drawString(wifiLine, wifiRect.x + 12, wifiRect.y + 5);
    }
    if (urlChanged) {
      M5.Display.fillRect(urlRect.x, urlRect.y + 24, urlRect.w,
                          static_cast<int16_t>(urlRect.h - 24), black);
      M5.Display.setTextSize(3);
      M5.Display.setTextColor(connected && _serverStarted ? amber : amberDim, black);
      M5.Display.drawString(url, urlRect.x, static_cast<int16_t>(urlRect.y + 30));
      M5.Display.setTextSize(2);
    }
#else
    if (wifiChanged) {
      M5.Display.fillRect(wifiRect.x, wifiRect.y, wifiRect.w, wifiRect.h, black);
      M5.Display.setTextColor(red, black);
      M5.Display.drawString("Wi-Fi build disabled", wifiRect.x + 12, wifiRect.y + 5);
    }
#endif
    if (requestChanged) {
      M5.Display.fillRect(requestRect.x + 2, requestRect.y + 2,
                          requestRect.w - 4, requestRect.h - 4, black);
      M5.Display.setTextSize(2);
      M5.Display.setTextColor(amberDim, black);
      M5.Display.drawString(requestLine, requestRect.x + 10, requestRect.y + 12);
    }
    _needsFullRedraw = false;
  }

private:
  void ensureServer() {
#if ENABLE_WIFI
    if (WiFi.status() != WL_CONNECTED) {
      _status = _wifiBootRequested ? "REBOOTING FOR WI-FI" : "WAITING FOR WI-FI";
      return;
    }
    _wifiBootRequested = false;
    if (!_serverStarted || !_server) {
      _server.stop();
      _server.begin(kFileTransferPort, 1);
      _server.setNoDelay(true);
      _serverStarted = static_cast<bool>(_server);
      _status = _serverStarted ? "READY" : "SERVER START FAIL";
      _url = String("http://") + WiFi.localIP().toString() + ":" +
             String(kFileTransferPort) + "/?token=" + _accessToken;
      debugPrint("FILEXFER: server %s port=%u protected=1",
                 _serverStarted ? "ready" : "start failed",
                 static_cast<unsigned>(kFileTransferPort));
    } else {
      String currentUrl = String("http://") + WiFi.localIP().toString() + ":" +
                          String(kFileTransferPort) + "/?token=" + _accessToken;
      if (currentUrl != _url) {
        _url = currentUrl;
        debugPrint("FILEXFER: local address changed; protected URL refreshed");
      }
    }
#else
    _status = "WI-FI DISABLED";
#endif
  }

  bool readLine(WiFiClient &client, char *buffer, size_t bufferSize, uint32_t timeoutMs) {
    size_t used = 0;
    uint32_t start = millis();
    while (millis() - start < timeoutMs && client.connected()) {
      while (client.available()) {
        char c = static_cast<char>(client.read());
        if (c == '\r') continue;
        if (c == '\n') {
          buffer[min(used, bufferSize - 1)] = 0;
          return true;
        }
        if (used + 1 < bufferSize) buffer[used++] = c;
      }
      delay(1);
    }
    buffer[min(used, bufferSize - 1)] = 0;
    return used > 0;
  }

  void serviceClient() {
#if ENABLE_WIFI
    if (!_serverStarted || !_server) return;
    if (!_server.hasClient()) return;
    WiFiClient client = _server.accept();
    if (!client) return;
    client.setTimeout(1000);
    debugPrint("FILEXFER: client connected");
    char line[kHttpLineMax] = {};
    if (!readLine(client, line, sizeof(line), 1200)) {
      debugPrint("FILEXFER: client no request line");
      client.stop();
      return;
    }
    String request(line);
    int firstSpace = request.indexOf(' ');
    int secondSpace = request.indexOf(' ', firstSpace + 1);
    if (firstSpace < 0 || secondSpace < 0) {
      sendPlain(client, 400, "Bad Request", "Bad request");
      client.stop();
      return;
    }
    String method = request.substring(0, firstSpace);
    String target = request.substring(firstSpace + 1, secondSpace);
    int queryStart = target.indexOf('?');
    String requestPath = queryStart >= 0 ? target.substring(0, queryStart) : target;
    debugPrint("FILEXFER: request method=%s path=%s", method.c_str(), requestPath.c_str());
    int contentLength = 0;
    bool cookieAuthorized = false;
    while (readLine(client, line, sizeof(line), 1200)) {
      if (line[0] == 0) break;
      String header(line);
      if (header.startsWith("Cookie:") || header.startsWith("cookie:")) {
        cookieAuthorized = header.indexOf(String("RNFT=") + _accessToken) >= 0;
      }
      header.toLowerCase();
      if (header.startsWith("content-length:")) {
        contentLength = header.substring(15).toInt();
      }
    }
    bool queryAuthorized = queryValue(target, "token") == _accessToken;
    bool authorized = cookieAuthorized || queryAuthorized;
    if (method == "GET" && target == "/ping") {
      sendPlain(client, 200, "OK", "OK\n");
    } else if (!authorized) {
      sendPlain(client, 401, "Unauthorized",
                "Access denied. Open the protected URL shown on the Tab5 screen.\n");
    } else if (method == "GET" && (target == "/" || target.startsWith("/?"))) {
      handleIndex(client, target, queryAuthorized);
    } else if (method == "GET" && target.startsWith("/download")) {
      handleDownload(client, target);
    } else if (method == "GET" && target.startsWith("/delete")) {
      handleDelete(client, target);
    } else if (method == "POST" && target.startsWith("/mkdir")) {
      handleMkdir(client, target);
    } else if (method == "POST" && target.startsWith("/upload")) {
      handleUpload(client, target, contentLength);
    } else {
      sendPlain(client, 404, "Not Found", "Not found");
    }
    client.stop();
#endif
  }

  void handleIndex(WiFiClient &client, const String &target, bool setAccessCookie) {
    String dir = queryValue(target, "dir");
    if (!sanitizePath(dir, true)) dir = "/";
    String html;
    html.reserve(12288);
    html += F("<!doctype html><html><head><meta charset=utf-8><meta name=viewport content='width=device-width,initial-scale=1'>");
    html += F("<title>Responder Nav File Transfer</title><style>:root{--bg:#050400;--panel:#0d0902;--amber:#ffb000;--bright:#ffe08a;--dim:#8f5700;--red:#dc3020}*{box-sizing:border-box}body{margin:0;background:radial-gradient(circle at 78% 12%,#221500 0,#080500 34%,var(--bg) 70%);color:var(--amber);font-family:'Courier New',monospace;min-height:100vh}body:after{content:'';position:fixed;inset:0;pointer-events:none;background:repeating-linear-gradient(0deg,transparent 0 3px,rgba(255,176,0,.025) 3px 4px)}header{border-bottom:1px solid var(--dim);padding:20px clamp(16px,4vw,52px);background:#050400e8;display:flex;justify-content:space-between;gap:18px;align-items:center;flex-wrap:wrap}h1{margin:0;font-size:clamp(22px,4vw,38px);letter-spacing:.08em}h1 span,.dim{color:var(--dim)}.badge{border:1px solid var(--amber);padding:7px 12px;font-size:12px;letter-spacing:.12em}.shell{width:min(1180px,calc(100% - 28px));margin:22px auto}.intro{color:var(--dim);margin:0 0 18px}.path,.panel{border:1px solid var(--dim);background:linear-gradient(135deg,#100b02,#070500);box-shadow:inset 0 0 24px #000;padding:14px}.path{overflow-wrap:anywhere;margin-bottom:12px}.toolbar,.row{display:flex;gap:10px;flex-wrap:wrap;align-items:center}.toolbar{margin:12px 0 18px}.actions{display:grid;grid-template-columns:1fr 1fr;gap:14px;margin:14px 0}.panel h2{font-size:16px;letter-spacing:.12em;margin:0 0 12px;color:var(--bright)}a{color:var(--bright);text-decoration:none}a:hover{text-shadow:0 0 8px var(--amber)}button,input{font:inherit;background:#080500;border:1px solid var(--amber);color:var(--amber);padding:10px 12px;min-height:42px}button{cursor:pointer;text-transform:uppercase;letter-spacing:.06em}button:hover{background:#241600;color:var(--bright)}input[type=file],input[type=text]{flex:1;min-width:190px}.progress{height:18px;border:1px solid var(--dim);margin-top:12px;padding:2px}.bar{height:100%;width:0;background:var(--amber);transition:width .12s}.status{min-height:22px;margin:8px 0 0;color:var(--bright);white-space:pre-wrap}.grid{display:grid;grid-template-columns:1fr 1fr;gap:14px}ul{list-style:none;padding:0;margin:0}.entry{border-top:1px solid #3d2600;padding:11px 4px;display:flex;gap:10px;align-items:center;flex-wrap:wrap}.entry:first-child{border-top:0}.entry .name{flex:1;min-width:150px}.tag{color:var(--dim);font-size:12px}.danger{color:#ff6a50}footer{color:var(--dim);padding:22px 2px;text-align:center;font-size:12px}@media(max-width:720px){.actions,.grid{grid-template-columns:1fr}header{align-items:flex-start}.shell{width:min(100% - 18px,1180px)}} </style></head><body>");
    html += F("<header><div><h1>RESPONDER NAV <span>//</span> FILE TRANSFER</h1><div class=dim>FIELD STORAGE INTERFACE</div></div><div class=badge>SD LINK ACTIVE</div></header><div class=shell><p class=intro>Browse folders, upload into the selected folder, download files, or remove files from the Tab5 SD card.</p>");
    html += F("<div class=path><span class=dim>CURRENT FOLDER</span><br>");
    appendEscaped(html, dir);
    html += F("</div><div class=toolbar>");
    if (dir != "/") {
      html += F("<a href='/?dir=");
      html += urlEncode(parentPathOf(dir));
      html += F("'>[ UP ONE LEVEL ]</a>");
    }
    html += F("<a href='/?dir=/'>[ SD ROOT ]</a></div><div class=actions><section class=panel><h2>UPLOAD FILE</h2><div class=row><input id=f type=file><button onclick='up()'>Upload here</button></div><div class=progress><div id=b class=bar></div></div><div id=s class=status>READY</div></section><section class=panel><h2>CREATE FOLDER</h2><div class=row><input id=dn type=text placeholder='new-folder'><button onclick='mk()'>Create folder</button></div><div id=ms class=status>READY</div></section></div>");
    html += F("<script>const DIR=");
    html += "'";
    html += urlEncode(dir);
    html += "'";
    html += F(";function up(){let f=document.getElementById('f').files[0],s=document.getElementById('s'),b=document.getElementById('b');if(!f){s.textContent='SELECT A FILE';return}let x=new XMLHttpRequest;x.open('POST','/upload?dir='+DIR+'&name='+encodeURIComponent(f.name));x.upload.onprogress=e=>{if(e.lengthComputable){let p=Math.round(e.loaded*100/e.total);b.style.width=p+'%';s.textContent='UPLOADING '+f.name+'  '+p+'%'}};x.onload=()=>{s.textContent=x.responseText||('HTTP '+x.status);if(x.status>=200&&x.status<300)setTimeout(()=>location.href='/?dir='+DIR,650)};x.onerror=()=>s.textContent='UPLOAD CONNECTION FAILED';x.send(f)}async function mk(){let n=document.getElementById('dn').value.trim(),s=document.getElementById('ms');if(!n){s.textContent='ENTER A FOLDER NAME';return}let r=await fetch('/mkdir?dir='+DIR+'&name='+encodeURIComponent(n),{method:'POST'});s.textContent=await r.text();if(r.ok)setTimeout(()=>location.href='/?dir='+DIR,500)}</script>");
    html += F("<main class=grid><section class=panel><h2>FOLDERS</h2><ul>");
    if (!sdReady || !lockSd(300)) {
      html += F("<li>SD not ready or busy</li>");
    } else {
      FsFile root = SD.open(dir.c_str());
      if (root && root.isDirectory()) {
        for (;;) {
          FsFile f = root.openNextFile();
          if (!f) break;
          if (f.isDirectory()) {
            char name[96] = {};
            f.getName(name, sizeof(name));
            String child = joinPath(dir, String(name));
            html += F("<li class=entry><span class=tag>[DIR]</span><span class=name>");
            appendEscaped(html, String(name));
            html += F("</span>");
            html += F("/ <a href='/?dir=");
            html += urlEncode(child);
            html += F("'>open</a></li>");
          }
          f.close();
        }
        root.rewind();
        html += F("</ul></section><section class=panel><h2>FILES</h2><ul>");
        for (;;) {
          FsFile f = root.openNextFile();
          if (!f) break;
          if (!f.isDirectory()) {
            char name[96] = {};
            f.getName(name, sizeof(name));
            String path = joinPath(dir, String(name));
            html += F("<li class=entry><span class=tag>[FILE]</span><span class=name>");
            appendEscaped(html, String(name));
            html += F("</span><span class=dim>");
            html += String(static_cast<unsigned>(f.fileSize() / 1024));
            html += F(" KB</span> <a href='/download?path=");
            html += urlEncode(path);
            html += F("'>DOWNLOAD</a><a class=danger href='/delete?path=");
            html += urlEncode(path);
            html += F("&dir=");
            html += urlEncode(dir);
            html += F("' onclick='return confirm(\"Delete file?\")'>DELETE</a></li>");
          }
          f.close();
        }
      }
      if (root) root.close();
      unlockSd();
    }
    html += F("</ul></section></main><footer>RESPONDER NAV // LOCAL NETWORK ONLY // PORT 8080</footer></div></body></html>");
    client.print("HTTP/1.1 200 OK\r\nConnection: close\r\nContent-Type: text/html\r\nCache-Control: no-store\r\n");
    if (setAccessCookie) {
      client.print("Set-Cookie: RNFT=");
      client.print(_accessToken);
      client.print("; Path=/; HttpOnly; SameSite=Strict\r\n");
    }
    client.printf("Content-Length: %u\r\n\r\n", static_cast<unsigned>(html.length()));
    client.print(html);
    _lastRequest = "INDEX";
  }

  void handleDownload(WiFiClient &client, const String &target) {
    String path = queryValue(target, "path");
    if (!sanitizePath(path)) {
      sendPlain(client, 400, "Bad Request", "Bad path");
      return;
    }
    if (!sdReady || !lockSd(500)) {
      sendPlain(client, 503, "Busy", "SD busy");
      return;
    }
    FsFile file = SD.open(path.c_str(), FILE_READ);
    if (!file || file.isDirectory()) {
      if (file) file.close();
      unlockSd();
      sendPlain(client, 404, "Not Found", "File not found");
      return;
    }
    client.print("HTTP/1.1 200 OK\r\nConnection: close\r\nContent-Type: application/octet-stream\r\n");
    String filename = path.substring(path.lastIndexOf('/') + 1);
    client.printf("Content-Length: %u\r\nContent-Disposition: attachment; filename=\"%s\"\r\n\r\n",
                  static_cast<unsigned>(file.fileSize()), filename.c_str());
    uint8_t buffer[1024];
    while (file.available() && client.connected()) {
      int n = file.read(buffer, sizeof(buffer));
      if (n <= 0) break;
      client.write(buffer, n);
      delay(0);
    }
    file.close();
    unlockSd();
    _lastRequest = "DOWNLOAD " + path;
  }

  void handleDelete(WiFiClient &client, const String &target) {
    String path = queryValue(target, "path");
    if (!sanitizePath(path)) {
      sendPlain(client, 400, "Bad Request", "Bad path");
      return;
    }
    bool ok = false;
    if (sdReady && lockSd(500)) {
      ok = SD.remove(path.c_str());
      unlockSd();
    }
    _lastRequest = String("DELETE ") + (ok ? "OK " : "FAIL ") + path;
    String dir = queryValue(target, "dir");
    if (!sanitizePath(dir, true)) dir = parentPathOf(path);
    sendRedirect(client, dir);
  }

  void handleMkdir(WiFiClient &client, const String &target) {
    String dir = queryValue(target, "dir");
    String name = queryValue(target, "name");
    if (!sanitizePath(dir, true)) dir = "/";
    name.replace("\\", "/");
    int slash = name.lastIndexOf('/');
    if (slash >= 0) name = name.substring(slash + 1);
    name.trim();
    if (name.length() == 0 || name.indexOf("..") >= 0 || name == "." || name == "/") {
      sendPlain(client, 400, "Bad Request", "Bad folder name");
      return;
    }
    String path = joinPath(dir, name);
    if (!sanitizePath(path)) {
      sendPlain(client, 400, "Bad Request", "Bad folder path");
      return;
    }
    bool ok = false;
    if (sdReady && lockSd(1000)) {
      ok = SD.mkdir(path.c_str());
      unlockSd();
    }
    _lastRequest = String("MKDIR ") + (ok ? "OK " : "FAIL ") + path;
    sendPlain(client, ok ? 200 : 500, ok ? "OK" : "Error",
              ok ? "Created " + path : "Create folder failed");
  }

  void handleUpload(WiFiClient &client, const String &target, int contentLength) {
    String dir = queryValue(target, "dir");
    if (!sanitizePath(dir, true)) dir = "/";
    String name = queryValue(target, "name");
    if (name.length() == 0) name = "upload.bin";
    name.replace("\\", "/");
    int slash = name.lastIndexOf('/');
    if (slash >= 0) name = name.substring(slash + 1);
    String path = joinPath(dir, name);
    if (!sanitizePath(path) || contentLength < 0) {
      sendPlain(client, 400, "Bad Request", "Bad upload");
      return;
    }
    if (!sdReady || !lockSd(1000)) {
      sendPlain(client, 503, "Busy", "SD busy");
      return;
    }
    FsFile out = SD.open(path.c_str(), FILE_WRITE);
    if (!out) {
      unlockSd();
      sendPlain(client, 500, "Error", "Open failed");
      return;
    }
    uint8_t buffer[1024];
    int remaining = contentLength;
    uint32_t last = millis();
    bool ok = true;
    while (remaining > 0 && millis() - last < 8000) {
      int avail = client.available();
      if (avail <= 0) {
        delay(1);
        continue;
      }
      last = millis();
      int want = min<int>(min<int>(avail, remaining), sizeof(buffer));
      int got = client.read(buffer, want);
      if (got <= 0) continue;
      if (out.write(buffer, got) != got) {
        ok = false;
        break;
      }
      remaining -= got;
      delay(0);
    }
    out.close();
    unlockSd();
    if (!ok || remaining != 0) {
      SD.remove(path.c_str());
      sendPlain(client, 500, "Error", "Upload failed");
      _lastRequest = "UPLOAD FAIL " + path;
      return;
    }
    _lastRequest = "UPLOAD OK " + path;
    sendPlain(client, 200, "OK", "Uploaded " + path);
  }

#if ENABLE_WIFI
  WiFiServer _server{kFileTransferPort};
#endif
  bool _serverStarted = false;
  bool _wifiBootRequested = false;
  bool _needsFullRedraw = true;
  uint32_t _lastClientMs = 0;
  String _status = "STOPPED";
  String _lastRequest = "NONE";
  String _url;
  String _accessToken;
  String _lastRenderedStatus;
  String _lastRenderedWifi;
  String _lastRenderedUrl;
  String _lastRenderedRequest;
  responder::AppMetadata _metadata = {
      "com.achindustries.file-transfer",
      "FILE TRANSFER",
      "1.0.0",
      "ACH Industries",
      "Wi-Fi web UI for Tab5 SD card file transfer.",
      "Utilities",
      nullptr,
      "builtin:FileTransferApp",
      RESPONDER_API_VERSION,
      true,
      false,
      false,
      19,
      kFileTransferPermissions,
      static_cast<uint8_t>(sizeof(kFileTransferPermissions) / sizeof(kFileTransferPermissions[0])),
      nullptr,
      0,
      nullptr,
      0,
      nullptr,
      0,
  };
};

FileTransferApp gFileTransferApp;

}  // namespace

responder::IResponderApp *fileTransferResponderApp() {
  return &gFileTransferApp;
}
