# App development

Responder Nav has a staged modular architecture. Built-in apps use registry metadata and lifecycle callbacks. Optional SD packages currently act as manifests that enable optional modules already compiled into the firmware.

## Important limitation

The current release does **not** load arbitrary C++, ELF, WASM, or script executables from SD. A manifest alone cannot add new executable behavior. A new native app must still be compiled into the firmware and registered before its SD package can expose it.

## SDK headers

The public interfaces are under `include/responder/`:

- `IResponderApp.hpp` - lifecycle contract
- `AppContext.hpp` - app context and service access
- `AppMetadata.hpp` - identity, title, category, and API information
- `AppRegistry.hpp` - fixed-capacity registry
- `Events.hpp` - input and system events
- `Services.hpp` - display, input, storage, settings, network, and USB contracts
- `Permissions.hpp` - permission names
- `AppPackageManager.hpp` - SD manifest parsing

## Package layout

```text
/apps/my_app/app.json
```

Minimal manifest:

```json
{
  "id": "com.example.my_app",
  "name": "My App",
  "version": "0.1.0",
  "minimumApiVersion": 1,
  "permissions": ["display", "input"]
}
```

Use a stable reverse-domain ID. Request only the resources the app actually needs. USB, Wi-Fi, BLE, audio, and storage are shared resources and must be acquired and released through the platform lifecycle.

## Native app lifecycle

An app should:

1. Register metadata and callbacks.
2. Allocate heavy resources in `onStart`, not at firmware boot.
3. Draw the static frame once and update only dirty regions.
4. Stop scans, streams, tasks, sockets, and audio in `onStop`.
5. Release USB ownership before returning to the menu.
6. Avoid rebooting as a normal exit mechanism.

Read `docs/modular_platform_phase1.md`, `docs/services_and_drivers.md`, and `examples/hello_responder/` for the current scaffold.

## Submission checklist

- Builds with `esp32p4_pioarduino55`
- No credentials, API keys, or personal paths
- No full-screen redraw loop when idle
- Enter/exit tested repeatedly
- Wi-Fi reconnect behavior tested when applicable
- USB ownership released on every exit and failure path
- Serial log contains no panic, watchdog, or reboot loop
- Package manifest uses API version 1
