#include <string.h>

#include <responder/AppRegistry.hpp>

namespace responder {

bool AppRegistry::validateMetadata(const AppMetadata &metadata) const {
  return metadata.id && metadata.id[0] && metadata.name && metadata.name[0] &&
         metadata.minimumApiVersion <= RESPONDER_API_VERSION;
}

bool AppRegistry::idExists(const char *id) const {
  if (!id) return false;
  for (uint8_t i = 0; i < _count; ++i) {
    if (_records[i].metadata.id && strcmp(_records[i].metadata.id, id) == 0) {
      return true;
    }
  }
  return false;
}

bool AppRegistry::registerBuiltIn(IResponderApp *app) {
  if (!app || _count >= kMaxRegisteredApps) return false;
  const AppMetadata &metadata = app->metadata();
  if (!validateMetadata(metadata) || idExists(metadata.id)) return false;
  _records[_count].app = app;
  _records[_count].metadata = metadata;
  _records[_count].installed = true;
  _records[_count].enabled = true;
  ++_count;
  return true;
}

bool AppRegistry::registerLegacy(const AppMetadata &metadata) {
  if (_count >= kMaxRegisteredApps || !validateMetadata(metadata) || idExists(metadata.id)) {
    return false;
  }
  _records[_count].app = nullptr;
  _records[_count].metadata = metadata;
  _records[_count].installed = true;
  _records[_count].enabled = true;
  ++_count;
  return true;
}

const AppRecord *AppRegistry::recordAt(uint8_t ordinal) const {
  if (ordinal >= _count) return nullptr;
  return &_records[ordinal];
}

const AppRecord *AppRegistry::findById(const char *id) const {
  if (!id) return nullptr;
  for (uint8_t i = 0; i < _count; ++i) {
    if (_records[i].metadata.id && strcmp(_records[i].metadata.id, id) == 0) {
      return &_records[i];
    }
  }
  return nullptr;
}

const AppRecord *AppRegistry::findByLegacyIndex(uint8_t legacyIndex) const {
  for (uint8_t i = 0; i < _count; ++i) {
    if (_records[i].metadata.legacyIndex == legacyIndex) {
      return &_records[i];
    }
  }
  return nullptr;
}

IResponderApp *AppRegistry::appByLegacyIndex(uint8_t legacyIndex) const {
  const AppRecord *record = findByLegacyIndex(legacyIndex);
  return record ? record->app : nullptr;
}

const char *AppRegistry::titleForLegacyIndex(uint8_t legacyIndex, const char *fallback) const {
  const AppRecord *record = findByLegacyIndex(legacyIndex);
  if (record && record->metadata.name && record->metadata.name[0]) return record->metadata.name;
  return fallback;
}

bool AppRegistry::launchByLegacyIndex(uint8_t legacyIndex, AppContext &context) {
  IResponderApp *app = appByLegacyIndex(legacyIndex);
  if (!app) {
    _activeLegacyIndex = legacyIndex;
    _activeApp = nullptr;
    return false;
  }
  if (_activeApp && _activeApp != app) {
    _activeApp->onSuspend();
  }
  _activeApp = app;
  _activeLegacyIndex = legacyIndex;
  return app->onStart(context);
}

bool AppRegistry::stopByLegacyIndex(uint8_t legacyIndex) {
  IResponderApp *app = appByLegacyIndex(legacyIndex);
  if (!app) return false;
  app->onStop();
  if (_activeApp == app) {
    _activeApp = nullptr;
    _activeLegacyIndex = kInvalidLegacyIndex;
  }
  return true;
}

bool AppRegistry::updateByLegacyIndex(uint8_t legacyIndex, uint32_t nowMs) {
  IResponderApp *app = appByLegacyIndex(legacyIndex);
  if (!app) return false;
  app->onUpdate(nowMs);
  return true;
}

bool AppRegistry::renderByLegacyIndex(uint8_t legacyIndex, DisplayCanvas &canvas) {
  IResponderApp *app = appByLegacyIndex(legacyIndex);
  if (!app) return false;
  app->onRender(canvas);
  return true;
}

bool AppRegistry::inputByLegacyIndex(uint8_t legacyIndex, const InputEvent &event) {
  IResponderApp *app = appByLegacyIndex(legacyIndex);
  if (!app) return false;
  return app->onInput(event);
}

}  // namespace responder
