# Responder NAV Field Computer

**Responder Nav v0.7 Beta** is an expandable, offline-capable field computer for the M5Stack Tab5. It brings navigation, off-grid communications, radio utilities, vehicle diagnostics, local file transfer, and practical field tools into one landscape terminal interface.

> Beta software: test critical workflows before relying on them. Responder NAV is not emergency, life-safety, aviation, or professional vehicle-diagnostic equipment.

## Design philosophy

Responder NAV is built around four ideas:

- Offline first
- Expandable by design
- Practical field utilities
- A focused, purpose-built user experience

It is not intended to replace a smartphone, laptop, certified scanner, or SDR workstation. The goal is a responsive field device that can continue to provide useful local tools when Internet service is unavailable.

## Photos

![Responder NAV image 2](https://github.com/user-attachments/assets/14b9f67e-42fb-4c98-8965-b2492a8dfcae)

![Responder NAV image 3](https://github.com/user-attachments/assets/0bba136d-f6e3-4597-b15f-a24e4516bbf2)

## Current features

### Navigation and field utilities

- GPS map navigation, manual panning, saved locations, and trip logging
- Weather, calendar, notes, gallery, audio recorder, and system health
- Token-protected local SD-card file transfer from a browser
- Custom lock screen and terminal-style UI

### Communications

- MeshCore-compatible LoRa messaging, position sharing, telemetry, and selectable radio presets
- BLE keyboard mode
- Wi-Fi configuration and reconnect handling

### RF and diagnostics

- RTL2832U/R820T2 RTL-SDR support, FM reception, RF scanning, Ghost Box, TPMS monitoring, and aircraft utilities
- USB ELM327 OBD-II diagnostics
- Optional Freenove ESP32-WROVER Deploy Cam companion firmware

Wi-Fi Motion and Wasteland Responder are intentionally excluded from v0.7 Beta. Hardware-dependent features vary with the exact module, USB adapter, antenna, hub, vehicle, region, and radio environment.

## Hardware

Core hardware:

- M5Stack Tab5 and Tab5 keyboard
- microSD card
- M5Stack Module GPS V2.0 w/ External Antenna (AT6668)
- M5Stack U184-US915 LORAWAN-US915 module

Optional peripherals:

- RTL2832U/R820T2 RTL-SDR
- USB ELM327 adapter
- Freenove FNK0060/FNK0060B ESP32-WROVER camera

See [HARDWARE.md](HARDWARE.md) for connection and compatibility notes.

## Build

1. Install VS Code with PlatformIO or PioArduino.
2. Clone this repository.
3. Open the repository root.
4. Build `esp32p4_pioarduino55`.

```powershell
pio run -e esp32p4_pioarduino55
```

Detailed setup is in [BUILDING.md](BUILDING.md). Prebuilt image usage is in [FLASHING.md](FLASHING.md).

## First start

1. Format a microSD card as FAT32 or exFAT.
2. Copy `sdcard/` to the card root.
3. Insert the card and boot the Tab5.
4. Open **Utilities > API Keys** and enter your own Thunderforest map key.
5. Open **Utilities > Wi-Fi** and configure your own network.

The release contains no personal SSID, Wi-Fi password, API key, private MeshCore network, location history, or development font. Credentials entered on the device are stored locally in preferences; the standard public image does not enable flash encryption.

## Optional apps

The v0.7 package system reads `/apps/<package>/app.json` manifests from the SD card to expose optional modules already compiled into the firmware. It does not load arbitrary native binaries from SD.

See [APP_DEVELOPMENT.md](APP_DEVELOPMENT.md) and [docs/external_app_packages.md](docs/external_app_packages.md).

## File transfer

Open **File Transfer** on the Tab5 and enter the complete protected URL shown on its screen from another device on the same trusted network. The web UI can browse folders, upload, download, create folders, and delete files. Closing the app stops the server and invalidates the session token.

The connection uses plain HTTP rather than TLS. Do not expose port 8080 to the Internet.

## Repository layout

- `src/` — Tab5 firmware
- `include/responder/` — application and service interfaces
- `lib/` — project-local hardware libraries
- `partitions/` — Tab5 partition layout
- `sdcard/` — public-safe starter SD-card tree
- `apps/` — optional v0.7 Beta app manifests ready to copy to SD
- `examples/` — app package examples
- `deploy_cam_esp32cam/` — companion camera firmware
- `firmware/` — versioned release images and checksums
- `tools/` — serial regression helpers

## Development and attribution

Responder NAV was designed by Alberto Cajiao Hernandez / ACH Industries. Its hardware architecture, product direction, feature set, and user experience were developed through hands-on embedded prototyping, with OpenAI Codex used as an AI-assisted implementation, debugging, refactoring, and documentation tool.

M5Stack, OpenAI, Codex, MeshCore, ESP-IDF, RTL-SDR, and other referenced names are trademarks or projects of their respective owners. Their mention describes compatibility or tooling and does not imply sponsorship or endorsement.

## Security, safety, and responsible use

- Read [SECURITY.md](SECURITY.md) before enabling local web services.
- Remove credentials and precise locations from shared serial logs.
- Follow local radio, privacy, aviation, traffic, and vehicle-access laws.
- Do not operate the interface while driving; have a passenger use it or stop safely.
- Do not use decoded radio or vehicle data as the sole basis for a safety-critical decision.

## Contributing

Bug reports should include the firmware version, hardware model, connection method, reproduction steps, and a sanitized 115200-baud serial log. See [CONTRIBUTING.md](CONTRIBUTING.md).

## License

Unless marked otherwise, original Responder NAV source, documentation, and UI assets are licensed under **GPL-3.0-or-later**; see [LICENSE](LICENSE). If you distribute a firmware binary, provide the corresponding source used to build it through an equally accessible location. Bundled components retain their own compatible licenses and notices in [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md).

This software is provided without warranty. See [LEGAL.md](LEGAL.md) for the practical release and trademark notes.
