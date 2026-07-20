#pragma once

#include <Arduino.h>

#ifndef RESPONDER_API_VERSION
#define RESPONDER_API_VERSION 1
#endif

namespace responder {

struct AppMetadata {
  const char *id = nullptr;
  const char *name = nullptr;
  const char *version = nullptr;
  const char *author = nullptr;
  const char *description = nullptr;
  const char *category = nullptr;
  const char *iconPath = nullptr;
  const char *entryPoint = nullptr;
  uint32_t minimumApiVersion = RESPONDER_API_VERSION;
  bool builtIn = true;
  bool supportsBackground = false;
  bool legacy = false;
  uint8_t legacyIndex = 0xFF;
  const char *const *permissions = nullptr;
  uint8_t permissionCount = 0;
  const char *const *requiredDrivers = nullptr;
  uint8_t requiredDriverCount = 0;
  const char *const *requiredCapabilities = nullptr;
  uint8_t requiredCapabilityCount = 0;
  const char *const *optionalCapabilities = nullptr;
  uint8_t optionalCapabilityCount = 0;
};

}  // namespace responder
