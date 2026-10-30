#pragma once
#include <stdint.h>
#include <stddef.h>
#include <string.h>

// Minimal Arduino.h shim for native (no-hardware) tests. Only the symbols
// used by the headers under test (src/stats.h) are provided; the real
// Arduino core and ESP32 libraries are never pulled in.

inline unsigned long millis() { return 0; }

// Arduino's min() is a macro; a template keeps it type-safe natively.
template <typename T, typename U>
inline auto min(const T& a, const U& b) -> decltype(a < b ? a : b) {
  return a < b ? a : b;
}
