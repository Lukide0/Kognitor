#ifndef BITS_H
#define BITS_H

#include <stdint.h>

constexpr uint8_t bit(uint8_t bit) { return 1 << bit; }

constexpr uint8_t bits_lower(uint16_t value) { return value & 0xFF; }

constexpr uint8_t bits_higher(uint16_t value) { return (value >> 8) & 0xFF; }

#endif
