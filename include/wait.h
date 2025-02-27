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

#endif
