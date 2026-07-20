#include "app_framework.h"

NativeAppRegistry::NativeAppRegistry(NativeAppCallbacks *slots, uint8_t count)
    : _slots(slots), _count(count) {}

bool NativeAppRegistry::registerApp(uint8_t index, const NativeAppCallbacks &callbacks) {
  if (!_slots || index >= _count) return false;
  _slots[index] = callbacks;
  return true;
}

const NativeAppCallbacks *NativeAppRegistry::get(uint8_t index) const {
  if (!_slots || index >= _count || !_slots[index].id) return nullptr;
  return &_slots[index];
}

bool NativeAppRegistry::has(uint8_t index) const {
  return get(index) != nullptr;
}

bool NativeAppRegistry::start(uint8_t index) const {
  const NativeAppCallbacks *app = get(index);
  if (!app || !app->onStart) return false;
  app->onStart();
  return true;
}

bool NativeAppRegistry::stop(uint8_t index) const {
  const NativeAppCallbacks *app = get(index);
  if (!app || !app->onStop) return false;
  app->onStop();
  return true;
}

bool NativeAppRegistry::draw(uint8_t index) const {
  const NativeAppCallbacks *app = get(index);
  if (!app || !app->onDraw) return false;
  app->onDraw();
  return true;
}

bool NativeAppRegistry::touch(uint8_t index, int16_t x, int16_t y) const {
  const NativeAppCallbacks *app = get(index);
  if (!app || !app->onTouch) return false;
  app->onTouch(x, y);
  return true;
}

bool NativeAppRegistry::key(uint8_t index, uint8_t modifier, uint8_t keycode) const {
  const NativeAppCallbacks *app = get(index);
  if (!app || !app->onKey) return false;
  app->onKey(modifier, keycode);
  return true;
}

bool NativeAppRegistry::update(uint8_t index, uint32_t now) const {
  const NativeAppCallbacks *app = get(index);
  if (!app || !app->onUpdate) return false;
  app->onUpdate(now);
  return true;
}
