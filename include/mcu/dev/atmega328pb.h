#ifndef MCU_DEV_ATMEGA328PB_H
#define MCU_DEV_ATMEGA328PB_H

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
#    define INT_USART0_START 26
#    define INT_PCINT3 27
#    define INT_USART1_RX 28
#    define INT_USART1_UDRE 29
#    define INT_USART1_TX 30
#    define INT_USART1_START 31
#    define INT_TIMER3_CAPT 32
#    define INT_TIMER3_COMPA 33
#    define INT_TIMER3_COMPB 34
#    define INT_TIMER3_OVF 35
#    define INT_CFD 36
#    define INT_PTC_EOC 37
#    define INT_PTC_WCOMP 38
#    define INT_CPI_STC 39
#    define INT_TWI1 40
#    define INT_TIMER4_CAPT 41
#    define INT_TIMER4_COMPA 42
#    define INT_TIMER4_COMPB 43
#    define INT_TIMER4_OVF 44

#endif

#include "mcu/dev/atmega328p.h"
#include "mcu/Pin.h"
#include "mcu/PinBit.h"
#include "mcu/Port.h"
#include "mcu/Reg.h"
#include "mcu/Timer.h"
#include <stdint.h>

namespace mcu::atmega328pb {

using namespace atmega328p;

// PinBits --------------------------------------------------------------------
namespace {
    using PINE0 = PinBit<0, PinProp::DIGITAL | PinProp::I2C_SDA>;
    using PINE1 = PinBit<1, PinProp::DIGITAL | PinProp::I2C_SDA>;
    using PINE2 = PinBit<2, PinProp::DIGITAL | PinProp::ANALOG | PinProp::SPI_SS>;
    using PINE3 = PinBit<3, PinProp::DIGITAL | PinProp::ANALOG | PinProp::SPI_MOSI>;

    using PIND0 = PinBit<0, PinProp::DIGITAL | PinProp::SERIAL_RX | PinProp::PWM>;
    using PIND1 = PinBit<1, PinProp::DIGITAL | PinProp::SERIAL_TX | PinProp::PWM>;
    using PIND2 = PinBit<2, PinProp::DIGITAL | PinProp::INTERRUPT | PinProp::PWM>;
    using PIND3 = PinBit<3, PinProp::DIGITAL | PinProp::INTERRUPT | PinProp::PWM>;
    using PIND4 = PinBit<4, PinProp::DIGITAL>;
    using PIND5 = PinBit<5, PinProp::DIGITAL | PinProp::PWM>;
    using PIND6 = PinBit<6, PinProp::DIGITAL | PinProp::PWM>;
    using PIND7 = PinBit<7, PinProp::DIGITAL>;

    using PINB0 = PinBit<0, PinProp::DIGITAL>;
    using PINB1 = PinBit<1, PinProp::DIGITAL | PinProp::PWM>;
    using PINB2 = PinBit<2, PinProp::DIGITAL | PinProp::SPI_SS | PinProp::PWM | PinProp::SERIAL_RX>;
    using PINB3 = PinBit<3, PinProp::DIGITAL | PinProp::SPI_MOSI | PinProp::PWM | PinProp::SERIAL_TX>;
    using PINB4 = PinBit<4, PinProp::DIGITAL | PinProp::SPI_MISO>;
    using PINB5 = PinBit<5, PinProp::DIGITAL | PinProp::SPI_SCK>;

    using PINC0 = PinBit<0, PinProp::DIGITAL | PinProp::ANALOG | PinProp::SPI_MISO>;
    using PINC1 = PinBit<1, PinProp::DIGITAL | PinProp::ANALOG | PinProp::SPI_SCK>;
    using PINC2 = PinBit<2, PinProp::DIGITAL | PinProp::ANALOG>;
    using PINC3 = PinBit<3, PinProp::DIGITAL | PinProp::ANALOG>;
    using PINC4 = PinBit<4, PinProp::DIGITAL | PinProp::ANALOG | PinProp::I2C_SDA>;
    using PINC5 = PinBit<5, PinProp::DIGITAL | PinProp::ANALOG | PinProp::I2C_SCL>;

    using PINB = Pin<0x03 + 0x20, PINB0, PINB1, PINB2, PINB3, PINB4, PINB5>;
    using PINC = Pin<0x06 + 0x20, PINC0, PINC1, PINC2, PINC3, PINC4, PINC5>;
    using PIND = Pin<0x09 + 0x20, PIND0, PIND1, PIND2, PIND3, PIND4, PIND5, PIND6, PIND7>;
    using PINE = Pin<0x0C + 0x20, PINE0, PINE1, PINE2, PINE3>;

    using PORTB = Port<0x05 + 0x20, PINB>;
    using PORTC = Port<0x08 + 0x20, PINC>;
    using PORTD = Port<0x0B + 0x20, PIND>;
    using PORTE = Port<0x0E + 0x20, PINE>;

    using DDRB = DDR<0x04 + 0x20, PORTB>;
    using DDRC = DDR<0x07 + 0x20, PORTC>;
    using DDRD = DDR<0x0A + 0x20, PORTD>;
    using DDRE = DDR<0x0D + 0x20, PORTE>;
}

// debugWIRE ------------------------------------------------------------------
using DWDR = RWReg<0x31 + 0x20, uint8_t>;

// USART1 ---------------------------------------------------------------------
using UBRR1H = RWReg<0xCD, uint8_t, RB0, RB1, RB2, RB3>;
using UBRR1L = RWReg<0xCC, uint8_t>;

namespace {
    using UMSEL11 = RB7;
    using UMSEL10 = RB6;
    using UPM11   = RB5;
    using UPM10   = RB4;
    using USBS1   = RB3;
    using UCSZ11  = RB2;
    using UCSZ10  = RB1;
    using UCPOL1  = RB0;
    using UCSR1C  = RWReg<0xCA, uint8_t, UMSEL11, UMSEL10, UPM11, UPM10, USBS1, UCSZ11, UCSZ10, UCPOL1>;

    using RXCIE1 = RB7;
    using TXCIE1 = RB6;
    using UDRIE1 = RB5;
    using RXEN1  = RB4;
    using TXEN1  = RB3;
    using UCSZ12 = RB2;
    using RXB81  = RegBit<1, false>;
    using TXB81  = RB0;
    using UCSR1B = RWReg<0xC9, uint8_t, RXCIE1, TXCIE1, UDRIE1, RXEN1, TXEN1, UCSZ12, RXB81, TXB81>;

    using RXC1   = RegBit<7, false>;
    using TXC1   = RegBit<6, false>;
    using UDRE1  = RB5;
    using FE1    = RegBit<4, false>;
    using DOR1   = RegBit<3, false>;
    using UPE1   = RegBit<2, false>;
    using U2X1   = RB1;
    using MPCM1  = RB0;
    using UCSR1A = RWReg<0xC8, uint8_t, RXC1, TXC1, UDRE1, FE1, DOR1, UPE1, U2X1, MPCM1>;
}

using UDR1 = RWReg<0xC7, uint8_t>;

// SPI1 -----------------------------------------------------------------------
namespace {
    using SPIE1 = RB7;
    using SPE1  = RB6;
    using DORD1 = RB5;
    using MSTR1 = RB4;
    using CPOL1 = RB3;
    using CPHA1 = RB2;
    using SPR11 = RB1;
    using SPR10 = RB0;
    using SPCR1 = RWReg<0xAC, uint8_t, SPIE1, SPE1, DORD1, MSTR1, CPOL1, CPHA1, SPR11, SPR10>;

    using SPIF1  = RegBit<7, false>;
    using WCOL1  = RegBit<6, false>;
    using SPI2X1 = RB0;
    using SPSR1  = RWReg<0xAD, uint8_t, SPIF1, WCOL1, SPI2X1>;

    using SPDR1 = RWReg<0xAE, uint8_t>;
}

// 2-wire1 -----------------------------------------------------------------
namespace {
    using TWBR17 = RB7;
    using TWBR16 = RB6;
    using TWBR15 = RB5;
    using TWBR14 = RB4;
    using TWBR13 = RB3;
    using TWBR12 = RB2;
    using TWBR11 = RB1;
    using TWBR10 = RB0;
    using TWBR1  = RWReg<0xD8, uint8_t, TWBR17, TWBR16, TWBR15, TWBR14, TWBR13, TWBR12, TWBR11, TWBR10>;

    using TWS17  = RegBit<7, false>;
    using TWS16  = RegBit<6, false>;
    using TWS15  = RegBit<5, false>;
    using TWS14  = RegBit<4, false>;
    using TWS13  = RegBit<3, false>;
    using TWPS11 = RB1;
    using TWPS10 = RB0;
    using TWSR1  = RWReg<0xD9, uint8_t, TWS17, TWS16, TWS15, TWS14, TWS13, TWPS11, TWPS10>;

    using TWA16  = RB7;
    using TWA15  = RB6;
    using TWA14  = RB5;
    using TWA13  = RB4;
    using TWA12  = RB3;
    using TWA11  = RB2;
    using TWA10  = RB1;
    using TWGCE1 = RB0;
    using TWAR1  = RWReg<0xDA, uint8_t, TWA16, TWA15, TWA14, TWA13, TWA12, TWA11, TWA10, TWGCE1>;

    using TWDR1 = RWReg<0xDB, uint8_t>;

    using TWINT1 = RB7;
    using TWEA1  = RB6;
    using TWSTA1 = RB5;
    using TWSTO1 = RB4;
    using TWWC1  = RB3;
    using TWEN1  = RB2;
    using TWIE1  = RB0;
    using TWCR1  = RWReg<0xDC, uint8_t, TWINT1, TWEA1, TWSTA1, TWSTO1, TWWC1, TWEN1, TWIE1>;

    using TWAMR1 = RWReg<0xDD, uint8_t, RB7, RB6, RB5, RB4, RB3, RB2, RB1>;
}

// Modifiers ------------------------------------------------------------------
namespace {
    using JTD   = RB7;
    using BODS  = RB6;
    using BODSE = RB5;
    using PUD   = RB4;
    using IVSEL = RB1;
    using IVCE  = RB0;
    using MCUCR = RWReg<0x35 + 0x20, uint8_t, JTD, BODS, BODSE, PUD, IVSEL, IVCE>;

    using JTRF  = RB4;
    using WDRF  = RB3;
    using BORF  = RB2;
    using EXTRF = RB1;
    using PORF  = RB0;
    using MCUSR = RWReg<0x34 + 0x20, uint8_t, JTRF, WDRF, BORF, EXTRF, PORF>;

    using XFDIF  = RegBit<1, false>;
    using XFDIE  = RB0;
    using XFDCSR = RWReg<0x62, uint8_t, XFDIF, XFDIE>;

    using PRTWI0   = RB7;
    using PRTIM2   = RB6;
    using PRTIM0   = RB5;
    using PRUSART1 = RB4;
    using PRTIM1   = RB3;
    using PRSPI0   = RB2;
    using PRUSART0 = RB1;
    using PRADC    = RB0;
    using PRR0     = RWReg<0x64, uint8_t, PRTWI0, PRTIM2, PRTIM0, PRUSART1, PRTIM1, PRSPI0, PRUSART0, PRADC>;

    using PRTWI1 = RB5;
    using PRPTC  = RB4;
    using PRTIM4 = RB3;
    using PRSPI1 = RB2;
    using PRTIM3 = RB0;
    using PRR1   = RWReg<0x65, uint8_t, PRTWI1, PRPTC, PRTIM4, PRSPI1, PRTIM3>;
}

// Timer3 ---------------------------------------------------------------------
namespace {
    using ICF3  = RB5;
    using OCF3B = RB2;
    using OCF3A = RB1;
    using TOV3  = RB0;
    using TIFR3 = RWReg<0x18 + 0x20, uint8_t, ICF3, OCF3B, OCF3A, TOV3>;

    using COM3A1 = RB7;
    using COM3A0 = RB6;
    using COM3B1 = RB5;
    using COM3B0 = RB4;
    using WGM31  = RB1;
    using WGM30  = RB0;
    using TCCR3A = RWReg<0x90, uint8_t, COM3A1, COM3A0, COM3B1, COM3B0, WGM31, WGM30>;

    using ICNC3  = RB7;
    using ICES3  = RB6;
    using WGM33  = RB4;
    using WGM32  = RB3;
    using CS32   = RB2;
    using CS31   = RB1;
    using CS30   = RB0;
    using TCCR3B = RWReg<0x91, uint8_t, ICNC3, ICES3, WGM33, WGM32, CS32, CS31, CS30>;

    using FOC3A  = RB7;
    using FOC3B  = RB6;
    using TCCR3C = RWReg<0x92, uint8_t, FOC3A, FOC3B>;

    using TCNT3L = RWReg<0x94, uint8_t>;
    using TCNT3H = RWReg<0x95, uint8_t>;
    using TCNT3  = RWReg<0x94, uint16_t>;

    using ICR3L = RWReg<0x96, uint8_t>;
    using ICR3H = RWReg<0x97, uint8_t>;
    using ICR3  = RWReg<0x96, uint16_t>;

    using OCR3AL = RWReg<0x98, uint8_t>;
    using OCR3AH = RWReg<0x99, uint8_t>;
    using OCR3A  = RWReg<0x98, uint16_t>;

    using OCR3BL = RWReg<0x9A, uint8_t>;
    using OCR3BH = RWReg<0x9B, uint8_t>;
    using OCR3B  = RWReg<0x9A, uint16_t>;

    using ICIE3  = RB5;
    using OCIE3B = RB2;
    using OCIE3A = RB1;
    using TOIE3  = RB0;
    using TIMSK3 = RWReg<0x71, uint8_t, ICIE3, OCIE3B, OCIE3A, TOIE3>;

    using TIMER3 = TimerInfoEx<uint16_t, TCCR3A, TCCR3B, TCCR3C, TCNT3, OCR3A, OCR3B, ICR3, TIMSK3, TIFR3>;
}

// Timer4 ---------------------------------------------------------------------
namespace {
    using ICF4  = RB5;
    using OCF4B = RB2;
    using OCF4A = RB1;
    using TOV4  = RB0;
    using TIFR4 = RWReg<0x19 + 0x20, uint8_t, ICF4, OCF4B, OCF4A, TOV4>;

    using COM4A1 = RB7;
    using COM4A0 = RB6;
    using COM4B1 = RB5;
    using COM4B0 = RB4;
    using WGM41  = RB1;
    using WGM40  = RB0;
    using TCCR4A = RWReg<0xA0, uint8_t, COM4A1, COM4A0, COM4B1, COM4B0, WGM41, WGM40>;

    using ICNC4  = RB7;
    using ICES4  = RB6;
    using WGM43  = RB4;
    using WGM42  = RB3;
    using CS42   = RB2;
    using CS41   = RB1;
    using CS40   = RB0;
    using TCCR4B = RWReg<0xA1, uint8_t, ICNC4, ICES4, WGM43, WGM42, CS42, CS41, CS40>;

    using FOC4A  = RB7;
    using FOC4B  = RB6;
    using TCCR4C = RWReg<0xA2, uint8_t, FOC4A, FOC4B>;

    using TCNT4L = RWReg<0xA4, uint8_t>;
    using TCNT4H = RWReg<0xA5, uint8_t>;
    using TCNT4  = RWReg<0xA4, uint16_t>;

    using ICR4L = RWReg<0xA6, uint8_t>;
    using ICR4H = RWReg<0xA7, uint8_t>;
    using ICR4  = RWReg<0xA6, uint16_t>;

    using OCR4AL = RWReg<0xA8, uint8_t>;
    using OCR4AH = RWReg<0xA9, uint8_t>;
    using OCR4A  = RWReg<0x88, uint16_t>;

    using OCR4BL = RWReg<0xAA, uint8_t>;
    using OCR4BH = RWReg<0xAB, uint8_t>;
    using OCR4B  = RWReg<0xAA, uint16_t>;

    using ICIE4  = RB5;
    using OCIE4B = RB2;
    using OCIE4A = RB1;
    using TOIE4  = RB0;
    using TIMSK4 = RWReg<0x72, uint8_t, ICIE4, OCIE4B, OCIE4A, TOIE4>;

    using TIMER4 = TimerInfoEx<uint16_t, TCCR4A, TCCR4B, TCCR4C, TCNT4, OCR4A, OCR4B, ICR4, TIMSK4, TIFR4>;

}

}

#endif
