#ifndef COMPONENT_LCD_I2C_H
#define COMPONENT_LCD_I2C_H

#include "com/twi.h"
#include "mcu/Reg.h"
#include <stdint.h>
#include <util/delay.h>

namespace component {

using namespace com;

template <is_twi_config TWI_CONFIG, uint8_t Address, bool TwoLinesMode = true> struct Lcd {
    using TWI = twi<TWI_CONFIG>;

    using LCD_RS  = RB0;
    using LCD_RW  = RB1;
    using LCD_E   = RB2;
    using LCD_BL  = RB3;
    using LCD_DB4 = RB4;
    using LCD_DB5 = RB5;
    using LCD_DB6 = RB6;
    using LCD_DB7 = RB7;

    static bool init() {
        if (!try_start_twi()) {
            return false;
        }

        // 4-bit mode
        send_with_pulse(LCD_DB5::bit);
        _delay_us(40);

        constexpr uint8_t two_lines_mode = 0b00101000;
        constexpr uint8_t one_line_mode  = 0b00100000;

        constexpr auto mode = (TwoLinesMode) ? two_lines_mode : one_line_mode;

        // 4-bit mode, x lines, 5x8 font
        send<false>(mode);
        _delay_us(40);

        // display on, cursor off
        send(0b00001100);
        _delay_us(40);

        // clear display
        send(0b00000001);
        _delay_ms(2);

        // auto increment cursor, no shift
        send(0b00000110);
        _delay_us(40);

        TWI::stop();
        return true;
    }

    static void set_cursor(uint8_t x, uint8_t y) {
        if constexpr (TwoLinesMode) {
            /*
            2-line mode:
                1. line 0x00 ... 0x27
                2. line 0x40 ... 0x67
             */
            if (y > 1 || x > 0x27) {
                return;
            }
            // 1. line
            else if (y == 0) {
                send_command(0x80 + x);
            }
            // 2. line
            else {
                send_command(0x80 + 0x40 + x);
            }
        } else {
            if (y > 0 || x > 0x4F) {
                return;
            }

            send_command(0x80 + x);
        }
    }

    static void set_cursor_l1(uint8_t x) {
        constexpr uint8_t max_x = (TwoLinesMode) ? 0x27 : 0x4F;

        if (x > max_x) {
            return;
        }
        send_command(0x80 + x);
    }

    static void set_cursor_l2(uint8_t x)
        requires(TwoLinesMode)
    {
        if (x > 0x27) {
            return;
        }

        send_command(0x80 + 0x40 + x);
    }

    static void write(uint8_t data) { send_command<true>(data); }

    static void write(const char* str) {
        for (; *str != '\0'; ++str) {
            write(*str);
            _delay_us(30);
        }
    }

    static void clear() { send_command(0x01); }

private:
    static void send_with_pulse(uint8_t value) {
        TWI::send_data(value | LCD_E::bit);
        TWI::send_data(value & ~LCD_E::bit);
    }

    template <bool Backlight = true> static void send(uint8_t data) {
        constexpr uint8_t bl = (Backlight) ? LCD_BL::bit : 0;

        const uint8_t hi = (data & 0xF0) | bl;
        const uint8_t lo = ((data << 4) & 0xF0) | bl;

        send_with_pulse(hi);
        send_with_pulse(lo);
    }

    template <bool Rs = false, bool Backlight = true> static void send_command(uint8_t data) {
        constexpr uint8_t bl = (Backlight) ? LCD_BL::bit : 0;
        constexpr uint8_t rs = (Rs) ? LCD_RS::bit : 0;

        const uint8_t hi = (data & 0xF0) | bl | rs;
        const uint8_t lo = ((data << 4) & 0xF0) | bl | rs;

        TWI::start();
        TWI::prepare_write(Address);

        send_with_pulse(hi);
        send_with_pulse(lo);

        TWI::stop();
    }

    static bool try_start_twi() {
        switch (TWI::start()) {
        case TWI::Status::START:
        case TWI::Status::START_REPEAT:
            break;
        default:
            return false;
        }

        switch (TWI::prepare_write(Address)) {
        case TWI::Status::SLA_W_ACK:
            break;
        case TWI::Status::SLA_W_NACK:
            TWI::stop();
            return false;
        default:
            return false;
        }

        return true;
    }
};

}

#endif
