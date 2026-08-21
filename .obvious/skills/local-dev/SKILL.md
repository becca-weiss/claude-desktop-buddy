---
name: local-dev
description: Build and verify claude-desktop-buddy firmware locally with PlatformIO — no hardware needed for the build
version: 1
---

# Local Dev — claude-desktop-buddy

Verified 2026-08-21 on sandbox snapshot `3v6q0vborbb1ypl1sifq:default`.

## What works without hardware

1. **Build firmware** (primary health check):

   ```bash
   pio run
   ```

   First run downloads the espressif32 platform + toolchain into `.pio/`
   (~1-2 min warm, ~5-10 min cold). Success looks like:

   ```
   RAM:   [==        ]  22.4% (used 73500 bytes from 327680 bytes)
   Flash: [======    ]  64.7% (used 1357877 bytes from 2097152 bytes)
   ========================= [SUCCESS] Took 70.78 seconds =========================
   ```

2. **Character-pack pipeline** (exercises Pillow + all 15 bufo GIFs):

   ```bash
   cp -r characters/bufo /tmp/bufo-src
   # edit /tmp/bufo-src/manifest.json: "name" -> "bufo-test"
   # (prep_character.py always writes to characters/<name>/ and rmtree's it first)
   python3 tools/prep_character.py /tmp/bufo-src
   # expect ~595 KB under characters/bufo-test; clean up afterwards
   rm -rf characters/bufo-test /tmp/bufo-src
   ```

## Toolchain (already in the snapshot)

- PlatformIO Core 6.1.19 (`pip3 install platformio`)
- Python 3.13 + Pillow 12.3.0 (`pip3 install pillow`)
- `.pio/` holds the espressif32 toolchain, downloaded libs, and build artifacts

## What needs a physical M5StickC Plus

`pio run -t upload`, `pio run -t uploadfs`, `pio device monitor`,
`tools/test_serial.py`, `tools/test_xfer.py` (the latter two also need
`pip3 install pyserial`). None can run in a sandbox — treat "no stick found"
as expected, not a failure. Never mark the run unhealthy for these.

## Gotchas

- No lint / typecheck / tests configured; no CI. `pio run` is the only
  automated check.
- `data/` (LittleFS staging for `uploadfs`) is gitignored; created by
  `tools/flash_character.py`.
- `prep_character.py` overwrites `characters/<name>/` — always run it on a
  copy with a renamed manifest.
- Flash budget: `no_ota.csv` gives a 2 MB app partition; build sits at 64.7%.
  Check the size report on any change.
- CONTRIBUTING.md: reference implementation — only protocol-doc fixes and
  reference-breaking bugs are accepted. Don't add features/pets/boards.
