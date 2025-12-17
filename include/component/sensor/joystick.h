#ifndef COMPONENT_SENSOR_JOYSTICK_H
#define COMPONENT_SENSOR_JOYSTICK_H

#include <microstd/mcu/adc.h>
#include <microstd/mcu/reg.h>
#include <stdint.h>

namespace component::sensor {

template <microstd::mcu::avr::Input AxisX, microstd::mcu::avr::Input AxisY> struct JoystickInfo {
    static constexpr auto axis_x = AxisX;
    static constexpr auto axis_y = AxisY;
};

template <typename T>
concept joystick_info = requires {
    T::axis_x;
    T::axis_y;

    { T::axis_x } -> microstd::similar_as<microstd::mcu::avr::Input>;
    { T::axis_y } -> microstd::similar_as<microstd::mcu::avr::Input>;
};

template <microstd::mcu::adc::is_adc ADC, joystick_info Info> struct Joystick {
private:
    template <microstd::mcu::avr::Input In> static uint16_t read() {
        ADC::template init<In, microstd::mcu::avr::RefVoltage::AVCC>();
        ADC::enable_and_start();

        while (!ADC::completed()) { }

        return ADC::read();
    }

public:
    static uint16_t read_x() { return read<Info::axis_x>(); }

    static uint16_t read_y() { return read<Info::axis_y>(); }
};

}

#endif
