#pragma once

#include <Arduino.h>

namespace responder {

struct ServiceStatus {
  bool available = false;
  bool busy = false;
  const char *state = nullptr;
};

class IDisplayService {
public:
  virtual ~IDisplayService() = default;
  virtual int16_t width() const = 0;
  virtual int16_t height() const = 0;
  virtual void requestRedraw() = 0;
};

class IInputService {
public:
  virtual ~IInputService() = default;
  virtual bool keyboardAvailable() const = 0;
  virtual bool touchAvailable() const = 0;
};

class IStorageService {
public:
  virtual ~IStorageService() = default;
  virtual ServiceStatus status() const = 0;
  virtual const char *appRoot() const = 0;
};

class ISettingsService {
public:
  virtual ~ISettingsService() = default;
  virtual bool getString(const char *key, String &value) = 0;
  virtual bool setString(const char *key, const String &value) = 0;
  virtual bool getBool(const char *key, bool defaultValue) = 0;
  virtual bool setBool(const char *key, bool value) = 0;
};

class IGpsService {
public:
  virtual ~IGpsService() = default;
  virtual ServiceStatus status() const = 0;
  virtual bool hasFix() const = 0;
  virtual bool location(double &lat, double &lon) const = 0;
  virtual uint8_t satellites() const = 0;
};

class INetworkService {
public:
  virtual ~INetworkService() = default;
  virtual ServiceStatus wifiStatus() const = 0;
  virtual bool wifiConnected() const = 0;
  virtual int32_t wifiRssi() const = 0;
};

class IUsbHostService {
public:
  virtual ~IUsbHostService() = default;
  virtual ServiceStatus status() const = 0;
  virtual bool requestAccess(const char *driverId, uint32_t timeoutMs) = 0;
  virtual void releaseAccess(const char *driverId) = 0;
};

class IAudioService {
public:
  virtual ~IAudioService() = default;
  virtual ServiceStatus status() const = 0;
  virtual void stopForegroundAudio() = 0;
};

class INotificationService {
public:
  virtual ~INotificationService() = default;
  virtual void toast(const char *message, uint32_t durationMs = 1500) = 0;
};

class IResourceService {
public:
  virtual ~IResourceService() = default;
  virtual ServiceStatus status() const = 0;
  virtual bool requestForegroundMode(const char *appId) = 0;
  virtual void releaseForegroundMode(const char *appId) = 0;
};

struct AppServices {
  IDisplayService *display = nullptr;
  IInputService *input = nullptr;
  IStorageService *storage = nullptr;
  ISettingsService *settings = nullptr;
  IGpsService *gps = nullptr;
  INetworkService *network = nullptr;
  IUsbHostService *usbHost = nullptr;
  IAudioService *audio = nullptr;
  INotificationService *notifications = nullptr;
  IResourceService *resources = nullptr;
};

}  // namespace responder
