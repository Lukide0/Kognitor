#include "app.h"
#include "component/sensor/joystick.h"
#include "mcu/ADC.h"
#include "mcu/io.h"
#include "types/sensors.h"
#include <stdint.h>
#include <util/delay.h>

using types::SensorBase;
using types::SensorFlags;
using types::SensorsCollection;

using ADC = mcu::ADC<io::ADCSRA, io::ADCSRB, io::ADMUX, io::ADCL, io::ADCH, io::PRR0>;

using namespace component::sensor;

constexpr uint16_t baudrate = 9600;

using sensor_info = JoystickInfo<io::PINC5, io::PINC4>;
using sensor      = Joystick<ADC, sensor_info>;

struct JoystickData {
    uint8_t x;
    uint8_t y;
};

struct JoystickSensor : SensorBase<JoystickData, SensorFlags::HAS_ENABLE | SensorFlags::HAS_WATCH> {
    using Base = SensorBase<JoystickData, SensorFlags::HAS_ENABLE | SensorFlags::HAS_WATCH>;

    static data_t measure() {
        sensor::prepare_for_read();

        JoystickData data;
        data.x = sensor::read_x();
        data.y = sensor::read_y();
        return data;
    }

    static void enable() {
        io::DDRB::enable_output<io::PINB5>();
        io::PORTB::set_low<io::PINB5>();
    }

    static void disable() { }

    static void usart_send(const JoystickData& data) {
        Base::usart_send(data.x);
        Base::usart_send(data.y);
    }

    static void watch(const JoystickData& data) {
        if (data.x > 0xE0) {
            io::PORTB::set_high<io::PINB5>();

        } else {
            io::PORTB::set_low<io::PINB5>();
        }
    }
};

using app_t = App<5, JoystickSensor>;

int main() {
    app_t app;
    app.run(baudrate);
}
