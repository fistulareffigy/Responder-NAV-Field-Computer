#pragma once

#include "IResponderApp.hpp"

namespace responder {

constexpr uint8_t kMaxRegisteredApps = 32;
constexpr uint8_t kInvalidLegacyIndex = 0xFF;

struct AppRecord {
  IResponderApp *app = nullptr;
  AppMetadata metadata;
  bool installed = false;
  bool enabled = true;
};

class AppRegistry {
public:
  bool registerBuiltIn(IResponderApp *app);
  bool registerLegacy(const AppMetadata &metadata);

  uint8_t count() const { return _count; }
  const AppRecord *recordAt(uint8_t ordinal) const;
  const AppRecord *findById(const char *id) const;
  const AppRecord *findByLegacyIndex(uint8_t legacyIndex) const;
  IResponderApp *appByLegacyIndex(uint8_t legacyIndex) const;

  const char *titleForLegacyIndex(uint8_t legacyIndex, const char *fallback) const;
  bool launchByLegacyIndex(uint8_t legacyIndex, AppContext &context);
  bool stopByLegacyIndex(uint8_t legacyIndex);
  bool updateByLegacyIndex(uint8_t legacyIndex, uint32_t nowMs);
  bool renderByLegacyIndex(uint8_t legacyIndex, DisplayCanvas &canvas);
  bool inputByLegacyIndex(uint8_t legacyIndex, const InputEvent &event);

  uint8_t activeLegacyIndex() const { return _activeLegacyIndex; }
  IResponderApp *activeApp() const { return _activeApp; }

private:
  bool validateMetadata(const AppMetadata &metadata) const;
  bool idExists(const char *id) const;

  AppRecord _records[kMaxRegisteredApps];
  uint8_t _count = 0;
  uint8_t _activeLegacyIndex = kInvalidLegacyIndex;
  IResponderApp *_activeApp = nullptr;
};

}  // namespace responder
