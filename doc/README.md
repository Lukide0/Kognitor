# Documentation

This document explains how to modify the code and its structure.

## Folder Structure

- `cmake`
- `doc`
- `include`
  - `com` – Communication protocols
  - `component` – Components such as sensors, displays, etc.
  - `types` – Types and traits
- `tools`

## Adding a new sensor component

To add a new sensor, create a header file in `include/component/sensor/xxx.h`.

## How to add new sensor to the app?

In `main.cpp`, create a `struct` that inherits from `SensorBase`. The `SensorBase` template parameters include the measured data type and flags that indicate whether the sensor supports enable/disable and watch methods.

- The `enable`method is called at startup and when the sensor is re-enabled.
- The `disable` method is called when the sensor is disabled.
- The `watch` method is called after every successful measurement.

The `app_t` type is defined in `main.cpp` and takes at least three arguments:
1. A boolean specifying the application type (`false` for no user interface, `true` for a UI-enabled application). If `true`, the application must be connected to an LCD display using the pin configuration defined in `lcd.h`.
2. The number of values stored in the ring buffer (oldest values are replaced as new ones arrive).
3. The sensor, followed by additional sensors.

### Example

```cpp
// Sensor pins
using joystick_info_t = JoystickInfo<Input::ADC5, Input::ADC4>;

// Sensor reader
using joystick        = Joystick<ADC, joystick_info_t>;

struct JoystickData {
    uint16_t x;
    uint16_t y;
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

// Application definition.
using app_t = App<false, 5, JoystickSensor, SomeSensor, AnotherSensor>;
```
