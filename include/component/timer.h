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

enum class TimeUnitType {
    MS,
    US,
};

template <TimeUnitType Unit, uint32_t Value> struct TimeUnit {
    static constexpr TimeUnitType unit = Unit;
    static constexpr uint32_t value    = Value;

    static consteval uint64_t per_second() {
        switch (unit) {
        case TimeUnitType::MS:
            return 1'000 / value;
        case TimeUnitType::US:
            return 1'000'000 / value;
        }
    }
};

template <uint32_t Ms> using MsUnit = TimeUnit<TimeUnitType::MS, Ms>;

template <uint32_t Us> using UsUnit = TimeUnit<TimeUnitType::US, Us>;

template <typename T>
concept time_unit = requires {
    T::unit;
    T::value;

    { T::unit } -> similar<TimeUnitType>;
    { T::value } -> similar<uint32_t>;
    { T::per_second() } -> similar<uint64_t>;
};

template <timer_info Info> struct NormalTimer {
    static constexpr uint8_t prescale_bits_mask = io::CS00::bit | io::CS01::bit | io::CS02::bit;
    using size_t                                = Info::size_t;
    static constexpr size_t MAX                 = Info::MAX;

    template <TimerClockSource Source> struct CalcMaxMs {
        static constexpr uint32_t value = 1000 / (F_CPU / (static_cast<uint32_t>(Source) * Info::MAX));
    };

    template <TimerClockSource Source, time_unit Unit = MsUnit<CalcMaxMs<Source>::value>> static void start() {
        using namespace io;
        constexpr uint8_t prescale_bits = clock_source_to_bits(Source);
        constexpr uint64_t per_second   = Unit::per_second();
        constexpr uint64_t divider      = static_cast<uint64_t>(Source) * per_second;
        static_assert(divider < F_CPU);

        constexpr uint32_t count_to = F_CPU / divider;

        static_assert(count_to <= MAX, "Timer is too fast");

        // set timer to 0
        clear();

        Info::OCRnA::write(count_to);

        // clear interrupt flags
        Info::TIFRn::write(OCF0A::bit);

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

    static bool match() { return (Info::TIFRn::read() & io::OCF0A::bit) != 0; }
};

}

#endif
