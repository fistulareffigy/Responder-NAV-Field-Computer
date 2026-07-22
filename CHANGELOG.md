# Changelog

## Responder Nav v0.71 Beta - 2026-07-21

- Fixed extended UI stalls while traveling at road speed.
- Replaced blocking background Wi-Fi reconnect loops with asynchronous reconnect attempts.
- Reused the PSRAM map framebuffer for GPS-follow movement instead of repeatedly decoding the full map.
- Limited adjacent-tile prefetch work and paused it during fresh road-speed motion.
- Added age-aware vehicle-motion handling with a short GPS dropout hold.
- Added safe serial drive diagnostics for repeatable map-follow stress testing.
- Verified the release build and completed 90 synthetic road-motion shifts on Tab5 hardware without a crash or heartbeat loss.

## Responder Nav v0.7 Beta - 2026-07-20

- Improved the MeshCore network selector layout, added Enter-to-apply keyboard control, and replaced full-screen selection redraws with targeted row updates.
- Removed the development-only private MeshCore preset from public source and binaries.
- Added Utilities > About with version, author, project attribution, GitHub location, license, and no-warranty notice.
- Added Utilities > API Keys for public builds so every user supplies their own map credential.

- Improved GPS filtering and foreground scheduling while driving so the map remains responsive without accepting implausible position jumps.
- Added named-location entry on the map, including a double-Enter shortcut for saving an unnamed location and explicit save feedback.
- Added direct numeric frequency entry in RF Scan and a dedicated live RF audio settings panel for filtering, gain, bandwidth, and output level.
- Unified BLE, USB-app, RF Scan, and Wi-Fi restoration loading screens and deferred navigation until Wi-Fi restoration completes.
- Corrected calendar and clock validation so invalid GPS or network time cannot force the UI to year 2100.
- Fixed RF settings layering, RF exit-to-map ordering, and stale loading overlays after USB radio apps close.
- Sanitized compiler path prefixes so public firmware images do not embed the local build account or checkout path.
- Completed the v0.7 Beta privacy, credential, licensing, and release-artifact audit.

## 2026.07.18-rc1 - 2026-07-18

- Refreshed the public source from the current development firmware while retaining a public-safe built-in boot title font.
- Added a polished Utilities > API Keys screen for masked Thunderforest key entry and local NVS storage.
- Redacted API-key input, tile-server URLs, aircraft request coordinates, and connected SSIDs from serial diagnostics.
- Added a random per-session access token and strict session cookie to the SD File Transfer web interface.
- Added the current map loading, zoom loading, sustained keyboard panning, single-draw lock transition, and menu/UI refinements.
- Added the current BLE keyboard transition handling, Wi-Fi restore flow, USB accessory lifecycle fixes, and Deploy Cam performance updates.
- Removed fixed development COM ports, personal toolchain paths, logs, private map keys, and the device-only Glitch Goblin font from the public tree.
- Pinned current M5Unified, M5GFX, and TinyGPSPlus revisions for reproducible builds.

## Initial public workspace - 2026-07-17

- Licensed the public project under GPL-3.0-or-later and documented bundled third-party licenses.
- Prepared the first public source and documentation layout.
- Added factory and application-only release images with SHA-256 checksums.
- Added public app SDK and package-format documentation.
- Added Aircraft Radar, Deploy Cam, Drone Detection, Ghost Box, and RF Watch packages.
- Removed Wi-Fi Motion from the current app catalog pending a reliable implementation.
- Excluded private API keys, Wi-Fi credentials, development logs, and recovery material.
- Added the compact Deploy Cam raw JPEG stream and PSRAM-backed enlarged live view.
- Switched the companion camera build to the exact Freenove ESP32-WROVER board definition.
- Verified approximately 50 incoming camera frames per second and stable enlarged playback on Tab5 hardware.
