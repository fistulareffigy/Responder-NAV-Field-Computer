# Hardware reference

## Tab5 connections

| Function | Connection |
| --- | --- |
| GPS RX | GPIO 7 |
| GPS TX | GPIO 6 |
| GPS UART | Port 2, 115200 baud |
| microSD MISO | GPIO 39 |
| microSD CS | GPIO 42 |
| microSD SCK | GPIO 43 |
| microSD MOSI | GPIO 44 |

## USB-A peripherals

RTL-SDR and USB ELM327 use the Tab5 USB host controller. The resource manager gives ownership to the foreground app so both drivers do not compete for the same device. Allow an app to finish its exit transition before opening another USB app.

Known RTL-SDR target hardware is an RTL2832U receiver with an R820T/R820T2 tuner. ELM327 support targets compatible USB serial bridges and still depends on the vehicle and adapter protocol support.

## Deploy Cam

The companion firmware targets the Freenove ESP32-WROVER camera board (`CAMERA_MODEL_WROVER_KIT`). See `deploy_cam_esp32cam/README.md` for flashing and hotspot configuration.

## Power

High-current USB peripherals can exceed what an unpowered hub or cable can reliably deliver. Firmware recovery can reset the USB host state but cannot compensate for voltage drop or a marginal cable.
