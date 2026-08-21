# claude-desktop-buddy — Agent Guide

Reference firmware for a Claude "desk pet": an M5StickC Plus (ESP32) that shows
Claude session state on its screen and lets you approve/deny permission prompts
from the device, talking to the Claude desktop apps over BLE.

## Stack

| Layer      | Technology                                                        |
| ---------- | ----------------------------------------------------------------- |
| Language   | C++ (Arduino framework)                                           |
| Target     | ESP32 — M5StickC Plus (board `m5stick-c`, env `m5stickc-plus`)     |
| Build      | PlatformIO Core 6.x (`platformio.ini`)                            |
| Firmware   | M5StickCPlus, AnimatedGIF ^2.1.1, ArduinoJson ^7.0.0 (`lib_deps`) |
| Tooling    | Python 3 scripts in `tools/` (Pillow for GIF prep)                |
| Radio      | BLE Nordic UART Service — wire contract in `REFERENCE.md`         |

No web app, no server, no database, no ports. The "app" is firmware that runs
on the device; the Claude desktop app connects to it over BLE.

## Commands

```bash
pio run                                # build firmware (works without hardware)
pio run -t upload                      # flash to a connected stick (hardware)
pio run -t erase && pio run -t upload  # wipe then reflash (hardware)
pio run -t uploadfs                    # flash LittleFS data/ partition (hardware)
pio device monitor                     # serial monitor @ 115200 (hardware)
python3 tools/prep_character.py <dir>  # normalize/resize a character pack to 96px
python3 tools/flash_character.py characters/bufo  # stage + flash a pack (hardware)
python3 tools/test_serial.py           # cycle device states over serial (hardware)
python3 tools/test_xfer.py [src] [nm]  # stream pack over serial, check acks (hardware)
```

Commands marked **(hardware)** need a physical M5StickC Plus on USB and cannot
run in a sandbox. Everything else is fully sandbox-verifiable.

## Environment

No env vars required. No `.env` file. The first `pio run` downloads the
espressif32 platform, toolchain, and `lib_deps` into `.pio/` (gitignored,
already cached in the snapshot).

## Local verification

The build is the health check:

```bash
pio run
```

Expected: `[SUCCESS]` plus a size report. Onboarding run (2026-08-21):
RAM 22.4% (73,500 / 327,680 B), Flash 64.7% (1,357,877 / 2,097,152 B),
artifacts in `.pio/build/m5stickc-plus/` (`firmware.bin`, `bootloader.bin`,
`partitions.bin`).

Secondary check (no hardware): run `tools/prep_character.py` on a copy of
`characters/bufo` — regenerates a 15-GIF pack (~595 KB, under the 1.8 MB cap).

There is no lint, typecheck, or unit-test configuration in this repo, and no CI.

### Local Verification Summary (onboarding run 2026-08-21)

- `pio run` — SUCCESS in 70.8 s; `firmware.bin` (1,364,448 B) produced.
- `tools/prep_character.py` — full pipeline on a bufo copy: 15 GIFs,
  594,639 bytes, manifest rewritten, output verified then removed.
- Hardware flows (upload, uploadfs, monitor, serial tests) — not exercisable
  in a sandbox; expected, not a failure.

## Codebase map

See [codebase-map.md](codebase-map.md).

## Conventions

- `CONTRIBUTING.md`: reference implementation — maintainers accept
  protocol-doc corrections and reference-breaking bug fixes only. No new
  features, pets, screens, board ports, refactors, or dependency bumps.
- `REFERENCE.md` is the wire-protocol contract; firmware changes must honor it.
- Each ASCII pet is `src/buddies/<species>.cpp` with seven animation functions
  (sleep, idle, busy, attention, celebrate, dizzy, heart).
- Character packs: `manifest.json` + 96px-wide GIFs, whole folder under 1.8 MB
  (LittleFS cap).

## Sandbox snapshot

- Snapshot / template ID: `3v6q0vborbb1ypl1sifq:default`
- Captured: 2026-08-21T17:21:46.931Z
- Contents: PlatformIO Core 6.1.19 + Pillow 12.3.0 installed; espressif32
  toolchain and `lib_deps` cached in `.pio/`; firmware pre-built.
