# Contributing to Responder Nav

Thanks for helping improve Responder Nav. Hardware-dependent changes need enough
evidence to distinguish firmware defects from peripheral, power, and USB-host
conditions.

## Before opening an issue

1. Check existing issues for the same symptom.
2. Reproduce with the current release build when practical.
3. Capture a 115200-baud serial log covering startup and the failure.
4. Remove Wi-Fi credentials, API keys, location history, and other private data.

Include the Tab5 firmware version, peripheral model, connection method, power
source, and exact enter/exit sequence. For USB problems, state whether the
device was connected directly or through a hub.

## Pull requests

Keep changes focused and explain the user-visible result. Before submitting:

```powershell
pio run -e esp32p4_pioarduino55
pio run -d deploy_cam_esp32cam -e freenove_esp32_wrover
```

Only the second command is required when changing Deploy Cam alone.

Also verify that the change:

- contains no credentials, API keys, personal paths, or captured user data;
- does not continuously redraw an idle screen;
- releases USB, Wi-Fi, BLE, audio, camera, and storage resources on exit;
- does not use a reboot as normal app navigation;
- has been tested through several enter/exit cycles when it changes an app;
- updates relevant documentation and the changelog.

## Optional apps

Read [APP_DEVELOPMENT.md](APP_DEVELOPMENT.md) before proposing an app. The
current SD package format enables modules compiled into the firmware; it is not
yet a general native-code loader.

