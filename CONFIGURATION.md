# Configuration

## Wi-Fi

Use Utilities > Wi-Fi to scan or enter a network manually. Credentials are saved in the Tab5 preferences partition and are not read from source files.

## Map tiles

The default tile server is configured by `/tile_server.txt` on the SD card. Enter the provider key from **Utilities > API Keys**. The value is masked on screen, stored in the Tab5 preferences partition, and never written to serial logs or included in File Transfer exports.

```text
/tile_server.txt
/boot/boot.png
/apps/
/tiles/
```

`/tile_key.txt` remains supported only as a legacy migration path. Do not place a real key in a repository or shared SD-card image. API usage, attribution, caching, and redistribution are governed by the selected map provider's terms.

Open-Meteo weather and the current adsb.lol Wi-Fi aircraft source do not require user API keys. Future providers that require credentials will be added to the same protected utility.

## Optional apps

Copy a supported package directory into `/apps`, then restart the Tab5. The boot log reports every accepted package and the final package count.

## Deploy Cam

The camera always exposes its own `DeployCam-xxxxxx` access point. Home Wi-Fi is optional and can be configured from its local setup page. Copy `deploy_cam_esp32cam/include/wifi_secrets.example.h` to `wifi_secrets.h` only if build-time defaults are desired; the real file is ignored by Git.
