#include "com/twi.h"
#include "com/usart.h"
#include "component/lcd_i2c.h"
#include "mcu/ADC.h"
#include "mcu/io.h"
#include "util/pulse.h"
#include <stdint.h>
#include <util/delay.h>

using namespace com;
using namespace io;

using Analog2Digit = mcu::ADC<ADCSRA, ADCSRB, ADMUX, ADCL, ADCH, PRR0>;
using Config       = TWIConfig<PORTC, DDRC, PINC4, PINC5, TWSR0, TWBR0, TWCR0, TWDR0>;

// constexpr uint8_t LCD_ADDR = 0x27;
// using LCD                  = component::Lcd<Config, LCD_ADDR, true>;

void measure() {
    DDRB::enable_output<PINB4>();
    PORTB::set_low<PINB4>();
    _delay_ms(20);
    PORTB::set_high<PINB4>();
    _delay_us(40);
    DDRB::enable_input<PINB4>();
    // _delay_us(50);
    //
    while ((PINB::read() & bit(PINB4::pos)) == 0) { }
    //
    while ((PINB::read() & bit(PINB4::pos)) != 0) { }

    // if (util::count_until<PINB, PINB5, true>() == util::COUNT_UNTIL_TIMEOUT) {
    //     return;
    // }
    //
    // if (util::count_until<PINB, PINB5, false>() == util::COUNT_UNTIL_TIMEOUT) {
    //     return;
    // }

    uint8_t bytes[5] = { 0, 0, 0, 0, 0 };
    for (uint8_t i = 0; i < 5; ++i) {
        for (uint8_t b = 0; b < 8; ++b) {

            while ((PINB::read() & bit(PINB4::pos)) == 0) { }

            uint8_t counter = util::count_until<PINB, PINB4, false>();
            if (counter > 50) {
                bytes[i] |= (1 << (7 - b));
            }
        }
    }

    usart::send("START:\r\n");
    usart::send_hex(bytes[0]);
    usart::send("\r\n");
    usart::send_hex(bytes[1]);
    usart::send("\r\n");
    usart::send_hex(bytes[2]);
    usart::send("\r\n");
    usart::send_hex(bytes[3]);
    usart::send("\r\n");
    usart::send_hex(bytes[4]);
    usart::send("\r\n");
    usart::send("END\r\n");
    if (bytes[0] + bytes[1] + bytes[2] + bytes[3] != bytes[4]) {
        return;
    }

    PORTB::set_high<PINB5>();
    // usart::send_hex(bytes[1]);
}

int main() {
    DDRB::enable_output<PINB5>();
    PORTB::set_low<PINB5>();

    // prepare pin
    usart::init(9600);
    // Analog2Digit::enable<mcu::ADCReference::AVCC, mcu::ADCPrescaler::CLK_8>();
    // Analog2Digit::prepare<PINC0>();

    _delay_ms(200);

    // init_lcd();
    // LCD::init();
    // LCD::set_cursor_l2(0);
    // LCD::write("Hello world!");

    while (true) {
        measure();
        _delay_ms(1000);
    }
}
