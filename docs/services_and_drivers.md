# Services and Drivers SDK Scaffold

This is the first service/driver API scaffold. It defines contracts only; most
existing firmware subsystems still use the legacy direct calls until they are
migrated incrementally.

## App services

Apps receive an `AppContext`. The context now exposes an `AppServices` bundle:

```cpp
responder::AppServices &services = context.services();
```

Defined service interfaces:

- `IDisplayService`
- `IInputService`
- `IStorageService`
- `ISettingsService`
- `IGpsService`
- `INetworkService`
- `IUsbHostService`
- `IAudioService`
- `INotificationService`
- `IResourceService`

The intent is that apps request shared resources through these services instead
of directly controlling hardware.

## Drivers

Drivers implement:

```cpp
class IResponderDriver {
public:
  virtual const DriverMetadata &metadata() const = 0;
  virtual bool begin(AppServices &services) = 0;
  virtual void update(uint32_t nowMs);
  virtual void end();
  virtual ServiceStatus status() const = 0;
};
```

The firmware now has a `DriverRegistry` scaffold. No hardware driver has been
migrated into it yet.

## Migration rule

Do not move RTL-SDR, ELM327, USB host, BLE, MeshCore, or Deploy Cam into the
driver registry until the service interfaces have been proven with lower-risk
drivers or read-only status surfaces.

