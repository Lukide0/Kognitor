#ifndef MCU_ADC_H
#define MCU_ADC_H

#include "mcu/PinBit.h"
#include "mcu/Reg.h"
#include <stdint.h>

namespace mcu {

enum class ADCPrescaler {
    CLK_2   = 2,
    CLK_4   = 4,
    CLK_8   = 8,
    CLK_16  = 16,
    CLK_32  = 32,
    CLK_64  = 64,
    CLK_128 = 128,
};

enum class ADCReference {
    AREF,
    AVCC,
    INTERNAL_WITH_CAPACITOR,
};

template <
    is_read_write_reg ADCSRA,
    is_read_write_reg ADCSRB,
    is_read_write_reg ADMUX,
    is_read_reg ADCL,
    is_read_reg ADCH,
    is_read_write_reg PRR>
class ADC {
private:
    using REFS1 = RB7;
    using REFS0 = RB6;
    using ADLAR = RB5;
    using MUX3  = RB3;
    using MUX2  = RB2;
    using MUX1  = RB1;
    using MUX0  = RB0;

    static constexpr uint8_t MUX_MASK = MUX0::bit | MUX1::bit | MUX2::bit | MUX3::bit;

    using ADEN  = RB7;
    using ADSC  = RB6;
    using ADIE  = RB3;
    using ADPS2 = RB2;
    using ADPS1 = RB1;
    using ADPS0 = RB0;

    using PRADC = RB0;

    static consteval uint8_t get_prescaler(ADCPrescaler prescaler) {
        switch (prescaler) {
        case ADCPrescaler::CLK_2:
            return 0;
        case ADCPrescaler::CLK_4:
            return ADPS1::bit;
        case ADCPrescaler::CLK_8:
            return ADPS0::bit | ADPS1::bit;
        case ADCPrescaler::CLK_16:
            return ADPS2::bit;
        case ADCPrescaler::CLK_32:
            return ADPS0::bit | ADPS2::bit;
        case ADCPrescaler::CLK_64:
            return ADPS1::bit | ADPS2::bit;
        case ADCPrescaler::CLK_128:
            return ADPS0::bit | ADPS1::bit | ADPS2::bit;
        }
    }

    static consteval uint8_t get_reference(ADCReference ref) {
        switch (ref) {
        case ADCReference::AREF:
            return 0;
        case ADCReference::AVCC:
            return REFS0::bit;
        case ADCReference::INTERNAL_WITH_CAPACITOR:
            return REFS0::bit | REFS1::bit;
        }
    }

    template <is_pinbit PinBit> static consteval uint8_t get_mux() {

        static_assert(PinBit::pos <= 8);
        switch (PinBit::pos) {
        case 0:
            return 0;
        case 1:
            return MUX0::bit;
        case 2:
            return MUX1::bit;
        case 3:
            return MUX1::bit | MUX0::bit;
        case 4:
            return MUX2::bit;
        case 5:
            return MUX2::bit | MUX0::bit;
        case 6:
            return MUX2::bit | MUX1::bit;
        case 7:
            return MUX2::bit | MUX1::bit | MUX0::bit;
        case 8:
            return MUX3::bit;
        }
    }

public:
    template <is_pinbit PinBit>
    static void prepare()
        requires(PinBit::template has<PinProp::ANALOG>())
    {
        constexpr uint8_t mux_value = get_mux<PinBit>();
        ADMUX::write((ADMUX::read() & ~MUX_MASK) | mux_value);
    }

    static uint16_t read() {
        ADCSRA::template set_bits<ADSC>();

        while ((ADCSRA::read() & ADSC::bit) != 0) { }

        uint16_t value = ADCL::read();
        value |= (ADCH::read() << 8);

        return 1023 - value;
    }

    static void read_int() { ADCSRA::template set_bits<ADSC>(); }

    template <
        ADCReference Ref       = ADCReference::AREF,
        ADCPrescaler Prescaler = ADCPrescaler::CLK_8,
        bool LeftAdjustResult  = false>
    static void enable() {
        constexpr uint8_t prescaler_value = get_prescaler(Prescaler);
        constexpr uint8_t reference_value = get_reference(Ref);

        PRR::template unset_bits<PRADC>();

        if constexpr (LeftAdjustResult) {
            ADMUX::write(reference_value | ADLAR::bit);
        } else {
            ADMUX::write(reference_value);
        }

        ADCSRA::write(prescaler_value | ADEN::bit);
    }

    static void disable() { ADCSRA::template unset_bits<ADEN>(); }

    static void enable_interrupt() { ADCSRA::template set_bits<ADIE>(); }

    static void disable_interrupt() { ADCSRA::template unset_bits<ADIE>(); }
};

}

#endif
