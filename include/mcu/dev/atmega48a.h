#ifndef MCU_DEV_ATMEGA48A_H
#define MCU_DEV_ATMEGA48A_H

#ifndef MCU_IO_H
#    error "Include <mcu/io.h> instead of this file."
#endif

#ifndef INTERRUPT_VECTOR
#    define INTERRUPT_VECTOR

#    define INT_RESET 0
#    define INT_INT0 1
#    define INT_INT1 2
#    define INT_PCINT0 3
#    define INT_PCINT1 4
#    define INT_PCINT2 5
#    define INT_WDT 6
#    define INT_TIMER2_COMPA 7
#    define INT_TIMER2_COMPB 8
#    define INT_TIMER2_OVF 9
#    define INT_TIMER1_CAPT 10
#    define INT_TIMER1_COMPA 11
#    define INT_TIMER1_COMPB 12
#    define INT_TIMER1_OVF 13
#    define INT_TIMER0_COMPA 14
#    define INT_TIMER0_COMPB 15
#    define INT_TIMER0_OVF 16
#    define INT_SPI0_STC 17
#    define INT_USART0_RX 18
#    define INT_USART0_UDRE 19
#    define INT_USART0_TX 20
#    define INT_ADC 21
#    define INT_EE_READY 22
#    define INT_ANALOG_COMP 23
#    define INT_TWI 24
#    define INT_SPM_READY 25

#endif

#include "mcu/Pin.h"
#include "mcu/PinBit.h"
#include "mcu/Port.h"
#include "mcu/Reg.h"
#include "mcu/Timer.h"
#include <stdint.h>

namespace mcu::atmega48a {

// PinBits ----------------------------------------------------------------
namespace {
    using PIND0 = PinBit<0, PinProp::DIGITAL | PinProp::SERIAL_RX>;
    using PIND1 = PinBit<1, PinProp::DIGITAL | PinProp::SERIAL_TX>;
    using PIND2 = PinBit<2, PinProp::DIGITAL | PinProp::INTERRUPT>;
    using PIND3 = PinBit<3, PinProp::DIGITAL | PinProp::INTERRUPT | PinProp::PWM>;
    using PIND4 = PinBit<4, PinProp::DIGITAL>;
    using PIND5 = PinBit<5, PinProp::DIGITAL | PinProp::PWM>;
    using PIND6 = PinBit<6, PinProp::DIGITAL | PinProp::PWM>;
    using PIND7 = PinBit<7, PinProp::DIGITAL>;

    using PINB0 = PinBit<0, PinProp::DIGITAL>;
    using PINB1 = PinBit<1, PinProp::DIGITAL | PinProp::PWM>;
    using PINB2 = PinBit<2, PinProp::DIGITAL | PinProp::SPI_SS | PinProp::PWM>;
    using PINB3 = PinBit<3, PinProp::DIGITAL | PinProp::SPI_MOSI | PinProp::PWM>;
    using PINB4 = PinBit<4, PinProp::DIGITAL | PinProp::SPI_MISO>;
    using PINB5 = PinBit<5, PinProp::DIGITAL | PinProp::SPI_SCK>;

    using PINC0 = PinBit<0, PinProp::DIGITAL | PinProp::ANALOG>;
    using PINC1 = PinBit<1, PinProp::DIGITAL | PinProp::ANALOG>;
    using PINC2 = PinBit<2, PinProp::DIGITAL | PinProp::ANALOG>;
    using PINC3 = PinBit<3, PinProp::DIGITAL | PinProp::ANALOG>;
    using PINC4 = PinBit<4, PinProp::DIGITAL | PinProp::ANALOG | PinProp::I2C_SDA>;
    using PINC5 = PinBit<5, PinProp::DIGITAL | PinProp::ANALOG | PinProp::I2C_SCL>;

    // Pins -------------------------------------------------------------------
    using PINB = Pin<0x03 + 0x20, PINB0, PINB1, PINB2, PINB3, PINB4, PINB5>;
    using PINC = Pin<0x06 + 0x20, PINC0, PINC1, PINC2, PINC3, PINC4, PINC5>;
    using PIND = Pin<0x09 + 0x20, PIND0, PIND1, PIND2, PIND3, PIND4, PIND5, PIND6, PIND7>;

    // Ports ------------------------------------------------------------------
    using PORTB = Port<0x05 + 0x20, PINB>;
    using PORTC = Port<0x08 + 0x20, PINC>;
    using PORTD = Port<0x0B + 0x20, PIND>;

    // DDRs -------------------------------------------------------------------
    using DDRB = DDR<0x04 + 0x20, PORTB>;
    using DDRC = DDR<0x07 + 0x20, PORTC>;
    using DDRD = DDR<0x0A + 0x20, PORTD>;

}

// USART ------------------------------------------------------------------

using UBRR0H = RWReg<0xC5, uint8_t, RB0, RB1, RB2, RB3>;
using UBRR0L = RWReg<0xC4, uint8_t>;

namespace {
    using UMSEL01 = RB7;
    using UMSEL00 = RB6;
    using UPM01   = RB5;
    using UPM00   = RB4;
    using USBS0   = RB3;
    using UCSZ01  = RB2;
    using UCSZ00  = RB1;
    using UCPOL0  = RB0;
    using UCSR0C  = RWReg<0xC2, uint8_t, UMSEL01, UMSEL00, UPM01, UPM00, USBS0, UCSZ01, UCSZ00, UCPOL0>;

    using RXCIE0 = RB7;
    using TXCIE0 = RB6;
    using UDRIE0 = RB5;
    using RXEN0  = RB4;
    using TXEN0  = RB3;
    using UCSZ02 = RB2;
    using RXB80  = RegBit<1, false>;
    using TXB80  = RB0;
    using UCSR0B = RWReg<0xC1, uint8_t, RXCIE0, TXCIE0, UDRIE0, RXEN0, TXEN0, UCSZ02, RXB80, TXB80>;

    using RXC0   = RegBit<7, false>;
    using TXC0   = RegBit<6, false>;
    using UDRE0  = RB5;
    using FE0    = RegBit<4, false>;
    using DOR0   = RegBit<3, false>;
    using UPE0   = RegBit<2, false>;
    using U2X0   = RB1;
    using MPCM0  = RB0;
    using UCSR0A = RWReg<0xC0, uint8_t, RXC0, TXC0, UDRE0, FE0, DOR0, UPE0, U2X0, MPCM0>;
}

using UDR0 = RWReg<0xC6, uint8_t>;

// Pins int ---------------------------------------------------------------
namespace {
    using PCIF2 = RB2;
    using PCIF1 = RB1;
    using PCIF0 = RB0;
    using PCIFR = RWReg<0x1B + 0x20, uint8_t, PCIF2, PCIF1, PCIF0>;

    using INTF1 = RB1;
    using INTF0 = RB0;
    using EIFR  = RWReg<0x1C + 0x20, uint8_t, INTF1, INTF0>;

    using INT1  = RB1;
    using INT0  = RB0;
    using EIMSK = RWReg<0x1D + 0x20, uint8_t, INT1, INT0>;

    using PCIE2 = RB2;
    using PCIE1 = RB1;
    using PCIE0 = RB0;
    using PCICR = RWReg<0x68, uint8_t, PCIE2, PCIE1, PCIE0>;

    using ISC11 = RB3;
    using ISC10 = RB2;
    using ISC01 = RB1;
    using ISC00 = RB0;
    using EICRA = RWReg<0x69, uint8_t, ISC11, ISC10, ISC01, ISC00>;

    using PCINT7 = RB7;
    using PCINT6 = RB6;
    using PCINT5 = RB5;
    using PCINT4 = RB4;
    using PCINT3 = RB3;
    using PCINT2 = RB2;
    using PCINT1 = RB1;
    using PCINT0 = RB0;
    using PCMSK0 = RWReg<0x6B, uint8_t, PCINT7, PCINT6, PCINT5, PCINT4, PCINT3, PCINT2, PCINT1, PCINT0>;

    using PCINT14 = RB6;
    using PCINT13 = RB5;
    using PCINT12 = RB4;
    using PCINT11 = RB3;
    using PCINT10 = RB2;
    using PCINT9  = RB1;
    using PCINT8  = RB0;
    using PCMSK1  = RWReg<0x6C, uint8_t, PCINT14, PCINT13, PCINT12, PCINT11, PCINT10, PCINT9, PCINT8>;

    using PCINT23 = RB7;
    using PCINT22 = RB6;
    using PCINT21 = RB5;
    using PCINT20 = RB4;
    using PCINT19 = RB3;
    using PCINT18 = RB2;
    using PCINT17 = RB1;
    using PCINT16 = RB0;
    using PCMSK2  = RWReg<0x6D, uint8_t, PCINT23, PCINT22, PCINT21, PCINT20, PCINT19, PCINT18, PCINT17, PCINT16>;
}

// GPIOR ------------------------------------------------------------------
using GPIOR0 = RWReg<0x1E + 0x20, uint8_t>;
using GPIOR1 = RWReg<0x2A + 0x20, uint8_t>;
using GPIOR2 = RWReg<0x2B + 0x20, uint8_t>;

// EEPROM -----------------------------------------------------------------
namespace {
    using EEPM1 = RB5;
    using EEPM0 = RB4;
    using EERIE = RB3;
    using EEMPE = RB2;
    using EEPE  = RB1;
    using EERE  = RB0;
    using EECR  = RWReg<0x1F + 0x20, uint8_t, EEPM1, EEPM0, EERIE, EEMPE, EEPE, EERE>;

    using EEDR = RWReg<0x20 + 0x20, uint8_t>;

    using EEAR7 = RB7;
    using EEAR6 = RB6;
    using EEAR5 = RB5;
    using EEAR4 = RB4;
    using EEAR3 = RB3;
    using EEAR2 = RB2;
    using EEAR1 = RB1;
    using EEAR0 = RB0;
    using EEARL = RWReg<0x21 + 0x20, uint8_t, EEAR7, EEAR6, EEAR5, EEAR4, EEAR3, EEAR2, EEAR1, EEAR0>;

    // NOTE: EEARH is unused and must always be 0
    using EEAR9 = RB1;
    using EEAR8 = RB0;
    using EEARH = RWReg<0x22 + 0x20, uint8_t, EEAR9, EEAR8>;
}

// SPI0 --------------------------------------------------------------------
namespace {
    using SPIE0 = RB7;
    using SPE0  = RB6;
    using DORD0 = RB5;
    using MSTR0 = RB4;
    using CPOL0 = RB3;
    using CPHA0 = RB2;
    using SPR01 = RB1;
    using SPR00 = RB0;
    using SPCR0 = RWReg<0x2C + 0x20, uint8_t, SPIE0, SPE0, DORD0, MSTR0, CPOL0, CPHA0, SPR01, SPR00>;

    using SPIF0  = RegBit<7, false>;
    using WCOL0  = RegBit<6, false>;
    using SPI2X0 = RB0;
    using SPSR0  = RWReg<0x2D + 0x20, uint8_t, SPIF0, WCOL0, SPI2X0>;

    using SPDR0 = RWReg<0x2E + 0x20, uint8_t>;
}

// Modifiers --------------------------------------------------------------
namespace {
    using ACD   = RB7;
    using ACBG  = RB6;
    using ACO   = RegBit<5, false>;
    using ACI   = RB4;
    using ACIE  = RB3;
    using ACIC  = RB2;
    using ACIS1 = RB1;
    using ACIS0 = RB0;
    using ACSR  = RWReg<0x30 + 0x20, uint8_t, ACD, ACBG, ACO, ACI, ACIE, ACIC, ACIS1, ACIS0>;

    using SM2  = RB3;
    using SM1  = RB2;
    using SM0  = RB1;
    using SE   = RB0;
    using SMCR = RWReg<0x33 + 0x20, uint8_t, SM2, SM1, SM0, SE>;

    using PUD   = RB4;
    using IVSEL = RB1;
    using IVCE  = RB0;
    using MCUCR = RWReg<0x35 + 0x20, uint8_t, PUD, IVSEL, IVCE>;

    using WDRF  = RB3;
    using BORF  = RB2;
    using EXTRF = RB1;
    using PORF  = RB0;
    using MCUSR = RWReg<0x34 + 0x20, uint8_t, WDRF, BORF, EXTRF, PORF>;

    using SPMIE  = RB7;
    using SIGRD  = RB5;
    using BLBSET = RB3;
    using PGWRT  = RB2;
    using PGERS  = RB1;
    using SPMEN  = RB0;
    using SPMCSR = RWReg<0x36 + 0x20, uint8_t, SPMIE, SIGRD, BLBSET, PGWRT, PGERS, SPMEN>;

    using CLKPCE = RB7;
    using CLKPS3 = RB3;
    using CLKPS2 = RB2;
    using CLKPS1 = RB1;
    using CLKPS0 = RB0;
    using CLKPR  = RWReg<0x61, uint8_t, CLKPCE, CLKPS3, CLKPS2, CLKPS1, CLKPS0>;

    using PRTWI0   = RB7;
    using PRTIM2   = RB6;
    using PRTIM0   = RB5;
    using PRTIM1   = RB3;
    using PRSPI0   = RB2;
    using PRUSART0 = RB1;
    using PRADC    = RB0;
    using PRR0     = RWReg<0x64, uint8_t, PRTWI0, PRTIM2, PRTIM0, PRTIM1, PRSPI0, PRUSART0, PRADC>;

    using CAL7   = RB7;
    using CAL6   = RB6;
    using CAL5   = RB5;
    using CAL4   = RB4;
    using CAL3   = RB3;
    using CAL2   = RB2;
    using CAL1   = RB1;
    using CAL0   = RB0;
    using OSCCAL = RWReg<0x66, uint8_t, CAL7, CAL6, CAL5, CAL4, CAL3, CAL2, CAL1, CAL0>;

    using SREG_I = RB7;
    using SREG_T = RB6;
    using SREG_H = RB5;
    using SREG_S = RB4;
    using SREG_V = RB3;
    using SREG_N = RB2;
    using SREG_Z = RB1;
    using SREG_C = RB0;
    using SREG   = RWReg<0x3F + 0x20, uint8_t, SREG_I, SREG_T, SREG_H, SREG_S, SREG_V, SREG_N, SREG_Z, SREG_C>;

    using WDIF   = RB7;
    using WDIE   = RB6;
    using WDP3   = RB5;
    using WDCE   = RB4;
    using WDE    = RB3;
    using WDP2   = RB2;
    using WDP1   = RB1;
    using WDP0   = RB0;
    using WDTCSR = RWReg<0x60, uint8_t, WDIF, WDIE, WDP3, WDCE, WDE, WDP2, WDP1, WDP0>;

}

// Stack ------------------------------------------------------------------
using SPL = RWReg<0x3D + 0x20, uint8_t>;
using SPH = RWReg<0x3E + 0x20, uint8_t>;
using SP  = RWReg<0x3D + 0x20, uint16_t>;

// Analog to digital ------------------------------------------------------
using ADCL = RReg<0x78, uint8_t>;
using ADCH = RReg<0x79, uint8_t>;
using ADC  = RReg<0x78, uint16_t>;

namespace {
    using ADEN   = RB7;
    using ADSC   = RB6;
    using ADATE  = RB5;
    using ADIF   = RB4;
    using ADIE   = RB3;
    using ADPS2  = RB2;
    using ADPS1  = RB1;
    using ADPS0  = RB0;
    using ADCSRA = RWReg<0x7A, uint8_t, ADEN, ADSC, ADATE, ADIF, ADIE, ADPS2, ADPS1, ADPS0>;

    using ACME   = RB6;
    using ADTS2  = RB2;
    using ADTS1  = RB1;
    using ADTS0  = RB0;
    using ADCSRB = RWReg<0x7B, uint8_t, ACME, ADTS2, ADTS1, ADTS0>;

    using REFS1 = RB7;
    using REFS0 = RB6;
    using ADLAR = RB5;
    using MUX3  = RB3;
    using MUX2  = RB2;
    using MUX1  = RB1;
    using MUX0  = RB0;
    using ADMUX = RWReg<0x7C, uint8_t, REFS1, REFS0, ADLAR, MUX3, MUX2, MUX1, MUX0>;

    using ADC5D = RB5;
    using ADC4D = RB4;
    using ADC3D = RB3;
    using ADC2D = RB2;
    using ADC1D = RB1;
    using ADC0D = RB0;
    using DIDR0 = RWReg<0x7E, uint8_t, ADC5D, ADC4D, ADC3D, ADC2D, ADC1D, ADC0D>;

    using AIN1D = RB1;
    using AIN0D = RB0;
    using DIDR1 = RWReg<0x7F, uint8_t, AIN1D, AIN0D>;
}

// 2-wire0 -----------------------------------------------------------------
namespace {
    using TWBR07 = RB7;
    using TWBR06 = RB6;
    using TWBR05 = RB5;
    using TWBR04 = RB4;
    using TWBR03 = RB3;
    using TWBR02 = RB2;
    using TWBR01 = RB1;
    using TWBR00 = RB0;
    using TWBR0  = RWReg<0xB8, uint8_t, TWBR07, TWBR06, TWBR05, TWBR04, TWBR03, TWBR02, TWBR01, TWBR00>;

    using TWS07  = RegBit<7, false>;
    using TWS06  = RegBit<6, false>;
    using TWS05  = RegBit<5, false>;
    using TWS04  = RegBit<4, false>;
    using TWS03  = RegBit<3, false>;
    using TWPS01 = RB1;
    using TWPS00 = RB0;
    using TWSR0  = RWReg<0xB9, uint8_t, TWS07, TWS06, TWS05, TWS04, TWS03, TWPS01, TWPS00>;

    using TWA06  = RB7;
    using TWA05  = RB6;
    using TWA04  = RB5;
    using TWA03  = RB4;
    using TWA02  = RB3;
    using TWA01  = RB2;
    using TWA00  = RB1;
    using TWGCE0 = RB0;
    using TWAR0  = RWReg<0xBA, uint8_t, TWA06, TWA05, TWA04, TWA03, TWA02, TWA01, TWA00, TWGCE0>;

    using TWDR0 = RWReg<0xBB, uint8_t>;

    using TWINT0 = RB7;
    using TWEA0  = RB6;
    using TWSTA0 = RB5;
    using TWSTO0 = RB4;
    using TWWC0  = RB3;
    using TWEN0  = RB2;
    using TWIE0  = RB0;
    using TWCR0  = RWReg<0xBC, uint8_t, TWINT0, TWEA0, TWSTA0, TWSTO0, TWWC0, TWEN0, TWIE0>;

    using TWAMR0 = RWReg<0xBD, uint8_t, RB7, RB6, RB5, RB4, RB3, RB2, RB1>;
}

// Timers -----------------------------------------------------------------
namespace {
    using TSM     = RB7;
    using PSRASY  = RB1;
    using PSRSYNC = RB0;
    using GTCCR   = RWReg<0x23 + 0x20, uint8_t, TSM, PSRASY, PSRSYNC>;
}

// Timer0 -----------------------------------------------------------------
namespace {
    using OCF0B = RB2;
    using OCF0A = RB1;
    using TOV0  = RB0;
    using TIFR0 = RWReg<0x15 + 0x20, uint8_t, OCF0B, OCF0A, TOV0>;

    using COM0A1 = RB7;
    using COM0A0 = RB6;
    using COM0B1 = RB5;
    using COM0B0 = RB4;
    using WGM01  = RB1;
    using WGM00  = RB0;
    using TCCR0A = RWReg<0x24 + 0x20, uint8_t, COM0A1, COM0A0, COM0B1, COM0B0, WGM01, WGM00>;

    using FOC0A  = RegBit<7, true, false>;
    using FOC0B  = RegBit<6, true, false>;
    using WGM02  = RB3;
    using CS02   = RB2;
    using CS01   = RB1;
    using CS00   = RB0;
    using TCCR0B = RWReg<0x25 + 0x20, uint8_t, FOC0A, FOC0B, WGM02, CS02, CS01, CS00>;

    using TCNT0 = RWReg<0x26 + 0x20, uint8_t>;

    using OCR0A = RWReg<0x27 + 0x20, uint8_t>;
    using OCR0B = RWReg<0x28 + 0x20, uint8_t>;

    using OCIE0B = RB2;
    using OCIE0A = RB1;
    using TOIE0  = RB0;
    using TIMSK0 = RWReg<0x6E, uint8_t, OCIE0B, OCIE0A, TOIE0>;

    using TIMER0 = TimerInfo<uint8_t, TCCR0A, TCCR0B, TCNT0, OCR0A, OCR0B, TIMSK0, TIFR0>;
}

// Timer1 -----------------------------------------------------------------
namespace {
    using ICF1  = RB5;
    using OCF1B = RB2;
    using OCF1A = RB1;
    using TOV1  = RB0;
    using TIFR1 = RWReg<0x16 + 0x20, uint8_t, ICF1, OCF1B, OCF1A, TOV1>;

    using COM1A1 = RB7;
    using COM1A0 = RB6;
    using COM1B1 = RB5;
    using COM1B0 = RB4;
    using WGM11  = RB1;
    using WGM10  = RB0;
    using TCCR1A = RWReg<0x80, uint8_t, COM1A1, COM1A0, COM1B1, COM1B0, WGM11, WGM10>;

    using ICNC1  = RB7;
    using ICES1  = RB6;
    using WGM13  = RB4;
    using WGM12  = RB3;
    using CS12   = RB2;
    using CS11   = RB1;
    using CS10   = RB0;
    using TCCR1B = RWReg<0x81, uint8_t, ICNC1, ICES1, WGM13, WGM12, CS12, CS11, CS10>;

    using FOC1A  = RB7;
    using FOC1B  = RB6;
    using TCCR1C = RWReg<0x82, uint8_t, FOC1A, FOC1B>;

    using TCNT1L = RWReg<0x84, uint8_t>;
    using TCNT1H = RWReg<0x85, uint8_t>;
    using TCNT1  = RWReg<0x84, uint16_t>;

    using ICR1L = RWReg<0x86, uint8_t>;
    using ICR1H = RWReg<0x87, uint8_t>;
    using ICR1  = RWReg<0x86, uint16_t>;

    using OCR1AL = RWReg<0x88, uint8_t>;
    using OCR1AH = RWReg<0x89, uint8_t>;
    using OCR1A  = RWReg<0x88, uint16_t>;

    using OCR1BL = RWReg<0x8A, uint8_t>;
    using OCR1BH = RWReg<0x8B, uint8_t>;
    using OCR1B  = RWReg<0x8A, uint16_t>;

    using ICIE1  = RB5;
    using OCIE1B = RB2;
    using OCIE1A = RB1;
    using TOIE1  = RB0;
    using TIMSK1 = RWReg<0x6F, uint8_t, ICIE1, OCIE1B, OCIE1A, TOIE1>;

    using TIMER1 = TimerInfoEx<uint16_t, TCCR1A, TCCR1B, TCCR1C, TCNT1, OCR1A, OCR1B, ICR1, TIMSK1, TIFR1>;
}

// Timer2 -----------------------------------------------------------------
namespace {
    using OCF2B = RB2;
    using OCF2A = RB1;
    using TOV2  = RB0;
    using TIFR2 = RWReg<0x17 + 0x20, uint8_t, OCF2B, OCF2A, TOV2>;

    using OCIE2B = RB2;
    using OCIE2A = RB1;
    using TOIE2  = RB0;
    using TIMSK2 = RWReg<0x70, uint8_t, OCIE2B, OCIE2A, TOIE2>;

    using COM2A1 = RB7;
    using COM2A0 = RB6;
    using COM2B1 = RB5;
    using COM2B0 = RB4;
    using WGM21  = RB1;
    using WGM20  = RB0;
    using TCCR2A = RWReg<0xB0, uint8_t, COM2A1, COM2A0, COM2B1, COM2B0, WGM21, WGM20>;

    using FOC2A  = RegBit<7, true, false>;
    using FOC2B  = RegBit<6, true, false>;
    using WGM22  = RB3;
    using CS22   = RB2;
    using CS21   = RB1;
    using CS20   = RB0;
    using TCCR2B = RWReg<0xB1, uint8_t, FOC2A, FOC2B, WGM22, CS22, CS21, CS20>;

    using TCNT2 = RWReg<0xB2, uint8_t>;

    using OCR2A = RWReg<0xB3, uint8_t>;
    using OCR2B = RWReg<0xB4, uint8_t>;

    using EXCLK   = RB6;
    using AS2     = RB5;
    using TCN2UB  = RegBit<4, false>;
    using OCR2AUB = RegBit<3, false>;
    using OCR2BUB = RegBit<2, false>;
    using TCR2AUB = RegBit<1, false>;
    using TCR2BUB = RegBit<0, false>;
    using ASSR    = RWReg<0xB6, uint8_t, EXCLK, AS2, TCN2UB, OCR2AUB, OCR2BUB, TCR2AUB, TCR2BUB>;

    using TIMER2 = AsyncTimerInfo<uint8_t, TCCR2A, TCCR2B, TCNT2, OCR2A, OCR2B, TIMSK2, TIFR2, ASSR>;
}
}

#endif
