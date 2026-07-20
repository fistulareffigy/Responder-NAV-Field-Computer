#include <ctype.h>
#include <fcntl.h>
#include <string.h>

#include <responder/AppPackageManager.hpp>

namespace responder {

namespace {

bool extractJsonStringValue(const String &json, const char *key, char *out, size_t outLen) {
  if (!key || !out || outLen == 0) {
    return false;
  }
  out[0] = '\0';
  String needle = String("\"") + key + "\"";
  int keyPos = json.indexOf(needle);
  if (keyPos < 0) {
    return false;
  }
  int colon = json.indexOf(':', keyPos + needle.length());
  if (colon < 0) {
    return false;
  }
  int quote = json.indexOf('"', colon + 1);
  if (quote < 0) {
    return false;
  }
  String value;
  bool escaped = false;
  for (int i = quote + 1; i < json.length(); ++i) {
    char c = json[i];
    if (escaped) {
      value += c;
      escaped = false;
      continue;
    }
    if (c == '\\') {
      escaped = true;
      continue;
    }
    if (c == '"') {
      break;
    }
    value += c;
  }
  value.trim();
  value.toCharArray(out, outLen);
  return out[0] != '\0';
}

uint32_t extractJsonUIntValue(const String &json, const char *key, uint32_t fallback) {
  if (!key) {
    return fallback;
  }
  String needle = String("\"") + key + "\"";
  int keyPos = json.indexOf(needle);
  if (keyPos < 0) {
    return fallback;
  }
  int colon = json.indexOf(':', keyPos + needle.length());
  if (colon < 0) {
    return fallback;
  }
  int start = colon + 1;
  while (start < json.length() && isspace(static_cast<unsigned char>(json[start]))) {
    ++start;
  }
  uint32_t value = 0;
  bool anyDigit = false;
  for (int i = start; i < json.length(); ++i) {
    char c = json[i];
    if (c < '0' || c > '9') {
      break;
    }
    anyDigit = true;
    value = value * 10U + static_cast<uint32_t>(c - '0');
  }
  return anyDigit ? value : fallback;
}

uint8_t extractJsonStringArrayValue(const String &json, const char *key,
                                    char out[][kExternalAppPermissionMax],
                                    uint8_t maxItems) {
  if (!key || !out || maxItems == 0) {
    return 0;
  }
  String needle = String("\"") + key + "\"";
  int keyPos = json.indexOf(needle);
  if (keyPos < 0) {
    return 0;
  }
  int colon = json.indexOf(':', keyPos + needle.length());
  if (colon < 0) {
    return 0;
  }
  int open = json.indexOf('[', colon + 1);
  if (open < 0) {
    return 0;
  }
  uint8_t count = 0;
  int cursor = open + 1;
  while (cursor < json.length() && count < maxItems) {
    int quote = json.indexOf('"', cursor);
    int close = json.indexOf(']', cursor);
    if (quote < 0 || (close >= 0 && close < quote)) {
      break;
    }
    String value;
    bool escaped = false;
    int i = quote + 1;
    for (; i < json.length(); ++i) {
      char c = json[i];
      if (escaped) {
        value += c;
        escaped = false;
        continue;
      }
      if (c == '\\') {
        escaped = true;
        continue;
      }
      if (c == '"') {
        break;
      }
      value += c;
    }
    value.trim();
    if (value.length() > 0) {
      value.toCharArray(out[count], kExternalAppPermissionMax);
      ++count;
    }
    cursor = i + 1;
  }
  return count;
}

void copyFallbackString(char *out, size_t outLen, const char *fallback) {
  if (!out || outLen == 0) {
    return;
  }
  out[0] = '\0';
  if (!fallback) {
    return;
  }
  strncpy(out, fallback, outLen - 1);
  out[outLen - 1] = '\0';
}

}  // namespace

void clearExternalAppManifests(ExternalAppManifest *manifests, uint8_t maxCount) {
  if (!manifests) {
    return;
  }
  for (uint8_t i = 0; i < maxCount; ++i) {
    manifests[i] = ExternalAppManifest{};
  }
}

bool loadExternalAppManifest(SdFs &sd, const String &packagePath, const String &manifestPath,
                             ExternalAppManifest &out) {
  out = ExternalAppManifest{};
  FsFile manifest = sd.open(manifestPath.c_str(), O_RDONLY);
  if (!manifest) {
    return false;
  }
  String json;
  while (manifest.available() && json.length() < 4096) {
    int ch = manifest.read();
    if (ch < 0) {
      break;
    }
    json += static_cast<char>(ch);
  }
  manifest.close();

  extractJsonStringValue(json, "id", out.id, sizeof(out.id));
  extractJsonStringValue(json, "name", out.name, sizeof(out.name));
  extractJsonStringValue(json, "version", out.version, sizeof(out.version));
  out.permissionCount =
      extractJsonStringArrayValue(json, "permissions", out.permissions, kExternalAppMaxPermissions);
  out.minimumApiVersion = extractJsonUIntValue(json, "minimumApiVersion", RESPONDER_API_VERSION);
  packagePath.toCharArray(out.path, sizeof(out.path));

  if (!out.id[0]) {
    String fallbackId = packagePath;
    int slash = fallbackId.lastIndexOf('/');
    if (slash >= 0) {
      fallbackId = fallbackId.substring(slash + 1);
    }
    fallbackId.toCharArray(out.id, sizeof(out.id));
  }
  if (!out.name[0]) {
    copyFallbackString(out.name, sizeof(out.name), out.id);
  }
  if (!out.version[0]) {
    copyFallbackString(out.version, sizeof(out.version), "unknown");
  }

  out.valid = out.id[0] && out.name[0] && out.minimumApiVersion <= RESPONDER_API_VERSION;
  return out.valid;
}

}  // namespace responder
