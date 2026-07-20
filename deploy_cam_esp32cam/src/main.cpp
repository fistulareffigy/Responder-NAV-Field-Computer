#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <Preferences.h>
#include "esp_camera.h"

#if __has_include("wifi_secrets.h")
#include "wifi_secrets.h"
#endif

#ifndef DEPLOY_CAM_WIFI_SSID
#define DEPLOY_CAM_WIFI_SSID ""
#endif

#ifndef DEPLOY_CAM_WIFI_PASS
#define DEPLOY_CAM_WIFI_PASS ""
#endif

// Arduino IDE CAMERA_MODEL_WROVER_KIT pin map.
#define PWDN_GPIO_NUM  -1
#define RESET_GPIO_NUM -1
#define XCLK_GPIO_NUM  21
#define SIOD_GPIO_NUM  26
#define SIOC_GPIO_NUM  27

#define Y9_GPIO_NUM    35
#define Y8_GPIO_NUM    34
#define Y7_GPIO_NUM    39
#define Y6_GPIO_NUM    36
#define Y5_GPIO_NUM    19
#define Y4_GPIO_NUM    18
#define Y3_GPIO_NUM    5
#define Y2_GPIO_NUM    4
#define VSYNC_GPIO_NUM 25
#define HREF_GPIO_NUM  23
#define PCLK_GPIO_NUM  22

// WROVER-KIT camera modules usually do not have a flashlight LED.
// Set this to your LED GPIO if your carrier board has one.
static constexpr int kFlashLedPin = -1;

static WebServer server(80);
static DNSServer dnsServer;
static Preferences prefs;
static bool cameraReady = false;
static bool flashOn = false;
static String cameraFrameSize = "QVGA";
static int cameraQuality = 34;
static String apSsid;
static String staSsid;
static String staPass;
static bool staEnabled = false;
static bool staConnected = false;

static constexpr byte kDnsPort = 53;
static const IPAddress kApIp(192, 168, 4, 1);
static const IPAddress kApGateway(192, 168, 4, 1);
static const IPAddress kApSubnet(255, 255, 255, 0);

static void sendCorsHeaders() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Cache-Control", "no-store, no-cache, must-revalidate, max-age=0");
}

static String htmlEscape(const String &input) {
  String out;
  out.reserve(input.length() + 8);
  for (size_t i = 0; i < input.length(); ++i) {
    char c = input[i];
    if (c == '&') out += F("&amp;");
    else if (c == '<') out += F("&lt;");
    else if (c == '>') out += F("&gt;");
    else if (c == '"') out += F("&quot;");
    else out += c;
  }
  return out;
}

static void loadSettings() {
  prefs.begin("deploycam", false);
  staSsid = prefs.getString("ssid", DEPLOY_CAM_WIFI_SSID);
  staPass = prefs.getString("pass", DEPLOY_CAM_WIFI_PASS);
  staEnabled = prefs.getBool("sta", staSsid.length() > 0);
  uint64_t mac = ESP.getEfuseMac();
  char suffix[7] = {};
  snprintf(suffix, sizeof(suffix), "%06X", static_cast<unsigned>(mac & 0xFFFFFF));
  apSsid = String("DeployCam-") + suffix;
}

static void saveSettings(const String &ssid, const String &pass, bool enableSta) {
  staSsid = ssid;
  staPass = pass;
  staEnabled = enableSta && staSsid.length() > 0;
  prefs.putString("ssid", staSsid);
  prefs.putString("pass", staPass);
  prefs.putBool("sta", staEnabled);
}

static bool initCamera() {
  camera_config_t config = {};
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  config.grab_mode = CAMERA_GRAB_LATEST;

  if (psramFound()) {
    config.frame_size = FRAMESIZE_QVGA;
    config.jpeg_quality = cameraQuality;
    config.fb_count = 2;
    config.fb_location = CAMERA_FB_IN_PSRAM;
  } else {
    config.frame_size = FRAMESIZE_QVGA;
    config.jpeg_quality = cameraQuality;
    config.fb_count = 1;
    config.fb_location = CAMERA_FB_IN_DRAM;
    cameraFrameSize = "QVGA";
  }

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("CAM: init failed 0x%x\n", err);
    return false;
  }

  sensor_t *sensor = esp_camera_sensor_get();
  if (sensor) {
    sensor->set_framesize(sensor, FRAMESIZE_QVGA);
    sensor->set_quality(sensor, cameraQuality);
    sensor->set_brightness(sensor, 0);
    sensor->set_contrast(sensor, 0);
    sensor->set_saturation(sensor, 0);
  }

  Serial.println("CAM: ready");
  return true;
}

static void handleRoot() {
  sendCorsHeaders();
  String body;
  body.reserve(2600);
  body += "<!doctype html><html><head><meta name='viewport' content='width=device-width,initial-scale=1'>";
  body += "<title>Deploy Cam</title><style>";
  body += "body{font-family:monospace;background:#101008;color:#d8c36a;margin:16px}";
  body += "a{color:#ffe58a}input,button{font:inherit;margin:4px 0;padding:8px;background:#1c1b12;color:#ffe58a;border:1px solid #8b7b35}";
  body += ".card{border:1px solid #6d612f;padding:12px;margin:10px 0}.ok{color:#9ee37d}.bad{color:#ff7878}";
  body += "</style></head><body>";
  body += "<h2>Deploy Cam</h2><div class='card'>";
  body += "<p>Camera: <span class='";
  body += cameraReady ? "ok" : "bad";
  body += "'>";
  body += cameraReady ? "camera ready" : "camera failed";
  body += "</span></p>";
  body += "<p>Hotspot: <b>";
  body += htmlEscape(apSsid);
  body += "</b> / pass <b>deploycam</b> / IP <b>";
  body += WiFi.softAPIP().toString();
  body += "</b></p><p>Home Wi-Fi: ";
  body += staConnected ? "<span class='ok'>connected</span> " : "<span class='bad'>not connected</span> ";
  body += htmlEscape(staSsid);
  if (staConnected) {
    body += " / IP <b>";
    body += WiFi.localIP().toString();
    body += "</b> / RSSI ";
    body += String(WiFi.RSSI());
  }
  body += "</p></div><div class='card'><h3>Camera Links</h3><ul>";
  body += "<li><a href='/capture'>/capture</a> single JPEG frame</li>";
  body += "<li><a href='/stream'>/stream</a> MJPEG live stream</li>";
  body += "<li><a href='/status'>/status</a> status JSON</li>";
  body += "</ul></div><div class='card'><h3>Wi-Fi Setup</h3>";
  body += "<form method='POST' action='/save'>";
  body += "<label>Home Wi-Fi SSID</label><br><input name='ssid' value='";
  body += htmlEscape(staSsid);
  body += "' style='width:95%'><br><label>Password</label><br>";
  body += "<input name='pass' type='password' value='";
  body += htmlEscape(staPass);
  body += "' style='width:95%'><br>";
  body += "<label><input name='sta' type='checkbox' value='1' ";
  body += staEnabled ? "checked" : "";
  body += "> Also join home Wi-Fi</label><br>";
  body += "<button type='submit'>Save and Reboot</button></form>";
  body += "<p><a href='/ap-only'>Stay hotspot-only</a> | <a href='/reboot'>Reboot</a></p></div>";
  body += "<div class='card'><img src='/stream' style='max-width:100%;height:auto;border:1px solid #d8c36a'></div>";
  body += "</body></html>";
  server.send(200, "text/html", body);
}

static void handleStatus() {
  sendCorsHeaders();
  String json;
  json.reserve(512);
  json += "{";
  json += "\"name\":\"Deploy Cam\",";
  json += "\"cameraReady\":";
  json += cameraReady ? "true" : "false";
  json += ",\"apSsid\":\"";
  json += apSsid;
  json += "\",\"apIp\":\"";
  json += WiFi.softAPIP().toString();
  json += "\",\"staEnabled\":";
  json += staEnabled ? "true" : "false";
  json += ",\"staConnected\":";
  json += staConnected ? "true" : "false";
  json += ",\"staSsid\":\"";
  json += staSsid;
  json += "\",\"staIp\":\"";
  json += WiFi.localIP().toString();
  json += "\",\"rssi\":";
  json += WiFi.RSSI();
  json += ",\"frameSize\":\"";
  json += cameraFrameSize;
  json += "\",\"quality\":";
  json += cameraQuality;
  json += ",\"heap\":";
  json += ESP.getFreeHeap();
  json += ",\"psram\":";
  json += psramFound() ? ESP.getFreePsram() : 0;
  json += "}";
  server.send(200, "application/json", json);
}

static void handleSave() {
  String ssid = server.arg("ssid");
  String pass = server.arg("pass");
  bool enableSta = server.hasArg("sta");
  saveSettings(ssid, pass, enableSta);
  sendCorsHeaders();
  String body = "<!doctype html><html><body style='font-family:monospace;background:#101008;color:#d8c36a'>";
  body += "<h3>Deploy Cam saved.</h3><p>Rebooting now. Hotspot remains available as ";
  body += htmlEscape(apSsid);
  body += ".</p></body></html>";
  server.send(200, "text/html", body);
  delay(500);
  ESP.restart();
}

static void handleApOnly() {
  saveSettings(staSsid, staPass, false);
  sendCorsHeaders();
  server.send(200, "text/html",
              "<!doctype html><html><body style='font-family:monospace;background:#101008;color:#d8c36a'>"
              "<h3>Deploy Cam set to hotspot-only.</h3><p>Rebooting now.</p></body></html>");
  delay(500);
  ESP.restart();
}

static void handleReboot() {
  sendCorsHeaders();
  server.send(200, "text/plain", "rebooting");
  delay(300);
  ESP.restart();
}

static void handleCapture() {
  if (!cameraReady) {
    server.send(503, "text/plain", "camera not ready");
    return;
  }

  camera_fb_t *fb = esp_camera_fb_get();
  if (!fb) {
    server.send(503, "text/plain", "capture failed");
    return;
  }

  sendCorsHeaders();
  server.sendHeader("Content-Disposition", "inline; filename=capture.jpg");
  server.setContentLength(fb->len);
  server.send(200, "image/jpeg", "");
  WiFiClient client = server.client();
  client.write(fb->buf, fb->len);
  esp_camera_fb_return(fb);
}

static void handleCaptureHex() {
  if (!cameraReady) {
    server.send(503, "text/plain", "camera not ready");
    return;
  }

  camera_fb_t *fb = esp_camera_fb_get();
  if (!fb) {
    server.send(503, "text/plain", "capture failed");
    return;
  }

  static const char hexDigits[] = "0123456789ABCDEF";
  char header[32];
  int headerLen = snprintf(header, sizeof(header), "JPGHEX %u\n", static_cast<unsigned>(fb->len));
  sendCorsHeaders();
  server.setContentLength(static_cast<size_t>(headerLen) + fb->len * 2U + 1U);
  server.send(200, "text/plain", "");
  WiFiClient client = server.client();
  client.write(reinterpret_cast<const uint8_t *>(header), static_cast<size_t>(headerLen));
  char chunk[129];
  size_t used = 0;
  for (size_t i = 0; i < fb->len; ++i) {
    uint8_t b = fb->buf[i];
    chunk[used++] = hexDigits[b >> 4];
    chunk[used++] = hexDigits[b & 0x0F];
    if (used >= sizeof(chunk) - 2) {
      client.write(reinterpret_cast<const uint8_t *>(chunk), used);
      used = 0;
    }
  }
  if (used) client.write(reinterpret_cast<const uint8_t *>(chunk), used);
  client.print("\n");
  client.stop();
  esp_camera_fb_return(fb);
}

static void handleStream() {
  if (!cameraReady) {
    server.send(503, "text/plain", "camera not ready");
    return;
  }

  WiFiClient client = server.client();
  client.setNoDelay(true);
  client.setTimeout(2);
  client.print("HTTP/1.1 200 OK\r\n"
               "Access-Control-Allow-Origin: *\r\n"
               "Content-Type: multipart/x-mixed-replace; boundary=deploycam\r\n"
               "Cache-Control: no-store, no-cache, must-revalidate, max-age=0\r\n"
               "Pragma: no-cache\r\n"
               "Connection: close\r\n\r\n");
  client.flush();

  uint32_t frames = 0;
  uint32_t started = millis();
  while (client.connected()) {
    camera_fb_t *fb = esp_camera_fb_get();
    if (!fb) {
      delay(30);
      continue;
    }
    client.printf("--deploycam\r\nContent-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n",
                  static_cast<unsigned>(fb->len));
    size_t written = client.write(fb->buf, fb->len);
    client.print("\r\n");
    esp_camera_fb_return(fb);
    if (written == 0) break;
    ++frames;
    if ((frames % 60) == 0) {
      Serial.printf("STREAM: frames=%lu ms=%lu\n",
                    static_cast<unsigned long>(frames),
                    static_cast<unsigned long>(millis() - started));
    }
    delay(35);
  }
  client.stop();
  Serial.printf("STREAM: closed frames=%lu\n", static_cast<unsigned long>(frames));
}

static void handleStreamHex() {
  if (!cameraReady) {
    server.send(503, "text/plain", "camera not ready");
    return;
  }

  WiFiClient client = server.client();
  client.setNoDelay(true);
  client.setTimeout(2);
  client.print("HTTP/1.1 200 OK\r\n"
               "Access-Control-Allow-Origin: *\r\n"
               "Content-Type: text/plain\r\n"
               "Cache-Control: no-store, no-cache, must-revalidate, max-age=0\r\n"
               "Pragma: no-cache\r\n"
               "Connection: close\r\n\r\n");
  client.flush();

  static const char hexDigits[] = "0123456789ABCDEF";
  uint32_t frames = 0;
  uint32_t started = millis();
  while (client.connected()) {
    camera_fb_t *fb = esp_camera_fb_get();
    if (!fb) {
      delay(25);
      continue;
    }

    client.printf("JPGHEX %u\n", static_cast<unsigned>(fb->len));
    char chunk[257];
    size_t used = 0;
    for (size_t i = 0; i < fb->len && client.connected(); ++i) {
      uint8_t b = fb->buf[i];
      chunk[used++] = hexDigits[b >> 4];
      chunk[used++] = hexDigits[b & 0x0F];
      if (used >= sizeof(chunk) - 2) {
        if (client.write(reinterpret_cast<const uint8_t *>(chunk), used) == 0) break;
        used = 0;
      }
    }
    if (used && client.connected()) {
      client.write(reinterpret_cast<const uint8_t *>(chunk), used);
    }
    client.print("\n");
    esp_camera_fb_return(fb);

    ++frames;
    if ((frames % 120) == 0) {
      Serial.printf("STREAMHEX: frames=%lu ms=%lu\n",
                    static_cast<unsigned long>(frames),
                    static_cast<unsigned long>(millis() - started));
    }
    delay(25);
  }
  client.stop();
  Serial.printf("STREAMHEX: closed frames=%lu\n", static_cast<unsigned long>(frames));
}

static bool writeClientAll(WiFiClient &client, const uint8_t *data, size_t length,
                           uint32_t timeoutMs) {
  size_t written = 0;
  uint32_t deadline = millis() + timeoutMs;
  while (written < length && client.connected()) {
    size_t chunk = min<size_t>(4096, length - written);
    size_t count = client.write(data + written, chunk);
    if (count > 0) {
      written += count;
      deadline = millis() + timeoutMs;
      continue;
    }
    if (static_cast<int32_t>(deadline - millis()) <= 0) break;
    delay(1);
  }
  return written == length;
}

static void handleStreamRaw() {
  if (!cameraReady) {
    server.send(503, "text/plain", "camera not ready");
    return;
  }

  WiFiClient client = server.client();
  client.setNoDelay(true);
  client.setTimeout(2);
  client.print("HTTP/1.1 200 OK\r\n"
               "Access-Control-Allow-Origin: *\r\n"
               "Content-Type: application/x-responder-jpeg-stream\r\n"
               "Cache-Control: no-store, no-cache, must-revalidate, max-age=0\r\n"
               "Connection: close\r\n\r\n");
  client.flush();

  uint32_t frames = 0;
  uint32_t started = millis();
  while (client.connected()) {
    camera_fb_t *fb = esp_camera_fb_get();
    if (!fb) {
      delay(2);
      continue;
    }
    char header[32];
    int headerLength = snprintf(header, sizeof(header), "JPG %u\n",
                                static_cast<unsigned>(fb->len));
    bool ok = headerLength > 0 &&
              writeClientAll(client, reinterpret_cast<const uint8_t *>(header),
                             static_cast<size_t>(headerLength), 800) &&
              writeClientAll(client, fb->buf, fb->len, 1200);
    esp_camera_fb_return(fb);
    if (!ok) break;
    ++frames;
    if ((frames % 120U) == 0U) {
      uint32_t elapsed = max<uint32_t>(1, millis() - started);
      Serial.printf("STREAMRAW: frames=%lu fps=%lu.%lu\n",
                    static_cast<unsigned long>(frames),
                    static_cast<unsigned long>((frames * 1000U) / elapsed),
                    static_cast<unsigned long>(((frames * 10000U) / elapsed) % 10U));
    }
    delay(1);
  }
  client.stop();
  Serial.printf("STREAMRAW: closed frames=%lu\n", static_cast<unsigned long>(frames));
}

static void handleFlash(bool enable) {
  if (kFlashLedPin < 0) {
    server.send(404, "text/plain", "flash LED not configured");
    return;
  }
  flashOn = enable;
  digitalWrite(kFlashLedPin, flashOn ? HIGH : LOW);
  server.send(200, "text/plain", flashOn ? "flash on" : "flash off");
}

static void handleQuality() {
  if (!server.hasArg("value")) {
    server.send(400, "text/plain", "missing value");
    return;
  }
  int value = constrain(server.arg("value").toInt(), 4, 63);
  sensor_t *sensor = esp_camera_sensor_get();
  if (!sensor) {
    server.send(503, "text/plain", "sensor unavailable");
    return;
  }
  sensor->set_quality(sensor, value);
  cameraQuality = value;
  server.send(200, "text/plain", "quality=" + String(value));
}

static void handleFrameSize() {
  if (!server.hasArg("value")) {
    server.send(400, "text/plain", "missing value");
    return;
  }
  String value = server.arg("value");
  value.toLowerCase();
  framesize_t frameSize = FRAMESIZE_VGA;
  String label = "VGA";
  if (value == "qqvga" || value == "160x120") {
    frameSize = FRAMESIZE_QQVGA;
    label = "QQVGA";
  } else if (value == "qvga" || value == "320x240") {
    frameSize = FRAMESIZE_QVGA;
    label = "QVGA";
  } else if (value == "cif" || value == "400x296") {
    frameSize = FRAMESIZE_CIF;
    label = "CIF";
  } else if (value == "vga" || value == "640x480") {
    frameSize = FRAMESIZE_VGA;
    label = "VGA";
  } else {
    server.send(400, "text/plain", "use qqvga, qvga, cif, or vga");
    return;
  }
  sensor_t *sensor = esp_camera_sensor_get();
  if (!sensor) {
    server.send(503, "text/plain", "sensor unavailable");
    return;
  }
  sensor->set_framesize(sensor, frameSize);
  cameraFrameSize = label;
  server.send(200, "text/plain", "framesize=" + label);
}

static void connectWifi() {
  WiFi.persistent(false);
  WiFi.setSleep(false);
  WiFi.mode(staEnabled ? WIFI_AP_STA : WIFI_AP);
  WiFi.softAPConfig(kApIp, kApGateway, kApSubnet);
  bool apOk = WiFi.softAP(apSsid.c_str(), "deploycam");
  Serial.printf("WIFI: AP %s %s ip=%s pass=deploycam\n",
                apSsid.c_str(), apOk ? "started" : "failed",
                WiFi.softAPIP().toString().c_str());
  dnsServer.start(kDnsPort, "*", WiFi.softAPIP());

  if (staEnabled && staSsid.length() > 0) {
    WiFi.begin(staSsid.c_str(), staPass.c_str());
    Serial.printf("WIFI: connecting STA to %s\n", staSsid.c_str());
    uint32_t deadline = millis() + 20000;
    while (WiFi.status() != WL_CONNECTED && static_cast<int32_t>(deadline - millis()) > 0) {
      delay(250);
      Serial.print(".");
    }
    Serial.println();
    if (WiFi.status() == WL_CONNECTED) {
      staConnected = true;
      Serial.printf("WIFI: STA connected ip=%s rssi=%d\n",
                    WiFi.localIP().toString().c_str(), WiFi.RSSI());
      return;
    }
    staConnected = false;
    Serial.println("WIFI: STA failed; AP remains available");
  } else {
    staConnected = false;
    Serial.println("WIFI: hotspot-only mode");
  }
}

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println();
  Serial.println("DEPLOY CAM: boot");

  if (kFlashLedPin >= 0) {
    pinMode(kFlashLedPin, OUTPUT);
    digitalWrite(kFlashLedPin, LOW);
  }

  loadSettings();
  cameraReady = initCamera();
  connectWifi();

  server.on("/", HTTP_GET, handleRoot);
  server.on("/status", HTTP_GET, handleStatus);
  server.on("/save", HTTP_POST, handleSave);
  server.on("/ap-only", HTTP_GET, handleApOnly);
  server.on("/reboot", HTTP_GET, handleReboot);
  server.on("/capture", HTTP_GET, handleCapture);
  server.on("/capture.hex", HTTP_GET, handleCaptureHex);
  server.on("/stream", HTTP_GET, handleStream);
  server.on("/stream.hex", HTTP_GET, handleStreamHex);
  server.on("/stream.raw", HTTP_GET, handleStreamRaw);
  server.on("/flash/on", HTTP_GET, []() { handleFlash(true); });
  server.on("/flash/off", HTTP_GET, []() { handleFlash(false); });
  server.on("/quality", HTTP_GET, handleQuality);
  server.on("/framesize", HTTP_GET, handleFrameSize);
  server.onNotFound([]() {
    String uri = server.uri();
    String accept = server.header("Accept");
    accept.toLowerCase();
    if (uri.indexOf("jpg") >= 0 || uri.indexOf("jpeg") >= 0 || uri.indexOf("snapshot") >= 0 ||
        uri.indexOf("video") >= 0 || uri.indexOf("mjpg") >= 0 || uri.indexOf("onvif") >= 0 ||
        uri.indexOf("cgi") >= 0) {
      server.send(404, "text/plain", "not found");
      return;
    }
    if (accept.indexOf("text/html") >= 0 || accept.length() == 0) {
      server.sendHeader("Location", String("http://") + WiFi.softAPIP().toString() + "/", true);
      server.send(302, "text/plain", "Deploy Cam setup");
    } else {
      server.send(404, "text/plain", "not found");
    }
  });
  server.begin();
  Serial.println("HTTP: server started");
}

void loop() {
  dnsServer.processNextRequest();
  server.handleClient();
  delay(2);
}
