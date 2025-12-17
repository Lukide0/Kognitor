#ifndef COMPONENT_SENSOR_BASICSENSOR_H
#define COMPONENT_SENSOR_BASICSENSOR_H

#include <microstd/concepts.h>

#include "types/sensor_pin.h"

namespace component::sensor {

template <types::sensor_pin Pin, bool EnablePullup> struct BasicSensorInfo {
    using pin_info               = Pin;
    static constexpr bool pullup = EnablePullup;
};

template <typename T>
concept basic_sensor_info = requires {
    typename T::pin_info;
    requires types::sensor_pin<typename T::pin_info>;

    T::pullup;
    { T::pullup } -> microstd::similar_as<bool>;
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
            Info::pin_info::port_bit::set();
        } else {
            Info::pin_info::port_bit::unset();
        }

        Info::pin_info::ddr_bit::unset();
    }

    /**
     * @brief Reads the sensor.
     */
    static bool read() { return (Info::pin_info::pin::read() & Info::pin_info::pin_bit::bit) != 0; }
};

}

#endif
