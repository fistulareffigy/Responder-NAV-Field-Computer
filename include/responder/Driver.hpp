#pragma once

#include <Arduino.h>

#include "Services.hpp"

namespace responder {

enum class DriverClass : uint8_t {
  Unknown,
  Usb,
  Serial,
  Radio,
  Sensor,
  Camera,
  Network,
  Audio,
};

struct DriverMetadata {
  const char *id = nullptr;
  const char *name = nullptr;
  const char *version = nullptr;
  const char *author = nullptr;
  const char *description = nullptr;
  DriverClass driverClass = DriverClass::Unknown;
  const char *const *providedCapabilities = nullptr;
  uint8_t providedCapabilityCount = 0;
  const char *const *requiredPermissions = nullptr;
  uint8_t requiredPermissionCount = 0;
};

class IResponderDriver {
public:
  virtual ~IResponderDriver() = default;
  virtual const DriverMetadata &metadata() const = 0;
  virtual bool begin(AppServices &services) = 0;
  virtual void update(uint32_t nowMs) { (void)nowMs; }
  virtual void end() {}
  virtual ServiceStatus status() const = 0;
};

constexpr uint8_t kMaxRegisteredDrivers = 24;

struct DriverRecord {
  IResponderDriver *driver = nullptr;
  DriverMetadata metadata;
  bool installed = false;
  bool enabled = true;
};

class DriverRegistry {
public:
  bool registerDriver(IResponderDriver *driver);
  uint8_t count() const { return _count; }
  const DriverRecord *recordAt(uint8_t ordinal) const;
  const DriverRecord *findById(const char *id) const;

private:
  bool validateMetadata(const DriverMetadata &metadata) const;
  bool idExists(const char *id) const;

  DriverRecord _records[kMaxRegisteredDrivers];
  uint8_t _count = 0;
};

}  // namespace responder
