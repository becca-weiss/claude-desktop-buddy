# Codebase Map — claude-desktop-buddy

Folder-level overview (depth 2). Two PlatformIO envs: `m5stickc-plus` (firmware)
and `native` (Unity tests, CI gate).

| Path                          | Purpose                                                                  |
| ----------------------------- | ------------------------------------------------------------------------ |
| `src/`                        | Firmware source (Arduino C++), built by `pio run -e m5stickc-plus`       |
| `src/main.cpp`                | Loop, state machine, UI screens (~44 KB, largest file)                   |
| `src/buddy.cpp` `buddy.h` `buddy_common.h` | ASCII species dispatch + render helpers                     |
| `src/buddies/`                | One file per ASCII species (18 species), seven anim functions each       |
| `src/ble_bridge.cpp` `.h`     | Nordic UART BLE service, line-buffered TX/RX (arduino-esp32 2.x API)     |
| `src/character.cpp` `.h`      | GIF character decode + render                                            |
| `src/data.h`                  | Wire protocol: JSON parse of heartbeat / prompt messages                 |
| `src/xfer.h`                  | Folder-push receiver (character packs over BLE/serial)                   |
| `src/stats.h`                 | NVS-backed stats, settings, owner, species choice — pure logic, unit-tested |
| `test/stubs/`                 | `Arduino.h` + `Preferences.h` shims so stats logic compiles natively     |
| `test/test_smoke/`            | 20 Unity test cases over `src/stats.h` — what CI runs                    |
| `characters/`                 | Example GIF character packs — `bufo/`: manifest.json + 96px GIFs         |
| `tools/`                      | Python helpers: `prep_character.py`, `flash_character.py`, `test_serial.py`, `test_xfer.py` |
| `docs/`                       | Device photo, screenshots, `manual.html`                                 |
| `.github/workflows/ci.yml`    | CI: installs PlatformIO, runs `pio test -e native` on push/PR            |
| `platformio.ini`              | Build config: espressif32 / m5stick-c / arduino, LittleFS, no_ota.csv; native env with `-I test/stubs` |
| `REFERENCE.md`                | BLE wire-protocol contract (Nordic UART UUIDs, JSON schemas)             |
| `README.md` `CONTRIBUTING.md` `LICENSE` | Project docs and contribution policy                          |
