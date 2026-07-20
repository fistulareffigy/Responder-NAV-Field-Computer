# Flashing Responder Nav

## Recommended release image

Use the factory image from the release package when installing on a fresh Tab5. It contains the bootloader, partition table, and application at their correct offsets.

```powershell
esptool --chip esp32p4 --port COM_PORT --baud 460800 write-flash 0x0 Responder-Nav-v0.7-Beta-factory.bin
```

Replace `COM_PORT` with the actual Tab5 port.

## Application-only update

The application-only binary is written at `0x10000` and is intended for devices that already have the matching bootloader and partition table:

```powershell
esptool --chip esp32p4 --port COM_PORT --baud 460800 write-flash 0x10000 Responder-Nav-v0.7-Beta-app.bin
```

Do not use the application-only image for a blank device. Compare downloads against `SHA256SUMS.txt` before flashing.

## Deploy Cam companion firmware

The companion image targets the Freenove FNK0060/FNK0060B ESP32-WROVER camera.
Use its CH340 serial port, not the Tab5 port:

```powershell
esptool --chip esp32 --port CAMERA_PORT --baud 115200 write-flash 0x0 Deploy-Cam-v0.7-Beta-Freenove-factory.bin
```

For a camera that already has the matching bootloader and partition table:

```powershell
esptool --chip esp32 --port CAMERA_PORT --baud 115200 write-flash 0x10000 Deploy-Cam-v0.7-Beta-Freenove-app.bin
```

Replace `CAMERA_PORT` with the camera's actual port. If automatic reset cannot enter
the bootloader, hold `BOOT`, tap `RST`, start the flash, then release `BOOT` once
writing begins.

## Serial diagnostics

Responder Nav logs at 115200 baud. A healthy boot includes `BOOT: start`, SD status, app package scan, Wi-Fi state, GPS data, and periodic `HB: alive` lines. A repeated ROM boot header, panic, Guru Meditation, or watchdog message indicates a reset that should be included in a bug report.
