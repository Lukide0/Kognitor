#ifndef COMPONENT_SENSOR_TILT_H
#define COMPONENT_SENSOR_TILT_H

#include "mcu/Port.h"

namespace component::sensor {

template <mcu::full_pin_info Data> struct Tilt {
    static void prepare() {
        Data::port::template disable_pullup<typename Data::pinbit>();
        Data::ddr::template enable_input<typename Data::pinbit>();
    }

    static bool read() { return (Data::pin::read() & bit(Data::pinbit::pos)) != 0; }
};

}

#endif
