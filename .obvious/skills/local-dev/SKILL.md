---
name: local-dev
description: Build and verify claude-desktop-buddy firmware locally with PlatformIO — no hardware needed for build and tests
version: 2
---

# Local Dev — claude-desktop-buddy

Verified 2026-11-05 on sandbox snapshot `hiy2ht7qqshv0h3x81sm:default`.

## Platform version pin (do this first on a fresh environment)

`platformio.ini` declares `platform = espressif32` unpinned. As of 2026-11-05
that resolves to espressif32 **8.0.0** (arduino-esp32 3.3.12), which **breaks
the firmware build** — `src/ble_bridge.cpp` uses the arduino-esp32 2.x BLE API:

- line 44: `std::string v = c->getValue();` — `getValue()` now returns `String`
- line 94: `BLEDevice::setEncryptionLevel(...)` — removed in 3.x

Pin 6.9.0 (arduino-esp32 2.0.17) at the environment level:

```bash
pio platform uninstall espressif32
pio platform install espressif32@6.9.0
```

With 6.9.0 installed the unpinned spec resolves to it. Symptom if skipped:
`pio run` fails in `ble_bridge.cpp` with a `String`→`std::string` conversion
error after ~5-6 min of toolchain downloads. TODO(confirm): maintainers may
want the pin (or a 3.x migration) committed in `platformio.ini` itself.

## What works without hardware

1. **Build firmware** (primary health check):

   ```bash
   pio run -e m5stickc-plus
   ```

   Success looks like:

   ```
   RAM:   [==        ]  22.4% (used 73500 bytes from 327680 bytes)
   Flash: [======    ]  64.7% (used 1357877 bytes from 2097152 bytes)
   ========================= [SUCCESS] Took 89.48 seconds =========================
   ```

   Artifacts: `.pio/build/m5stickc-plus/firmware.bin` (~1.36 MB).

   Do NOT use a bare `pio run` as the health check — it processes the `native`
   env too, which cannot compile `src/buddies/*` (they need `M5StickCPlus.h`).
   CI runs only `pio test -e native`.

2. **Native unit tests** (CI gate, fast):

   ```bash
   pio test -e native
   ```

   20 Unity test cases over the pure logic in `src/stats.h`, with Arduino/NVS
   shims from `test/stubs/`. ~7 s cold, <1 s warm. All 20 must pass.

3. **Character-pack pipeline** (exercises Pillow + all 15 bufo GIFs):

   ```bash
   cp -r characters/bufo /tmp/bufo-src
   # edit /tmp/bufo-src/manifest.json: "name" -> "bufo-test"
   # (prep_character.py always writes to characters/<name>/ and rmtree's it first)
   python3 tools/prep_character.py /tmp/bufo-src
   # expect ~595 KB under characters/bufo-test; clean up afterwards
   rm -rf characters/bufo-test /tmp/bufo-src
   ```

## Toolchain (already in the snapshot)

- PlatformIO Core 6.2.0 (`pip3 install platformio`)
- Python 3.13 + Pillow 12.3.0 (`pip3 install pillow pyserial` for serial tools)
- espressif32 6.9.0 + toolchain-xtensa-esp32 8.4.0+2021r2-patch5,
  framework-arduinoespressif32 3.20017 (arduino-esp32 2.0.17)
- `lib_deps` resolved: M5StickCPlus 0.1.1, AnimatedGIF 2.2.3, ArduinoJson 7.4.3;
  Unity 2.6.1 for the native env
- `.pio/` holds the toolchain, downloaded libs, and build artifacts

## What needs a physical M5StickC Plus

`pio run -t upload`, `pio run -t uploadfs`, `pio device monitor`,
`tools/test_serial.py`, `tools/test_xfer.py` (the latter two also need
`pip3 install pyserial`). None can run in a sandbox — treat "no stick found"
as expected, not a failure. Never mark the run unhealthy for these.

## Gotchas

- Unpinned `platform = espressif32` will silently drift again when PlatformIO
  releases new versions — if the firmware build breaks in BLE code, suspect a
  platform upgrade first (see pin above).
- No lint or typecheck in this repo; automated checks = native test suite (CI)
  + the firmware build.
- `data/` (LittleFS staging for `uploadfs`) is gitignored; created by
  `tools/flash_character.py`.
- `prep_character.py` overwrites `characters/<name>/` — always run it on a
  copy with a renamed manifest.
- Flash budget: `no_ota.csv` gives a 2 MB app partition; build sits at 64.7%.
  Check the size report on any change.
- CONTRIBUTING.md: reference implementation — only protocol-doc fixes and
  reference-breaking bugs are accepted. Don't add features/pets/boards.
