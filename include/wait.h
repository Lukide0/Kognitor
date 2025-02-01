#ifndef WAIT_H
#define WAIT_H

#include "bits.h"
#include "component/timer.h"
#include "mcu/Pin.h"
#include "mcu/PinBit.h"
#include "mcu/Timer.h"
#include <stdint.h>

template <mcu::is_pin Pin, mcu::is_pinbit PinBit> inline void wait_until_on() {
    while ((Pin::read() & bit(PinBit::pos)) == 0) { }
}

template <mcu::is_pin Pin, mcu::is_pinbit PinBit> inline void wait_until_off() {
    while ((Pin::read() & bit(PinBit::pos)) != 0) { }
}

template <
    bool WaitUntilOn,
    mcu::is_pin Pin,
    mcu::is_pinbit PinBit,
    component::timer_info TimerInfo,
    component::TimerClockSource Source,
    component::time_unit Timeout>
bool wait_timeout(uint8_t& count) {
    using namespace component;
    using namespace io;

    using timer_t = NormalTimer<TimerInfo>;

    timer_t::template start<Source, Timeout>();

    do {
        const auto value = Pin::read() & bit(PinBit::pos);
        if constexpr (WaitUntilOn) {
            if (value != 0) {
                count = timer_t::count();
                return true;
            }
        } else {
            if (value == 0) {
                count = timer_t::count();
                return true;
            }
        }

    } while (!timer_t::match());

    return false;
}

template <mcu::is_pin Pin, mcu::is_pinbit PinBit>
    requires(Pin::reg >= 0x20 && Pin::reg < 0x60 && F_CPU % 1'000'000 == 0 && F_CPU / 1'000'000 < 255)
struct count_us {
    static constexpr uint8_t inst_per_us = (F_CPU / 1'000'000);
    static constexpr uint8_t io_reg      = Pin::reg - 0x20;
    static constexpr uint8_t io_reg_pos  = PinBit::pos;

    static uint8_t until_off() {

        uint8_t count = 0;
        uint8_t tmp   = 0;

        // NOTE: Single loop is 10 cycles (not counting the overflow and final loop)
        constexpr uint8_t inc_interval = (inst_per_us * 10) - 10; // increment every ~10us
        static_assert(inc_interval != 0 && inc_interval <= 255);

        asm(R"(
        WAIT%=:
            cpi %[tmp], %[interval]     ; 1 cycle

            brne INCREMENT%=            ; 1 cycle (2 if the condition is true)

            ldi %[tmp], 0               ; 1 cycle
            inc %[count]                ; 1 cycle

            brne EXIT%=                 ; 1 cycle (branch if 'count' overflows)

            rjmp CONTINUE%=             ; 2 cycles
            ; 1(cpi) + 1(brne) + 5 = 7 cycles
        INCREMENT%=:
            inc %[tmp]                  ; 1 cycle
            nop
            nop
            nop
            ; 1(cpi) + 2(brne) + 1(inc) + 3(nop) = 7 cycles
        CONTINUE%=:
            sbic %[io], %[io_bit] ; skip next instruction if bit is cleared (1 cycle)
            rjmp WAIT%=           ; 2 cycles
        EXIT%=:
        )"
            : [count] "+r"(count), [tmp] "=r"(tmp)
            : [io] "I"(io_reg), [io_bit] "I"(io_reg_pos), [interval] "n"(inc_interval));

        return count;
    }

    static uint8_t until_on() {

        uint8_t count = 0;
        uint8_t tmp   = 0;

        // NOTE: Single loop is 10 cycles (not counting the overflow and final loop)
        constexpr uint8_t inc_interval = (inst_per_us * 10) - 10; // increment every ~10us
        static_assert(inc_interval != 0);

        asm(R"(
        WAIT%=:
            cpi %[tmp], %[interval]     ; 1 cycle

            brne INCREMENT%=            ; 1 cycle (2 if the condition is true)

            xor %[tmp], %[tmp]          ; 1 cycle
            inc %[count]                ; 1 cycle

            brne EXIT%=                 ; 1 cycle (branch if 'count' overflows)

            rjmp CONTINUE%=             ; 2 cycles
            ; 1(cpi) + 1(brne) + 5 = 7 cycles
        INCREMENT%=:
            inc %[tmp]                  ; 1 cycle
            nop
            nop
            nop
            ; 1(cpi) + 2(brne) + 1(inc) + 3(nop) = 7 cycles
        CONTINUE%=:
            sbis %[io], %[io_bit] ; skip next instruction if bit is set (1 cycle)
            rjmp WAIT%=           ; 2 cycles
        EXIT%=:
        )"
            : [count] "+r"(count), [tmp] "=r"(tmp)
            : [io] "I"(io_reg), [io_bit] "I"(io_reg_pos), [interval] "M"(inc_interval));

        return count;
    }
};

#endif
