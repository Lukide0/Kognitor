#ifndef TYPES_SENSOR_PIN_H
#define TYPES_SENSOR_PIN_H

#include <microstd/mcu/reg.h>

namespace types {

template <
    microstd::mcu::is_any_register_bit PortBit,
    microstd::mcu::is_any_register_bit DDRBit,
    microstd::mcu::is_any_register_bit PinBit>
struct SensorPin {
    using port = typename PortBit::register_t;
    using ddr  = typename DDRBit::register_t;
    using pin  = typename PinBit::register_t;

    using port_bit = PortBit;
    using ddr_bit  = DDRBit;
    using pin_bit  = PinBit;
};

template <typename T>
concept sensor_pin = requires {
    typename T::port;
    typename T::ddr;
    typename T::pin;

    typename T::port_bit;
    typename T::ddr_bit;
    typename T::pin_bit;

    requires microstd::mcu::is_normal_register<typename T::port>;
    requires microstd::mcu::is_normal_register<typename T::ddr>;
    requires microstd::mcu::is_readable_register<typename T::pin>;
};

}

#endif
