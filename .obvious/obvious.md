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
| Firmware   | M5StickCPlus 0.1.1, AnimatedGIF 2.2.3, ArduinoJson 7.4.3          |
| Tests      | Unity (native env) over `src/stats.h` logic — 20 cases, CI gate   |
| Tooling    | Python 3 scripts in `tools/` (Pillow for GIF prep)                |
| Radio      | BLE Nordic UART Service — wire contract in `REFERENCE.md`         |

No web app, no server, no database, no ports. The "app" is firmware that runs
on the device; the Claude desktop app connects to it over BLE.

## Platform version pin (important)

`platformio.ini` declares `platform = espressif32` **unpinned**. The newest
release, espressif32 8.0.0 (arduino-esp32 3.3.12), **breaks the build**:

- `src/ble_bridge.cpp:44` — `getValue()` now returns `String`, code assigns `std::string`
- `src/ble_bridge.cpp:94` — `BLEDevice::setEncryptionLevel` no longer exists

The firmware targets the arduino-esp32 2.x API. The sandbox therefore pins
**espressif32 6.9.0** (arduino-esp32 2.0.17) at the environment level:

```bash
pio platform uninstall espressif32
pio platform install espressif32@6.9.0
```

With 6.9.0 installed, the unpinned spec resolves to it and the build is green.
If a fresh clone builds broken, run the two commands above first.
TODO(confirm): maintainers may prefer pinning `espressif32@6.9.0` (or migrating
`ble_bridge.cpp` to the 3.x BLE API) in the repo itself — not done here, since
onboarding does not change build behavior for everyone.

## Commands

```bash
pio run -e m5stickc-plus              # build firmware — works without hardware
pio test -e native                    # native Unity test suite (CI gate)
pio run -t upload                     # flash to a connected stick (hardware)
pio run -t erase && pio run -t upload # wipe then reflash (hardware)
pio run -t uploadfs                   # flash LittleFS data/ partition (hardware)
pio device monitor                    # serial monitor @ 115200 (hardware)
python3 tools/prep_character.py <dir> # normalize/resize a character pack to 96px
python3 tools/flash_character.py characters/bufo # stage + flash a pack (hardware)
python3 tools/test_serial.py          # cycle device states over serial (hardware)
python3 tools/test_xfer.py [src] [nm] # stream pack over serial, check acks (hardware)
```

Build the firmware env explicitly (`-e m5stickc-plus`). A bare `pio run`
processes **both** envs and fails in `native`, which compiles `src/` without
the Arduino core (only `pio test -e native` is valid there — that is what CI
runs). Commands marked **(hardware)** need a physical M5StickC Plus on USB.

## Environment

No env vars required. No `.env` file, no services, no ports. First `pio run`
downloads the espressif32 platform, toolchain, and `lib_deps` into `.pio/`
(gitignored; cached in the snapshot).

## Local verification

Build + tests are the health check:

```bash
pio run -e m5stickc-plus   # expect [SUCCESS] + RAM/Flash size report
pio test -e native         # expect: 20 test cases succeeded
```

There is no lint or typecheck. CI (`.github/workflows/ci.yml`) runs
`pio test -e native` on every push/PR.

### Local Verification Summary (onboarding run 2026-11-05)

- `pio run -e m5stickc-plus` — SUCCESS in 89.5 s on espressif32 6.9.0.
  RAM 22.4% (73,500 / 327,680 B), Flash 64.7% (1,357,877 / 2,097,152 B);
  `firmware.bin` (1,364,448 B) in `.pio/build/m5stickc-plus/`.
- `pio test -e native` — 20/20 test cases PASSED (7.4 s cold, 0.4 s warm).
  Matches the CI gate exactly.
- `tools/prep_character.py` — full pipeline on a copy of `characters/bufo`:
  15 GIFs, 594,639 bytes, manifest rewritten (pack under the 1.8 MB cap);
  output verified and removed, working tree clean.
- Hardware flows (upload, uploadfs, monitor, serial tests) — not exercisable
  in a sandbox; expected, not a failure.
- First build attempt on the default-unpinned platform (espressif32 8.0.0)
  FAILED in `ble_bridge.cpp` — see "Platform version pin" above; 6.9.0 fixes it.

## Codebase map

See [codebase-map.md](codebase-map.md).

## Conventions

- `CONTRIBUTING.md`: reference implementation — maintainers accept
  protocol-doc corrections and reference-breaking bug fixes only. No new
  features, pets, screens, board ports, refactors, or dependency bumps.
- `REFERENCE.md` is the wire-protocol contract; firmware changes must honor it.
- Each ASCII pet is `src/buddies/<species>.cpp` with seven animation functions
  (sleep, idle, busy, attention, celebrate, dizzy, heart).
- Native tests cover the pure logic in `src/stats.h` via stubs in
  `test/stubs/`; keep device code out of natively-tested headers or extend
  the stubs instead.
- Character packs: `manifest.json` + 96px-wide GIFs, whole folder under 1.8 MB
  (LittleFS cap).

## Sandbox snapshot

- Snapshot / template ID: `hiy2ht7qqshv0h3x81sm:default`
- Captured: 2026-11-05T01:16:43.468Z
- Contents: PlatformIO Core 6.2.0, Python 3.13 + Pillow 12.3.0,
  espressif32 6.9.0 (arduino-esp32 2.0.17) + toolchains, `lib_deps` and Unity
  cached, firmware pre-built in `.pio/`, native tests pre-built.
