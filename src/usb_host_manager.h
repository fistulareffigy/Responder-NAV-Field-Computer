#pragma once

#include <Arduino.h>

enum class UsbHostState : uint8_t {
  OFF,
  POWERING,
  ENUMERATING,
  READY,
  STREAMING,
  ERROR,
  RECOVERY_PENDING,
  SHUTTING_DOWN,
};

enum class UsbHostOwner : uint8_t {
  NONE,
  ELM327,
  RTL_SDR,
  ADS_B,
  CAMERA,
  UTILITY,
};

struct UsbHostSnapshot {
  UsbHostState state;
  UsbHostOwner owner;
  int lastError;
  uint8_t recoveryAttempts;
  uint32_t stateSinceMs;
  bool hostInstalled;
  bool phyReady;
  bool deviceConnected;
};

class UsbHostManager {
 public:
  bool request(UsbHostOwner owner, uint32_t now);
  void release(UsbHostOwner owner, uint32_t now);
  bool transition(UsbHostState next, uint32_t now);
  void setInstalled(bool installed, bool phyReady, uint32_t now);
  void setDeviceConnected(bool connected, uint32_t now);
  void markError(int error, uint32_t now);
  bool requestRecovery(uint32_t now);
  void clearError(uint32_t now);
  void resetTracking(uint32_t now);
  UsbHostSnapshot snapshot() const;

  static const char *stateName(UsbHostState state);
  static const char *ownerName(UsbHostOwner owner);

 private:
  bool validTransition(UsbHostState from, UsbHostState to) const;

  mutable portMUX_TYPE mux_ = portMUX_INITIALIZER_UNLOCKED;
  UsbHostState state_ = UsbHostState::OFF;
  UsbHostOwner owner_ = UsbHostOwner::NONE;
  int lastError_ = 0;
  uint8_t recoveryAttempts_ = 0;
  uint32_t stateSinceMs_ = 0;
  bool hostInstalled_ = false;
  bool phyReady_ = false;
  bool deviceConnected_ = false;
};
