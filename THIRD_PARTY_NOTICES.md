# Third-party notices

Responder Nav includes or depends on third-party software. The original
copyright and license notices remain in their source files. This summary is
provided for release clarity and does not replace those notices.

## R820T/R828D tuner driver

Files: `src/tuner_r82xx.c`, `src/tuner_r82xx.h`

Copyright (C) 2013 Mauro Carvalho Chehab and Steve Markgraf.

License: GNU General Public License, version 2 or (at your option) any later
version (`GPL-2.0-or-later`). See the headers in the listed source files.

## Espressif camera and video components

Directories: `lib/esp_cam_sensor`, `lib/esp_sccb_intf`, `lib/esp_video`,
`lib/esp_ipa`

Copyright Espressif Systems (Shanghai) CO LTD.

The individual files identify their licenses with SPDX headers. The camera
sensor and SCCB sources are primarily `Apache-2.0`; the video and IPA sources
are primarily Espressif's MIT license. The bundled Linux V4L2 UAPI headers are
dual licensed as stated in their own SPDX headers.

## USB Host MSC component

Directory: `lib/UsbHostMsc`

Derived from Espressif's `esp-usb` USB Host MSC component.

Copyright Espressif Systems (Shanghai) CO LTD.

License: Apache License 2.0 (`Apache-2.0`). The upstream source is
<https://github.com/espressif/esp-usb/tree/master/host/class/msc/usb_host_msc>.

## MeshCore

Directory: `lib/MeshCoreCore`

MeshCore is distributed under the MIT License. Upstream source:
<https://github.com/meshcore-dev/MeshCore>.

The directory also contains Ed25519 implementation files with their own zlib
license notice in `lib/MeshCoreCore/src/license.txt`.

## PlatformIO dependencies

The build downloads additional libraries declared in `platformio.ini`,
including M5Unified, M5GFX, TinyGPSPlus, SdFat, Crypto, and base64. Those
packages remain subject to their respective upstream licenses and notices.

At the revisions resolved by `platformio.ini`:

- M5Unified and M5GFX are MIT licensed; copyright M5Stack.
- TinyGPSPlus is LGPL-2.1-or-later; copyright Mikal Hart. Its license notice is retained in the library source headers.
- SdFat is MIT licensed; copyright Bill Greiman.
- Arduino Crypto is MIT licensed; copyright Southern Storm Software / Rhys Weatherley.
- base64 is MIT licensed; copyright Densaugeo.

M5Unified, M5GFX, and the M5Stack TinyGPSPlus fork are pinned to exact Git
revisions in `platformio.ini`. The other resolved packages and their source
metadata are supplied through the pinned PioArduino/Arduino-ESP32 platform.
Review the resolved dependency tree and downloaded notices whenever a revision
changes.

Espressif publishes source and license information for `esp_cam_sensor`,
`esp_sccb_intf`, `esp_video`, and `esp_ipa` at
<https://github.com/espressif/esp-video-components>. The repository identifies
the first two as Apache-2.0 and the latter two as ESPRESSIF MIT.

## Trademarks and attribution

Product and project names are used only to describe dependencies,
interoperability, or development tooling. No third-party endorsement is
claimed. See [LEGAL.md](LEGAL.md).
