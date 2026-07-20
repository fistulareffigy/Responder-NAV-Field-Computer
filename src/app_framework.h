#pragma once

#include <Arduino.h>

struct NativeAppCallbacks {
  const char *id = nullptr;
  const char *title = nullptr;
  void (*onStart)() = nullptr;
  void (*onStop)() = nullptr;
  void (*onDraw)() = nullptr;
  void (*onTouch)(int16_t x, int16_t y) = nullptr;
  void (*onKey)(uint8_t modifier, uint8_t keycode) = nullptr;
  void (*onUpdate)(uint32_t now) = nullptr;
};

class NativeAppRegistry {
public:
  NativeAppRegistry(NativeAppCallbacks *slots, uint8_t count);

  bool registerApp(uint8_t index, const NativeAppCallbacks &callbacks);
  const NativeAppCallbacks *get(uint8_t index) const;
  bool has(uint8_t index) const;

  bool start(uint8_t index) const;
  bool stop(uint8_t index) const;
  bool draw(uint8_t index) const;
  bool touch(uint8_t index, int16_t x, int16_t y) const;
  bool key(uint8_t index, uint8_t modifier, uint8_t keycode) const;
  bool update(uint8_t index, uint32_t now) const;

private:
  NativeAppCallbacks *_slots;
  uint8_t _count;
};
