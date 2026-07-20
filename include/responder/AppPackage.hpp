#pragma once

#include <Arduino.h>

#include "AppMetadata.hpp"

namespace responder {

constexpr const char *kSdAppRoot = "/apps";
constexpr const char *kAppManifestFile = "app.json";
constexpr const char *kAppAssetsDirectory = "assets";
constexpr const char *kAppDataDirectory = "data";
constexpr uint8_t kMaxDiscoveredAppPackages = 16;
constexpr size_t kExternalAppIdMax = 64;
constexpr size_t kExternalAppNameMax = 32;
constexpr size_t kExternalAppVersionMax = 16;
constexpr size_t kExternalAppPathMax = 96;
constexpr uint8_t kExternalAppMaxPermissions = 8;
constexpr size_t kExternalAppPermissionMax = 32;

struct ExternalAppManifest {
  char id[kExternalAppIdMax] = {};
  char name[kExternalAppNameMax] = {};
  char version[kExternalAppVersionMax] = {};
  char path[kExternalAppPathMax] = {};
  char permissions[kExternalAppMaxPermissions][kExternalAppPermissionMax] = {};
  uint8_t permissionCount = 0;
  uint32_t minimumApiVersion = RESPONDER_API_VERSION;
  bool valid = false;
};

}  // namespace responder
