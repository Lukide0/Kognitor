#ifndef COMPONENT_SENSOR_IR_H
#define COMPONENT_SENSOR_IR_H

#include "mcu/Port.h"

namespace component::sensor {

template <mcu::full_pin_info Data, mcu::full_pin_info Enable> struct IrInfo {
    using data   = Data;
    using enable = Enable;
};

template <typename T>
concept ir_info = requires {
    typename T::data;
    typename T::enable;

    requires mcu::full_pin_info<typename T::data>;
    requires mcu::full_pin_info<typename T::enable>;
};

template <ir_info Info> struct IR {
    static void prepare() {
        Info::data::port::template enable_pullup<typename Info::data::pinbit>();
        Info::data::ddr::template enable_input<typename Info::data::pinbit>();

        Info::enable::port::template set_low<typename Info::enable::pinbit>();
        Info::enable::ddr::template enable_output<typename Info::enable::pinbit>();
    }

    static void enable() { Info::enable::port::template set_high<typename Info::enable::pinbit>(); }

    static void disable() { Info::enable::port::template set_low<typename Info::enable::pinbit>(); }

    static bool read() { return (Info::data::pin::read() & bit(Info::data::pinbit::pos)) == 0; }
};

}

#endif
