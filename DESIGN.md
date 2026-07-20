---
version: "alpha"
name: "Tab5 Responder Terminal"
description: "Dark rugged responder-terminal UI for the M5Stack Tab5 firmware."
colors:
  primary: "#F2C15A"
  on-primary: "#070806"
  background: "#050604"
  surface: "#10150C"
  surface-muted: "#182012"
  outline: "#7D6830"
  outline-dim: "#3B3219"
  text: "#F2C15A"
  text-muted: "#A8893B"
  highlight: "#FFE89A"
  warning: "#D85A3A"
  success: "#8FCB5A"
typography:
  title:
    fontFamily: "Built-in bitmap / M5GFX"
    fontSize: "24px"
    fontWeight: "700"
    lineHeight: "1.1"
  label:
    fontFamily: "Built-in bitmap / M5GFX"
    fontSize: "18px"
    fontWeight: "600"
    lineHeight: "1.15"
  body:
    fontFamily: "Built-in bitmap / M5GFX"
    fontSize: "16px"
    fontWeight: "500"
    lineHeight: "1.2"
  small:
    fontFamily: "Built-in bitmap / M5GFX"
    fontSize: "13px"
    fontWeight: "500"
    lineHeight: "1.15"
rounded:
  none: "0px"
  sm: "3px"
  md: "6px"
spacing:
  xs: "4px"
  sm: "8px"
  md: "12px"
  lg: "18px"
  xl: "28px"
components:
  screen:
    backgroundColor: "{colors.background}"
    textColor: "{colors.text}"
    typography: "{typography.body}"
  top-bar:
    backgroundColor: "{colors.surface}"
    textColor: "{colors.highlight}"
    typography: "{typography.title}"
    padding: "8px"
  side-rail:
    backgroundColor: "{colors.background}"
    textColor: "{colors.primary}"
    rounded: "{rounded.none}"
    padding: "8px"
  panel:
    backgroundColor: "{colors.surface}"
    textColor: "{colors.text}"
    rounded: "{rounded.sm}"
    padding: "10px"
  button-primary:
    backgroundColor: "{colors.surface-muted}"
    textColor: "{colors.primary}"
    typography: "{typography.label}"
    rounded: "{rounded.sm}"
    padding: "10px"
  button-active:
    backgroundColor: "{colors.primary}"
    textColor: "{colors.on-primary}"
    typography: "{typography.label}"
    rounded: "{rounded.sm}"
    padding: "10px"
  button-danger:
    backgroundColor: "{colors.warning}"
    textColor: "{colors.on-primary}"
    typography: "{typography.label}"
    rounded: "{rounded.sm}"
    padding: "10px"
  status-ok:
    backgroundColor: "{colors.background}"
    textColor: "{colors.success}"
    typography: "{typography.small}"
  status-muted:
    backgroundColor: "{colors.background}"
    textColor: "{colors.text-muted}"
    typography: "{typography.small}"
  status-warning:
    backgroundColor: "{colors.background}"
    textColor: "{colors.warning}"
    typography: "{typography.small}"
---

## Overview

Tab5 Responder Nav should feel like a rugged field terminal: high-contrast, low-glare,
fast to scan, and stable under embedded display constraints. The UI should prioritize
legibility, predictable navigation, and minimal full-screen redraws over decorative effects.

The visual language is dark amber-on-black with compact panels, strong outlines, and large
touch/keyboard targets. Screens should look consistent across map, apps, RF tools, MeshCore,
Deploy Cam, Wi-Fi, and system utilities.

## Colors

Use the amber palette as the normal interaction language:

- `background` is the default screen clear color.
- `surface` and `surface-muted` are used for panels, list rows, and inactive buttons.
- `primary` is the main text and active-line color.
- `highlight` is reserved for titles, selected text, and temporary emphasis.
- `warning` is only for errors, destructive actions, signal loss, or dangerous state.
- `success` is only for confirmed OK/connected/ready state.

Avoid bright blue crash/reboot-style screens for normal app transitions.

## Typography

Use the built-in bitmap/M5GFX text system consistently. Do not shrink app-critical labels
below the `body` token unless the text is secondary telemetry. If a screen cannot fit the
content at readable sizes, use pagination, scrolling, or a secondary details screen.

## Layout

Landscape app screens should reserve clear functional zones:

- top bar: app title, mode, and one-line status;
- main content: the primary task area;
- side rail: persistent action buttons where needed;
- bottom/status panel: telemetry, progress, or current selection.

For map screens, rails must not be redrawn as part of map tile panning. Map tile rendering
should clip to the map viewport and never leave artifacts under the right-side rail.

For camera/video screens, the live image should be centered and scaled within the image
viewport. Telemetry belongs in the right rail and should not overlap the feed.

## Components

Buttons should be large enough to read at a glance on-device. Prefer fewer, clearer controls
per page over dense tiny menus. App menus can paginate, but page changes must be explicit
and stable.

Progress indicators should be used for scans, downloads, file transfer, camera connection,
and USB enumeration. A screen that is waiting for hardware should show state and next action,
not appear frozen.

## Do's and Don'ts

Do:

- use dirty rectangles or bounded redraw regions;
- keep background services visually quiet;
- preserve app state when returning from app menu;
- show hardware status directly when USB, Wi-Fi, BLE, GPS, LoRa, or SD is unavailable.

Don't:

- flash the whole screen for telemetry, chat, scan, or frame updates;
- use tiny unreadable fonts to fit more content;
- draw map/video content under rails or buttons;
- use placeholder rectangles when recognizable icons or sprites are required;
- make normal app entry/exit look like a crash or boot failure.
