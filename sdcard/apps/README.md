# Responder Nav optional SD apps

This folder is scanned by the firmware at boot as `/apps`.

It is normal for this folder to be empty.

The base firmware apps are built in and do not live here:

- Trips/Locs
- Calendar
- Camera
- Gallery
- Weather
- Recorder
- Audio
- Games
- System Health
- File Transfer
- MeshCore
- Notes
- BLE Keyboard
- RF Scan
- Car Scanner

Only optional add-on packages go here. Copy packages from `/app_packages_available` into `/apps` to make those optional apps appear in the app menu.
