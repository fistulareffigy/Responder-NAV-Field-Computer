# Optional App Packages

The base firmware apps are built in. They do not live on the SD card and do not
need entries under `/apps`.

The `/apps` folder is only for optional add-on packages. In the current firmware
generation, those packages enable optional modules that are already compiled into
the firmware. Arbitrary executable code loading from SD is not enabled.

## SD-card location

Put future app packages under:

```text
/apps
```

Each app gets its own folder:

```text
/apps/<app-id>/app.json
/apps/<app-id>/assets/
/apps/<app-id>/data/
```

Example optional package:

```text
/apps/aircraft_radar/app.json
/apps/aircraft_radar/assets/icon.qoi
/apps/aircraft_radar/data/settings.json
```

## Current firmware behavior

At boot and SD remount, the firmware:

1. Creates `/apps` if it does not exist.
2. Scans direct child folders.
3. Opens folders containing `app.json`.
4. Parses the basic manifest fields into a fixed in-memory discovery table.
5. Logs discovered packages as `APPREG: SD package id=... name=... version=...`.
6. Shows matching optional app modules in the app menu.

It does not load or execute arbitrary external code.

## Manifest

The expected manifest filename is:

```text
app.json
```

Currently parsed fields:

- `id`
- `name`
- `version`
- `minimumApiVersion`
- `permissions`

If `id` is missing, the folder name is used as a fallback. If `name` or
`version` are missing, the firmware uses the app id and `unknown`.
Packages that request a newer API than the firmware supports are ignored until
the loader can present a compatibility warning.

The `permissions` array is bounded to the first eight string entries. It is
recorded for future review/approval UI only; it does not grant access yet.

The parser is implemented in the core package manager module.

Optional package templates are staged under `sdcard/app_packages_available`.

## Next loader steps

- Add an app management UI for installing/removing optional packages.
- Add permission review.
- Consider safe script/native module loading only after sandbox/resource rules are in place.
