# Changelog

## Responder Nav v0.73 Beta UI maintenance update - 2026-08-23

- Reworked the MeshCore footer as a single coherent region so navigation hints and action buttons no longer overlap or leave stale pixels behind.
- Increased MeshCore row-label and action-button legibility while keeping long values clipped inside their assigned columns.
- Replaced the oversized identity export payload with a compact profile-ready status and shortened device-ID preview.
- Placed RF Scan RTL status and tuned-frequency summaries side by side to recover vertical space for larger tuning, volume, audio-settings, and mode controls.
- Added bounded RF status text and concise frequency-entry, step, level, TPMS, and key-fob monitoring details.
- Built, flashed, and exercised the MeshCore menu, identity, messages, keyboard selection, RF Scan entry, Wi-Fi restoration, and memory/heartbeat paths on Tab5 hardware.

## Responder Nav v0.73 Beta maintenance update - 2026-08-20

- Fixed intermittent Wi-Fi scans by waiting for the ESP-Hosted scan-complete event and clearing stale driver scan state between attempts.
- Replaced the blocking saved-network reconnect loop with responsive asynchronous Wi-Fi enable and bounded per-network attempts.
- Made the last successful saved network the preferred target after Wi-Fi is disabled and re-enabled.
- Added an immediate `ENABLING WIFI` state so the Wi-Fi Control screen remains responsive during radio startup.
- Re-ran the public privacy gate, CodeQL analysis, credential/path checks, and release-binary marker scan; no private credentials or development-only identifiers were detected.
- Hardened File Transfer with one-time 128-bit bootstrap authentication, a separate exact-match session cookie, Host validation, POST-only deletion, and restrictive browser security headers.
- Removed disabled TLS verification from weather and default map downloads, and moved the Wi-Fi aircraft feed to certificate-validated HTTPS.
- Documented the reversible software-only security model, including the explicit decision not to burn security eFuses and the remaining physical-access and Deploy Cam limitations.
- Restricted GitHub Actions to read-only repository contents and hardened vendored USB MSC READ/WRITE(10) length validation to clear the open CodeQL findings.
- Bounded File Transfer request lines, header count, header bytes, and header parsing time; invalid or incomplete requests now fail closed.
- Closed registered app services during direct navigation so File Transfer no longer leaves TCP port 8080 listening after exit.
- Redacted custom map-server URLs from serial output and pinned GitHub Actions, PlatformIO, and the optional design tool to reviewed versions.

## Responder Nav v0.73 Beta - 2026-07-30

- Corrected Wi-Fi clock synchronization so a plausible but stale cached epoch cannot be mistaken for a fresh SNTP update.
- Made the map sidebar use the same validated, advancing local clock as Calendar instead of an invalid GPS time field.
- Matched the initial atomic Apps-menu footer texture to subsequent page redraws so its dotted pattern appears immediately.
- Fixed RF/USB exit handoff so the foreground app cannot reclaim USB while Wi-Fi restoration is still in progress.
- Required a real Wi-Fi connection before dismissing the restoration screen and added deferred app navigation after restoration.
- Prevented Deploy Cam low-memory recovery from tearing down Wi-Fi while the live stream task still owns its socket.
- Held the Tab5 backlight off across managed BLE resets to suppress the controller's brief default blue frame.
- Polished the RF Scan frequency panel, MeshCore contact rail, Car Scanner metrics, and File Manager navigation redraws.
- Protected active-zoom map tiles from speculative neighboring-zoom cache eviction and added framebuffer retry backoff.
- Hardware-tested map panning in four directions, key release without continued drift, recenter, and zoom 14 to 15 to 14.
- Repeated the public-source, Git-history, and release-binary privacy scan before publication.

## Responder Nav v0.72 Beta - 2026-07-27

- Added reliable background MeshCore direct-message reception with unread indicators on the Apps navigation button and MeshCore app tile.
- Added reciprocal contact telemetry requests and clipped contact-marker updates for the main map.
- Added adjustable MeshCore transmit power and telemetry-sharing controls; public builds default telemetry sharing to off.
- Removed synchronous MeshCore history writes from the live RX/TX path to prevent RGB display underruns and blue flashes.
- Reworked MeshCore menu navigation and targeted redraws so selections, messages, telemetry, and discovery do not clear the full display.
- Added a bounded adjacent-zoom tile warmer for faster map zoom changes without displacing the active map view.
- Improved map marker cleanup, panning, menu transitions, loading screens, and UI redraw behavior.
- Verified direct T-Deck messages, acknowledgements, telemetry requests, stable heartbeat, and healthy memory over live serial hardware testing.

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
