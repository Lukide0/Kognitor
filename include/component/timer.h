#ifndef COMPONENT_TIMER_H
#define COMPONENT_TIMER_H

#include "bits.h"
#include "concepts.h"
#include "mcu/io.h"
#include "mcu/Reg.h"
#include "mcu/Timer.h"
#include <stdint.h>

namespace component {

using mcu::timer_info;
using mcu::TimerClockSource;

static consteval uint8_t clock_source_to_bits(TimerClockSource source) {
    using namespace io;
    switch (source) {
    case TimerClockSource::NO_SOURCE:
        return 0;
    case TimerClockSource::CLK:
        return CS00::bit;
    case TimerClockSource::CLK_8:
        return CS01::bit;
    case TimerClockSource::CLK_64:
        return CS00::bit | CS01::bit;
    case TimerClockSource::CLK_256:
        return CS02::bit;
    case TimerClockSource::CLK_1024:
        return CS00::bit | CS02::bit;
    }
}

template <timer_info Info> struct NormalTimer {
    static constexpr uint8_t prescale_bits_mask = io::CS00::bit | io::CS01::bit | io::CS02::bit;
    using size_t                                = Info::size_t;
    static constexpr size_t MAX                 = Info::MAX;

    template <TimerClockSource Source> struct CalcMaxMs {
        static constexpr uint32_t value = 1000 / (F_CPU / (static_cast<uint32_t>(Source) * Info::MAX));
    };

    template <TimerClockSource Source, size_t Ms = CalcMaxMs<Source>::value> static void start() {
        using namespace io;
        constexpr uint8_t prescale_bits = clock_source_to_bits(Source);
        constexpr uint32_t per_second   = 1000 / Ms;
        constexpr uint32_t count_to     = F_CPU / (static_cast<uint32_t>(Source) * per_second);

        static_assert(count_to <= MAX, "Timer is too fast");

        // set timer to 0
        clear();

        Info::OCRnA::write(count_to);

        // 8bit timer
        if constexpr (same_as<size_t, uint8_t>) {
            Info::TCCRnA::template write<WGM01>();
            Info::TCCRnB::write(prescale_bits);
        }
        // 16bit timer
        else if constexpr (same_as<size_t, uint16_t>) {
            Info::TCCRnA::write(0);
            Info::TCCRnB::write(WGM12::bit | prescale_bits);
        } else {
            static_assert(false, "Unsupported timer size");
        }
    }

    static void enable_interrupt() {
        using namespace io;

        Info::TIMSKn::template set_bits<OCIE0A>();
    }

    static void disable_interrupt() {
        using namespace io;
        Info::TIMSKn::template unset_bits<OCIE0A>();
    }

    static void stop() { *Info::TCCRnB::get_ptr() &= ~prescale_bits_mask; }

    static void clear() { Info::TCNTn::write(0); }

    static size_t count() { return Info::TCNTn::read(); }
};

}

#endif
