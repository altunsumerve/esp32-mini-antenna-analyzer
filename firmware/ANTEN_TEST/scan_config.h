// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once
#include <stdint.h>
#include <stddef.h>

constexpr uint32_t MIN_HZ = 6000000, MAX_HZ = 8000000;
inline bool parseHz(const char *s, uint32_t &value) {
  if (!s || !*s) return false;
  uint32_t n = 0;
  for (; *s; ++s) {
    if (*s < '0' || *s > '9') return false;
    const uint32_t digit = *s - '0';
    if (n > (UINT32_MAX - digit) / 10) return false;
    n = n * 10 + digit;
  }
  value = n;
  return true;
}
inline bool validScan(uint32_t first, uint32_t last, uint32_t step) {
  return first >= MIN_HZ && last <= MAX_HZ && first < last &&
         step >= 100 && step <= last - first &&
         (last - first) % step == 0 && (last - first) / step + 1 <= 401;
}
struct DetectorReading { float mean; uint16_t low, high; };
