# Codebase Map — claude-desktop-buddy

Folder-level overview (depth 2). Single PlatformIO env: `m5stickc-plus`.

| Path                          | Purpose                                                                  |
| ----------------------------- | ------------------------------------------------------------------------ |
| `src/`                        | Firmware source (Arduino C++), compiled by `pio run`                     |
| `src/main.cpp`                | Loop, state machine, UI screens (~44 KB, largest file)                   |
| `src/buddy.cpp` `buddy.h` `buddy_common.h` | ASCII species dispatch + render helpers                     |
| `src/buddies/`                | One file per ASCII species (18 species), seven anim functions each       |
| `src/ble_bridge.cpp` `.h`     | Nordic UART BLE service, line-buffered TX/RX                             |
| `src/character.cpp` `.h`      | GIF character decode + render                                            |
| `src/data.h`                  | Wire protocol: JSON parse of heartbeat / prompt messages                 |
| `src/xfer.h`                  | Folder-push receiver (character packs over BLE/serial)                   |
| `src/stats.h`                 | NVS-backed stats, settings, owner, species choice                        |
| `characters/`                 | Example GIF character packs — `bufo/`: manifest.json + 96px GIFs         |
| `tools/`                      | Python helpers: `prep_character.py`, `flash_character.py`, `test_serial.py`, `test_xfer.py` |
| `docs/`                       | Device photo, screenshots, `manual.html`                                 |
| `platformio.ini`              | Build config: espressif32 / m5stick-c / arduino, LittleFS, no_ota.csv    |
| `REFERENCE.md`                | BLE wire-protocol contract (Nordic UART UUIDs, JSON schemas)             |
| `README.md` `CONTRIBUTING.md` `LICENSE` | Project docs and contribution policy                          |
