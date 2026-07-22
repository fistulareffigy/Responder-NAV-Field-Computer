# Deploy Cam ESP32-CAM Firmware

Companion firmware for the Tab5 `DEPLOY CAM` app.

Target board:

- Freenove FNK0060 / FNK0060B ESP32-WROVER CAM
- Arduino camera model: `CAMERA_MODEL_WROVER_KIT`
- PlatformIO board: `esp-wrover-kit`
- USB serial: built-in CH340, no external FTDI adapter required

Endpoints:

- `http://<ip>/capture` - single JPEG frame
- `http://<ip>/stream` - MJPEG live stream
- `http://<ip>/status` - status JSON
- `http://<ip>/quality?value=10` - JPEG quality, lower is better quality

Wi-Fi setup:

The camera always starts its own hotspot:

- SSID: `DeployCam-xxxxxx`
- Password: `deploycam`

The v0.71 Beta setup password is public and is not a security boundary. Use the
camera AP only as a local deployment link, do not expose it to the Internet,
and do not use it where an untrusted nearby device could access sensitive video.
- Setup URL: `http://192.168.4.1/`

From the setup page you can:

- stay hotspot-only;
- enter home Wi-Fi credentials so the camera also joins the local LAN;
- view `/capture`, `/stream`, and `/status`.

The hotspot remains available even when local Wi-Fi is enabled, so field setup does not require a router.

Optional compile-time Wi-Fi defaults:

1. Copy `include/wifi_secrets.example.h` to `include/wifi_secrets.h`.
2. Put your Wi-Fi SSID/password in `wifi_secrets.h`.
3. Build/upload.

Runtime settings saved from the web setup page override compile-time defaults.

Serial port / upload note:

- The Freenove board should appear as a CH340 serial port. Use `pio device list` to distinguish it from the Tab5.
- Use the board's BOOT and RST buttons instead of wiring GPIO0 manually.
- If upload hangs at connecting:
  1. Hold `BOOT`.
  2. Tap/release `RST`.
  3. Keep holding `BOOT` until upload starts writing.
  4. Release `BOOT`.
- After upload, tap `RST` once to boot normally.

Build:

```powershell
cd deploy_cam_esp32cam
pio run
```

Upload after identifying the ESP32-CAM COM port:

```powershell
cd deploy_cam_esp32cam
pio run -t upload --upload-port CAMERA_PORT
```
