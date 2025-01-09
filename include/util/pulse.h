#ifndef UTIL_PULSE_H
#define UTIL_PULSE_H

#include "mcu/Pin.h"
#include "mcu/PinBit.h"

#include <stdint.h>

#ifndef F_CPU
#    error "ERROR: F_CPU is not defined"
#endif

namespace util {

constexpr uint8_t COUNT_UNTIL_TIMEOUT = 255;

/**
 * @brief Counts until the pin is in the expected state
 *
 * @tparam Pin
 * @tparam PinBit
 * @tparam ExpectLow
 *
 * @return uint8_t Count of 8 cycles to reach the expected state. If the expected state is not reached, returns 255
 * (2040 cycles). For example if the function returns 5, the pin is in the expected state after 40 cycles.
 *
 */
template <mcu::is_pin Pin, mcu::is_pinbit PinBit, bool ExpectLow>
[[gnu::always_inline]] [[nodiscard]] inline uint8_t count_until()
    requires(Pin::reg >= 0x20 && Pin::reg < 0x60)
{
    constexpr uint8_t addr = Pin::reg - 0x20;

    uint8_t result = 0;
    if constexpr (!ExpectLow) {
        asm(R"(
    LOOP%=:
        sbis %[pin], %[pin_bit]         ; skip if bit set
        rjmp EXIT%=

        cpi %[count], 255                    ; check counter overflow
        breq EXIT%=                     ;
        inc  %[count]                       ; increment counter
        rjmp LOOP%=
    EXIT%=:
    )"
            : [count] "+r"(result)
            : [pin] "I"(addr), [pin_bit] "M"(PinBit::pos));
    } else {
        asm(R"(
    LOOP%=:
        sbic %[pin], %[pin_bit]         ; skip if bit clear
        rjmp EXIT%=

        cpi %[count], 255                    ; check counter overflow
        breq EXIT%=                     ;
        inc  %[count]                       ; increment counter
        rjmp LOOP%=
    EXIT%=:
    )"
            : [count] "+r"(result)
            : [pin] "I"(addr), [pin_bit] "M"(PinBit::pos));
    }
    return result;
}

template <mcu::is_pin Pin, mcu::is_pinbit PinBit, bool ExpectLow>
[[gnu::always_inline]] [[nodiscard]] inline uint8_t count_until_us()
    requires(Pin::reg >= 0x20 && Pin::reg <= 0x60 && F_CPU % 8'000'000 == 0)
{
    constexpr uint16_t cycles8_per_us = F_CPU / 8'000'000;
    const uint8_t cycles8             = count_until<Pin, PinBit, ExpectLow>();

    return cycles8 / cycles8_per_us;
}

}

#endif
