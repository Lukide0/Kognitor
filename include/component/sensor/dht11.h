#ifndef COMPONENT_SENSOR_DHT11_H
#define COMPONENT_SENSOR_DHT11_H

#include <microstd/mcu/io.h>
#include <microstd/time/timer.h>
#include <microstd/time/wait.h>
#include <microstd/types/array.h>

#include <stdint.h>
#include <util/delay.h>

#include "types/sensor_pin.h"

namespace component::sensor {

struct dht11_data_t {
    uint8_t humidity_int;
    uint8_t humidity_dec;
    uint8_t temperature_int;
    uint8_t temperature_dec;
};

template <types::sensor_pin Pin, microstd::time::avr::is_avr_timer Timer> class dht11 {
private:
    using timer_t  = Timer;
    using pin_info = Pin;

    template <bool WaitUntilOn, uint8_t Us> static bool wait_until(uint8_t& count) {
        return wait_timeout<
            WaitUntilOn,
            typename Pin::pin_bit,
            timer_t,
            microstd::time::avr::ClockSource256,
            microstd::time::Microseconds<Us>>(count);
    }

    template <uint8_t Us> static bool wait_until_on(uint8_t& count) {
        bool res
            = microstd::time::wait_for_bit_on<typename pin_info::pin_bit, timer_t, microstd::time::Microseconds<Us>>();

        count = timer_t::get_value();
        return res;
    }

    template <uint8_t Us> static bool wait_until_off(uint8_t& count) {
        bool res
            = microstd::time::wait_for_bit_off<typename pin_info::pin_bit, timer_t, microstd::time::Microseconds<Us>>();

        count = timer_t::get_value();
        return res;
    }

    using raw_data_t = microstd::types::array_t<uint8_t, 5>;

public:
    static void prepare() {
        pin_info::port_bit::set();
        pin_info::ddr_bit::unset();
    }

    static bool measure(dht11_data_t& data) {
        raw_data_t raw_data;

        pin_info::port_bit::unset();
        pin_info::ddr_bit::set();

        // wait atleast 18ms
        _delay_ms(19);

        pin_info::port_bit::set();
        pin_info::ddr_bit::unset();

        uint8_t tmp;
        if (!wait_until_off<20>(tmp)) {
            return false;
        }

        // ~80us low signal
        if (!wait_until_on<100>(tmp)) {
            return false;
        }

        // ~80us high signal
        if (!wait_until_off<100>(tmp)) {
            return false;
        }

        for (uint8_t b = 0; b < 5; ++b) {
            uint8_t byte = 0;
            for (uint8_t i = 0; i < 8; ++i) {
                uint8_t low;
                // ~50us signal
                if (!wait_until_on<75>(low)) {
                    return false;
                }

                uint8_t high;
                if (!wait_until_off<100>(high)) {
                    return false;
                }

                /*
                 * Bit format:
                 * +-------+--------------+--------------+
                 * |  Bit  |  High (µs)   | Low (µs)     |
                 * +-------+--------------+--------------+
                 * |   0   |   ~26-28     |     ~50      |
                 * |   1   |     ~70      |     ~50      |
                 * +-------+--------------+--------------+
                 * Bits are determined by comparing high and low durations.
                 */
                if (low <= high) {
                    byte |= 1 << (7 - i);
                }
            }
            raw_data[b] = byte;
        }

        // checksum
        if (raw_data[0] + raw_data[1] + raw_data[2] + raw_data[3] != raw_data[4]) {
            return false;
        }

        data.humidity_int    = raw_data[0];
        data.humidity_dec    = raw_data[1];
        data.temperature_int = raw_data[2];
        data.temperature_dec = raw_data[3];

        return true;
    }
};

}

#endif
