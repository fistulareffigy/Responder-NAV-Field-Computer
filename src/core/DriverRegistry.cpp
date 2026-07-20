#include <string.h>

#include <responder/Driver.hpp>

namespace responder {

bool DriverRegistry::validateMetadata(const DriverMetadata &metadata) const {
  return metadata.id && metadata.id[0] && metadata.name && metadata.name[0];
}

bool DriverRegistry::idExists(const char *id) const {
  if (!id) return false;
  for (uint8_t i = 0; i < _count; ++i) {
    if (_records[i].metadata.id && strcmp(_records[i].metadata.id, id) == 0) {
      return true;
    }
  }
  return false;
}

bool DriverRegistry::registerDriver(IResponderDriver *driver) {
  if (!driver || _count >= kMaxRegisteredDrivers) return false;
  const DriverMetadata &metadata = driver->metadata();
  if (!validateMetadata(metadata) || idExists(metadata.id)) return false;
  _records[_count].driver = driver;
  _records[_count].metadata = metadata;
  _records[_count].installed = true;
  _records[_count].enabled = true;
  ++_count;
  return true;
}

const DriverRecord *DriverRegistry::recordAt(uint8_t ordinal) const {
  if (ordinal >= _count) return nullptr;
  return &_records[ordinal];
}

const DriverRecord *DriverRegistry::findById(const char *id) const {
  if (!id) return nullptr;
  for (uint8_t i = 0; i < _count; ++i) {
    if (_records[i].metadata.id && strcmp(_records[i].metadata.id, id) == 0) {
      return &_records[i];
    }
  }
  return nullptr;
}

}  // namespace responder
