Responder Nav SD Card Layout

Required:
- /boot/boot.png
- /tiles/{z}/{x}/{y}.png

Optional:
- /tile_server.txt (base URL for WiFi tile downloads, no trailing slash)
- /tile_key.txt (legacy only; prefer Utilities > API Keys)
- /ui/apps_menu_bg.png
- /ui/utilities_menu_bg.png
- /ui/lock_screen_bg.png
- /waypoints.txt (auto-generated after first save)

Notes:
- Copy the provided boot image from `sdcard/boot/boot.png` to the SD card.
- Tiles should mirror the folder structure you already have (e.g. `/tiles/10/180/396.png`).
- If `tile_server.txt` is set, missing tiles will be fetched on demand.
- Enter a map key on-device from Utilities > API Keys. Never copy a real key into a public SD-card image.
