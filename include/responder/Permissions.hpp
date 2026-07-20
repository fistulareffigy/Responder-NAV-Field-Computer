#pragma once

namespace responder::permissions {

constexpr const char *Display = "display";
constexpr const char *Input = "input";
constexpr const char *StorageRead = "storage.read";
constexpr const char *StorageWrite = "storage.write";
constexpr const char *SettingsRead = "settings.read";
constexpr const char *SettingsWrite = "settings.write";
constexpr const char *GpsRead = "gps.read";
constexpr const char *GpsTelemetrySend = "gps.telemetry.send";
constexpr const char *NetworkLocal = "network.local";
constexpr const char *UsbRequest = "usb.request";
constexpr const char *SensorRead = "sensor.read";
constexpr const char *Notifications = "notifications";

}  // namespace responder::permissions
