#include "usb_host_manager.h"

bool UsbHostManager::validTransition(UsbHostState from, UsbHostState to) const {
  if (from == to) return true;
  switch (from) {
    case UsbHostState::OFF:
      return to == UsbHostState::POWERING || to == UsbHostState::ERROR;
    case UsbHostState::POWERING:
      return to == UsbHostState::ENUMERATING || to == UsbHostState::ERROR ||
             to == UsbHostState::OFF;
    case UsbHostState::ENUMERATING:
      return to == UsbHostState::READY || to == UsbHostState::ERROR ||
             to == UsbHostState::RECOVERY_PENDING;
    case UsbHostState::READY:
      return to == UsbHostState::ENUMERATING || to == UsbHostState::STREAMING ||
             to == UsbHostState::ERROR || to == UsbHostState::RECOVERY_PENDING ||
             to == UsbHostState::SHUTTING_DOWN;
    case UsbHostState::STREAMING:
      return to == UsbHostState::READY || to == UsbHostState::ERROR ||
             to == UsbHostState::RECOVERY_PENDING;
    case UsbHostState::ERROR:
      return to == UsbHostState::RECOVERY_PENDING || to == UsbHostState::READY ||
             to == UsbHostState::OFF;
    case UsbHostState::RECOVERY_PENDING:
      return to == UsbHostState::ENUMERATING || to == UsbHostState::READY ||
             to == UsbHostState::ERROR;
    case UsbHostState::SHUTTING_DOWN:
      return to == UsbHostState::OFF || to == UsbHostState::ERROR;
  }
  return false;
}

bool UsbHostManager::request(UsbHostOwner owner, uint32_t now) {
  if (owner == UsbHostOwner::NONE) return false;
  portENTER_CRITICAL(&mux_);
  bool allowed = owner_ == UsbHostOwner::NONE || owner_ == owner;
  if (allowed) {
    owner_ = owner;
    if (state_ == UsbHostState::OFF) {
      state_ = UsbHostState::POWERING;
      stateSinceMs_ = now;
    }
  }
  portEXIT_CRITICAL(&mux_);
  return allowed;
}

void UsbHostManager::release(UsbHostOwner owner, uint32_t now) {
  portENTER_CRITICAL(&mux_);
  if (owner_ == owner) owner_ = UsbHostOwner::NONE;
  if (state_ == UsbHostState::STREAMING) {
    state_ = UsbHostState::READY;
    stateSinceMs_ = now;
  }
  portEXIT_CRITICAL(&mux_);
}

bool UsbHostManager::transition(UsbHostState next, uint32_t now) {
  portENTER_CRITICAL(&mux_);
  bool valid = validTransition(state_, next);
  if (valid) {
    state_ = next;
    stateSinceMs_ = now;
  }
  portEXIT_CRITICAL(&mux_);
  return valid;
}

void UsbHostManager::setInstalled(bool installed, bool phyReady, uint32_t now) {
  portENTER_CRITICAL(&mux_);
  hostInstalled_ = installed;
  phyReady_ = phyReady;
  if (!installed) {
    state_ = UsbHostState::OFF;
    deviceConnected_ = false;
    owner_ = UsbHostOwner::NONE;
  } else if (state_ == UsbHostState::POWERING || state_ == UsbHostState::OFF) {
    state_ = UsbHostState::ENUMERATING;
  }
  stateSinceMs_ = now;
  portEXIT_CRITICAL(&mux_);
}

void UsbHostManager::setDeviceConnected(bool connected, uint32_t now) {
  portENTER_CRITICAL(&mux_);
  deviceConnected_ = connected;
  if (connected && (state_ == UsbHostState::ENUMERATING ||
                    state_ == UsbHostState::RECOVERY_PENDING)) {
    state_ = UsbHostState::READY;
    stateSinceMs_ = now;
    recoveryAttempts_ = 0;
    lastError_ = 0;
  } else if (!connected && state_ == UsbHostState::STREAMING) {
    state_ = UsbHostState::ERROR;
    stateSinceMs_ = now;
  }
  portEXIT_CRITICAL(&mux_);
}

void UsbHostManager::markError(int error, uint32_t now) {
  portENTER_CRITICAL(&mux_);
  lastError_ = error;
  state_ = UsbHostState::ERROR;
  stateSinceMs_ = now;
  portEXIT_CRITICAL(&mux_);
}

bool UsbHostManager::requestRecovery(uint32_t now) {
  portENTER_CRITICAL(&mux_);
  bool allowed = recoveryAttempts_ < 2 && state_ != UsbHostState::SHUTTING_DOWN;
  if (allowed) {
    ++recoveryAttempts_;
    state_ = UsbHostState::RECOVERY_PENDING;
    stateSinceMs_ = now;
  }
  portEXIT_CRITICAL(&mux_);
  return allowed;
}

void UsbHostManager::clearError(uint32_t now) {
  portENTER_CRITICAL(&mux_);
  lastError_ = 0;
  recoveryAttempts_ = 0;
  state_ = hostInstalled_ ? UsbHostState::READY : UsbHostState::OFF;
  stateSinceMs_ = now;
  portEXIT_CRITICAL(&mux_);
}

void UsbHostManager::resetTracking(uint32_t now) {
  portENTER_CRITICAL(&mux_);
  lastError_ = 0;
  recoveryAttempts_ = 0;
  deviceConnected_ = false;
  state_ = hostInstalled_ ? UsbHostState::ENUMERATING : UsbHostState::OFF;
  stateSinceMs_ = now;
  portEXIT_CRITICAL(&mux_);
}

UsbHostSnapshot UsbHostManager::snapshot() const {
  portENTER_CRITICAL(&mux_);
  UsbHostSnapshot value = {state_, owner_, lastError_, recoveryAttempts_, stateSinceMs_,
                           hostInstalled_, phyReady_, deviceConnected_};
  portEXIT_CRITICAL(&mux_);
  return value;
}

const char *UsbHostManager::stateName(UsbHostState state) {
  switch (state) {
    case UsbHostState::OFF: return "OFF";
    case UsbHostState::POWERING: return "POWERING";
    case UsbHostState::ENUMERATING: return "ENUMERATING";
    case UsbHostState::READY: return "READY";
    case UsbHostState::STREAMING: return "STREAMING";
    case UsbHostState::ERROR: return "ERROR";
    case UsbHostState::RECOVERY_PENDING: return "RECOVERY PENDING";
    case UsbHostState::SHUTTING_DOWN: return "SHUTTING DOWN";
  }
  return "UNKNOWN";
}

const char *UsbHostManager::ownerName(UsbHostOwner owner) {
  switch (owner) {
    case UsbHostOwner::NONE: return "NONE";
    case UsbHostOwner::ELM327: return "ELM327";
    case UsbHostOwner::RTL_SDR: return "RTL-SDR";
    case UsbHostOwner::ADS_B: return "ADS-B";
    case UsbHostOwner::CAMERA: return "CAMERA";
    case UsbHostOwner::UTILITY: return "UTILITY";
  }
  return "UNKNOWN";
}
