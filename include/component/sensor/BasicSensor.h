#ifndef COMPONENT_SENSOR_BASICSENSOR_H
#define COMPONENT_SENSOR_BASICSENSOR_H

#include "concepts.h"
#include "mcu/Pin.h"
#include "mcu/PinBit.h"
#include "mcu/Port.h"

namespace component::sensor {

template <mcu::full_pin_info Info, bool EnablePullup> struct BasicSensorInfo {
    using pin    = typename Info::Pin;
    using pinbit = typename Info::PinBit;
    using port   = typename Info::Port;
    using ddr    = typename Info::DDR;

    static constexpr bool pullup = EnablePullup;
};

template <typename T>
concept basic_sensor_info = requires {
    typename T::pin;
    typename T::pinbit;
    typename T::port;
    typename T::ddr;

    T::pullup;

    { T::pullup } -> similar<bool>;

    requires mcu::is_pinbit<typename T::pinbit>;
    requires mcu::is_pin<typename T::pin>;
    requires mcu::is_port<typename T::port>;
    requires mcu::is_ddr<typename T::ddr>;
};

template <typename T, bool Pullup>
concept basic_sensor_info_pullup = requires {
    requires basic_sensor_info<T>;
    requires T::pullup == Pullup;
};

/**
 * @brief The basic sensor base.
 */
template <basic_sensor_info Info> struct BasicSensor {
    /**
     * @brief Enables the sensor.
     */
    static void enable() {
        if constexpr (Info::pullup) {
            Info::port::template enable_pullup<typename Info::pinbit>();
        } else {
            Info::port::template disable_pullup<typename Info::pinbit>();
        }

        Info::ddr::template enable_input<typename Info::pinbit>();
    }

    /**
     * @brief Reads the sensor.
     */
    static bool read() { return (Info::pin::read() & bit(Info::pinbit::pos)) != 0; }
};

}

#endif
