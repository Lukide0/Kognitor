#include "app.h"
#include "component/sensor/joystick.h"
#include "mcu/ADC.h"
#include "mcu/io.h"
#include "types/sensors.h"
#include <stdint.h>
#include <util/delay.h>

constexpr uint16_t baudrate = 9600;

using namespace component::sensor;

using types::SensorBase;
using types::SensorFlags;
using types::SensorsCollection;

using ADC = mcu::ADC<io::ADCSRA, io::ADCSRB, io::ADMUX, io::ADCL, io::ADCH, io::PRR0>;

using sensor_info = JoystickInfo<io::PINC5, io::PINC4>;
using sensor      = Joystick<ADC, sensor_info>;

struct JoystickData {
    uint8_t x;
    uint8_t y;
};

struct JoystickSensor : SensorBase<JoystickData, SensorFlags::HAS_ENABLE | SensorFlags::HAS_WATCH> {
    // Shortcut to base
    using Base = SensorBase<JoystickData, SensorFlags::HAS_ENABLE | SensorFlags::HAS_WATCH>;

    static data_t measure() {
        sensor::prepare_for_read();

        JoystickData data;
        data.x = sensor::read_x();
        data.y = sensor::read_y();
        return data;
    }

    // This function is called when the sensor is enabled
    static void enable() {
        io::DDRB::enable_output<io::PINB5>();
        io::PORTB::set_low<io::PINB5>();
    }

    // This function is called when the sensor is disabled
    static void disable() { }

    // This function is called when the sensor is read
    static void usart_send(const data_t& data) {
        Base::usart_send(data.x);
        Base::usart_send(data.y);
    }

    // This function is called after the measure() only if the watch is enabled
    static void watch(const data_t& data) {
        if (data.x > 0xE0) {
            io::PORTB::set_high<io::PINB5>();

        } else {
            io::PORTB::set_low<io::PINB5>();
        }
    }
};

// The first argument is number of cached values for each sensor. If the sensor is disabled then the value is copied
// from last cached value. After the whole cache is full then the oldest value is dropped.
//
// The next arguments are sensors.
using app_t = App<5, JoystickSensor>;

int main() {
    app_t app;
    app.run(baudrate);
}
