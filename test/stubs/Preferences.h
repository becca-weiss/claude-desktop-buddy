#pragma once
#include <stdint.h>
#include <stddef.h>

// Native-test stub for the ESP32 NVS Preferences class. No persistence —
// just enough surface for the stats logic to call begin/end/get*/put*
// without a device. The stats tests drive state through the API, not
// through storage, so reads return their defaults.
class Preferences {
public:
  bool begin(const char*, bool) { return true; }
  void end() {}

  uint32_t getUInt(const char*, uint32_t defaultValue) const { return defaultValue; }
  uint16_t getUShort(const char*, uint16_t defaultValue) const { return defaultValue; }
  uint8_t getUChar(const char*, uint8_t defaultValue) const { return defaultValue; }
  bool getBool(const char*, bool defaultValue) const { return defaultValue; }
  size_t getBytes(const char*, void*, size_t) { return 0; }
  size_t getString(const char* key, char* value, size_t maxLen, const char* defaultValue = "") {
    (void)key;
    strncpy(value, defaultValue, maxLen - 1);
    value[maxLen - 1] = 0;
    return strlen(value);
  }

  size_t putUInt(const char*, uint32_t) { return sizeof(uint32_t); }
  size_t putUShort(const char*, uint16_t) { return sizeof(uint16_t); }
  size_t putUChar(const char*, uint8_t) { return sizeof(uint8_t); }
  size_t putBool(const char*, bool) { return sizeof(bool); }
  size_t putBytes(const char*, const void*, size_t n) { return n; }
  size_t putString(const char*, const char* value) { return strlen(value); }
};
