#ifndef COMPONENT_SENSOR_MIC_H
#define COMPONENT_SENSOR_MIC_H

#include "mcu/ADC.h"
#include "mcu/Pin.h"
#include "mcu/PinBit.h"
#include "mcu/Port.h"

namespace component::sensor {

template <
    mcu::is_pinbit Analog,
    mcu::is_pinbit Digital,
    mcu::is_port DigitalPort,
    mcu::is_ddr DigitalDDR,
    mcu::is_pin DigitalPin>
    requires(Analog::template has<mcu::PinProp::ANALOG>() && Digital::template has<mcu::PinProp::DIGITAL>())
struct MicInfo {
    using analog  = Analog;
    using digital = Digital;
    using port    = DigitalPort;
    using ddr     = DigitalDDR;
    using pin     = DigitalPin;
};

template <typename T>
concept mic_info = requires {
    typename T::analog;
    typename T::digital;

    typename T::port;
    typename T::ddr;

    requires mcu::is_pinbit<typename T::analog>;
    requires mcu::is_pinbit<typename T::digital>;

    requires mcu::is_port<typename T::port>;
    requires mcu::is_ddr<typename T::ddr>;
};

template <mcu::adc ADC, mic_info Info> struct Mic {
    static void prepare_digital() {
        Info::port::template disable_pullup<typename Info::digital>();
        Info::ddr::template enable_input<typename Info::digital>();
    }

    static uint16_t read() {
        ADC::template enable<mcu::ADCReference::AVCC, mcu::ADCPrescaler::CLK_8, false>();
        ADC::template prepare<typename Info::analog>();

        return ADC::read();
    }

    static bool reached_threshold() { return (Info::pin::read() & bit(Info::digital::pos)) != 0; }
};

}

#endif
