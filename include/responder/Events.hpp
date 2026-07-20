#pragma once

#include <Arduino.h>

namespace responder {

enum class InputEventType : uint8_t {
  None,
  KeyDown,
  KeyUp,
  TouchDown,
  TouchMove,
  TouchUp,
  Back,
  Confirm,
};

struct InputEvent {
  InputEventType type = InputEventType::None;
  uint8_t keycode = 0;
  uint8_t modifier = 0;
  char character = 0;
  int16_t x = 0;
  int16_t y = 0;
  bool repeat = false;
  uint32_t timestampMs = 0;
};

enum class SystemEventType : uint8_t {
  None,
  AppStarted,
  AppStopped,
  WifiConnected,
  WifiDisconnected,
  GpsFixUpdated,
  UsbDeviceConnected,
  UsbDeviceDisconnected,
  ThemeChanged,
};

struct SystemEvent {
  SystemEventType type = SystemEventType::None;
  uint32_t timestampMs = 0;
  const char *source = nullptr;
};

}  // namespace responder
