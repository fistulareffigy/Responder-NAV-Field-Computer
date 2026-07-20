# Modular Platform Phase 1

This phase adds the first internal SDK layer without rewriting the firmware.
Legacy apps still run through the existing `main.cpp` paths.

## Added SDK interfaces

- `responder/AppMetadata.hpp` — stable metadata for built-in and future external apps.
- `responder/AppContext.hpp` — shared app context and service placeholders.
- `responder/Events.hpp` — input and system event structures.
- `responder/IResponderApp.hpp` — app lifecycle interface.
- `responder/AppRegistry.hpp` — fixed-size app registry for built-in and legacy apps.
- `responder/Permissions.hpp` — initial permission string constants.
- `responder/AppPackage.hpp` — SD-card external package path constants.
- `responder/AppPackageManager.hpp` — SD-card package manifest parsing helpers.
- `responder/Services.hpp` — service interfaces for controlled app access to system features.
- `responder/Driver.hpp` — driver interface and fixed-size driver registry.

## Current registry behavior

`registerBuiltInResponderApps()` registers:

- Legacy metadata for the existing apps.
- A migrated System Health app wrapper.
- A migrated Weather app wrapper.
- A migrated Notes app wrapper.
- A migrated Calendar app wrapper.

The Apps menu reads labels from the registry when available and falls back to
the existing hard-coded labels. This keeps the current UI working while letting
new app entries be introduced incrementally.

## Migrated app

System Health is wrapped by `src/apps/system_health_app.cpp`.
Weather is wrapped by `src/apps/weather_app.cpp`.
Notes is wrapped by `src/apps/notes_app.cpp`.
Calendar is wrapped by `src/apps/calendar_app.cpp`.
These still use the existing renderers, but lifecycle entry now goes through
`IResponderApp`.

## Rules for the next phases

- Do not migrate Map, MeshCore, RTL-SDR, OBD2, Ghost, or camera until the app
  registry path has stayed stable.
- Keep each migrated app buildable after every step.
- New shared hardware access should be exposed as services or drivers, not
  copied directly into app files.
- Preserve the existing screen layout and navigation until external packages
  are introduced.
- Migrate hardware access through service/driver interfaces only after the low-risk app wrappers are stable.

## SD package path

Optional app packages live under `/apps` on the Tab5 SD card. The firmware
creates and scans this folder, parses basic `app.json` metadata, and uses
matching package IDs to reveal optional modules in the app menu. Arbitrary
external app execution is not enabled. See `docs/external_app_packages.md`.

The manifest parser now lives in `src/core/AppPackageManager.cpp`; `main.cpp`
still owns SD locking and boot/remount scan timing.

## Service and driver scaffold

The SDK now defines system service interfaces and a driver registry scaffold.
See `docs/services_and_drivers.md`.
