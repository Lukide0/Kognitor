#include "com/usart.h"
#include "mcu/ADC.h"
#include "mcu/io.h"
#include <stdint.h>
#include <util/delay.h>

using namespace com;
using Analog2Digit = mcu::ADC<io::ADCSRA, io::ADCSRB, io::ADMUX, io::ADCL, io::ADCH, io::PRR0>;

// void set_led_on() { io::PORTB::set_high<io::PINB5>(); }
//
// void set_led_off() { io::PORTB::set_low<io::PINB5>(); }
//
// void switch_led() { *io::PORTB::get_ptr() ^= bit(io::PINB5::pos); }

int main() {

    using namespace io;

    DDRB::enable_output<PINB5>();
    usart::init(9600);
    Analog2Digit::enable<mcu::ADCReference::AVCC, mcu::ADCPrescaler::CLK_8>();
    Analog2Digit::prepare<PINC0>();

    while (true) {

        uint16_t value = Analog2Digit::read();

        usart::send((value >> 8) & 0xFF);
        usart::send(value & 0xFF);

        _delay_ms(50);
    }
}
