#include "app.h"

#include <microstd/mcu/io.h>
#include <microstd/time/timer.h>
#include <stdint.h>
#include <util/delay.h>

#include "component/sensor/dht11.h"
#include "component/sensor/joystick.h"
#include "types/sensor_pin.h"
#include "types/sensors.h"

constexpr uint16_t baudrate = 9600;

using namespace microstd;
using namespace microstd::mcu;

using microstd::mcu::avr::Input;
using ADC = io::AnalogConverter;

using namespace component::sensor;
using ::types::SensorBase;
using ::types::SensorFlags;
using ::types::SensorsCollection;

using joystick_info_t = JoystickInfo<Input::ADC5, Input::ADC4>;
using joystick        = Joystick<ADC, joystick_info_t>;

using temperature = dht11<::types::SensorPin<io::PORTD4, io::DDRD4, io::PIND4>, io::Timer0>;

struct JoystickData {
    uint16_t x;
    uint16_t y;
};

struct TemperatureData {
    uint8_t temp;
};

struct JoystickSensor : SensorBase<JoystickData, SensorFlags::HAS_ENABLE | SensorFlags::HAS_WATCH> {
    // Shortcut to base
    using Base = SensorBase<JoystickData, SensorFlags::HAS_ENABLE | SensorFlags::HAS_WATCH>;

    static optional_data_t measure() {

        JoystickData data;
        data.x = joystick::read_x();
        data.y = joystick::read_y();
        return optional_data_t::some(data);
    }

    // This function is called when the sensor is enabled
    static void enable() {
        io::DDRB5::set();
        io::PORTB5::unset();
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
        if (data.x > 512) {
            io::PORTB5::set();

        } else {
            io::PORTB5::unset();
        }
    }
};

struct TemperatureSensor : SensorBase<TemperatureData, SensorFlags::HAS_ENABLE | SensorFlags::HAS_WATCH> {
    // Shortcut to base
    using Base = SensorBase<TemperatureData, SensorFlags::HAS_ENABLE | SensorFlags::HAS_WATCH>;

    static optional_data_t measure() {
        dht11_data_t data;

        if (temperature::measure(data)) {
            return optional_data_t::some(
                TemperatureData {
                    .temp = data.temperature_int,
                }
            );
        }

        return optional_data_t::none();
    }

    static void enable() { temperature::prepare(); }

    static void disable() { }

    static void usart_send(const data_t& data) { Base::usart_send(data.temp); }

    static void watch(const data_t& data) {
        if (data.temp > 25) {
            io::PORTB5::set();
        } else {
            io::PORTB5::unset();
        }
    }
};

// The first argument is number of cached values for each sensor. If the sensor is disabled then the value is
// copied from last cached value. After the whole cache is full then the oldest value is dropped.
//
// The next arguments are sensors.
using app_t = App<5, TemperatureSensor, JoystickSensor>;

int main() {
    microstd::mcu::io::DDRB5::set();
    microstd::mcu::io::PORTB5::set();

    app_t app;
    app.run(baudrate);
}
