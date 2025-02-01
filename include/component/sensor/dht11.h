#ifndef COMPONENT_SENSOR_DHT11_H
#define COMPONENT_SENSOR_DHT11_H

#include "component/timer.h"
#include "mcu/Pin.h"
#include "mcu/Port.h"
#include "types/collection.h"
#include "wait.h"
#include <stdint.h>
#include <util/delay.h>

namespace component::sensor {

struct dht11_data_t {
    uint8_t humidity_int;
    uint8_t humidity_dec;
    uint8_t temperature_int;
    uint8_t temperature_dec;
};

template <mcu::full_pin_info PinInfo, component::timer_info Timer> class dht11 {
private:
    using timer_info = Timer;

    template <bool WaitUntilOn, uint8_t Us> static bool wait_until(uint8_t& count) {
        return wait_timeout<
            WaitUntilOn,
            typename PinInfo::pin,
            typename PinInfo::pinbit,
            timer_info,
            TimerClockSource::CLK_256,
            UsUnit<Us>>(count);
    }

    template <uint8_t Us> static bool wait_until_on(uint8_t& count) { return wait_until<true, Us>(count); }

    template <uint8_t Us> static bool wait_until_off(uint8_t& count) { return wait_until<false, Us>(count); }

    using raw_data_t = types::array<uint8_t, 5>;

public:
    static void prepare() {
        PinInfo::port::template enable_pullup<typename PinInfo::pinbit>();
        PinInfo::ddr::template enable_input<typename PinInfo::pinbit>();
    }

    static bool measure(dht11_data_t& data) {

        raw_data_t raw_data;

        PinInfo::port::template set_low<typename PinInfo::pinbit>();
        PinInfo::ddr::template enable_output<typename PinInfo::pinbit>();

        // wait atleast 18ms
        _delay_ms(19);

        PinInfo::port::template enable_pullup<typename PinInfo::pinbit>();
        PinInfo::ddr::template enable_input<typename PinInfo::pinbit>();

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
            raw_data.write(b, byte);
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
