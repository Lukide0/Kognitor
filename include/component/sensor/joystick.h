#ifndef COMPONENT_SENSOR_JOYSTICK_H
#define COMPONENT_SENSOR_JOYSTICK_H

#include "mcu/ADC.h"
#include "mcu/PinBit.h"
#include <stdint.h>

namespace component::sensor {

template <mcu::is_pinbit AxisX, mcu::is_pinbit AxisY>
    requires(AxisX::template has<mcu::PinProp::ANALOG>() && AxisY::template has<mcu::PinProp::ANALOG>())
struct JoystickInfo {
    using axis_x = AxisX;
    using axis_y = AxisY;
};

template <typename T>
concept joystick_info = requires {
    typename T::axis_x;
    typename T::axis_y;

    requires mcu::is_pinbit<typename T::axis_x>;
    requires mcu::is_pinbit<typename T::axis_y>;
};

template <mcu::adc ADC, joystick_info Info> struct Joystick {
private:
    template <mcu::is_pinbit PinBit> static uint8_t read() {
        ADC::template prepare<PinBit>();
        return ADC::read_adjusted();
    }

public:
    static void prepare_for_read() { ADC::template enable<mcu::ADCReference::AVCC, mcu::ADCPrescaler::CLK_8, true>(); }

    static uint8_t read_x() { return read<typename Info::axis_x>(); }

    static uint8_t read_y() { return read<typename Info::axis_y>(); }
};

}

#endif
