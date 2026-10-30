#pragma once
#include <stdint.h>
#include <stddef.h>
#include <string.h>

// Minimal Arduino.h shim for native (no-hardware) tests. Only the symbols
// used by the headers under test (src/stats.h) are provided; the real
// Arduino core and ESP32 libraries are never pulled in.

// Fake clock: stats.h's energy logic divides by millis(), so tests need to
// advance time. Default stays 0 to match a fresh boot.
inline unsigned long& _fakeMillis() {
  static unsigned long t = 0;
  return t;
}
inline unsigned long millis() { return _fakeMillis(); }
inline void setFakeMillis(unsigned long ms) { _fakeMillis() = ms; }

// Arduino's min() is a macro; a template keeps it type-safe natively.
template <typename T, typename U>
inline auto min(const T& a, const U& b) -> decltype(a < b ? a : b) {
  return a < b ? a : b;
}
