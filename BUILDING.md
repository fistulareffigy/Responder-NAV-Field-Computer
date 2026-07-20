# Building the firmware

## Requirements

- Windows, Linux, or macOS
- Python 3.10 or newer
- PlatformIO CLI, PlatformIO IDE, or PioArduino
- Git
- A data-capable USB cable

The tested environment is `esp32p4_pioarduino55`, using the PioArduino ESP32 platform and `m5stack-tab5-p4` board definition.

## Build commands

```powershell
pio run -e esp32p4_pioarduino55
```

The main application image is generated at `.pio/build/esp32p4_pioarduino55/firmware.bin`.

## Upload and monitor

List attached serial devices, then pass the detected port explicitly when auto-detection is ambiguous:

```powershell
pio device list
pio run -e esp32p4_pioarduino55 -t upload --upload-port COM_PORT
pio device monitor --port COM_PORT --baud 115200
```

Replace `COM_PORT` with the port reported on your computer. Linux and macOS use device names such as `/dev/ttyACM0` or `/dev/cu.usbmodem...`.

The public `platformio.ini` contains no fixed serial port or user-specific toolchain path. Library commits are pinned for reproducible release builds.

## Clean build

```powershell
pio run -e esp32p4_pioarduino55 -t clean
pio run -e esp32p4_pioarduino55
```

If the editor's C++ language server crashes, that does not by itself mean the firmware source is damaged. Use the PlatformIO build result as the authoritative compiler check.
