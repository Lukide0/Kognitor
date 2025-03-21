#include "app.h"
#include "component/sensor/dht11.h"
#include "component/sensor/joystick.h"
#include "mcu/ADC.h"
#include "mcu/io.h"
#include "mcu/Port.h"
#include "types/sensors.h"
#include <stdint.h>
#include <util/delay.h>

constexpr uint16_t baudrate = 9600;

using namespace component::sensor;

using types::SensorBase;
using types::SensorFlags;
using types::SensorsCollection;

using ADC   = mcu::ADC<io::ADCSRA, io::ADCSRB, io::ADMUX, io::ADCL, io::ADCH, io::PRR0>;
using TIMER = io::TIMER0;

using joystick_info_t = JoystickInfo<io::PINC5, io::PINC4>;
using joystick        = Joystick<ADC, joystick_info_t>;

using temperature = dht11<mcu::FullPinInfo<io::PIND4, io::PORTD, io::DDRD, io::PIND>, TIMER>;

struct JoystickData {
    uint8_t x;
    uint8_t y;
};

struct TemperatureData {
    uint8_t temp;
};

struct JoystickSensor : SensorBase<JoystickData, SensorFlags::HAS_ENABLE | SensorFlags::HAS_WATCH> {
    // Shortcut to base
    using Base = SensorBase<JoystickData, SensorFlags::HAS_ENABLE | SensorFlags::HAS_WATCH>;

    static optional_data_t measure() {
        joystick::prepare_for_read();

        JoystickData data;
        data.x = joystick::read_x();
        data.y = joystick::read_y();
        return optional_data_t::some(data);
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

struct TemperatureSensor : SensorBase<TemperatureData, SensorFlags::HAS_ENABLE | SensorFlags::HAS_WATCH> {
    // Shortcut to base
    using Base = SensorBase<TemperatureData, SensorFlags::HAS_ENABLE | SensorFlags::HAS_WATCH>;

    static optional_data_t measure() {
        dht11_data_t data;
        if (temperature::measure(data)) {
            return optional_data_t::some(TemperatureData {
                .temp = data.temperature_int,
            });
        }

        return optional_data_t::none();
    }

    static void enable() { temperature::prepare(); }

    static void disable() { }

    static void usart_send(const data_t& data) { Base::usart_send(data.temp); }

    static void watch(const data_t& data) {
        if (data.temp > 25) {
            io::PORTB::set_high<io::PINB5>();
        } else {
            io::PORTB::set_low<io::PINB5>();
        }
    }
};

// The first argument is number of cached values for each sensor. If the sensor is disabled then the value is
// copied from last cached value. After the whole cache is full then the oldest value is dropped.
//
// The next arguments are sensors.
using app_t = App<5, JoystickSensor, TemperatureSensor>;

int main() {
    app_t app;
    app.run(baudrate);
}
