# Documentation

This document explains how to modify the code and its structure.

## Folder Structure

- `cmake`
- `doc`
- `include`
  - `com` – Communication protocols
  - `component` – Components such as sensors, displays, etc.
  - `mcu` – Microcontroller definitions
  - `types` – Types and traits
- `tools`

## Adding a New Microcontroller

Each microcontroller has a dedicated file in `mcu/dev/xxx.h`, which is included in `mcu/io.h`. To add a new microcontroller, create a header file in `mcu/dev/xxx.h` and add conditional compilation in `mcu/io.h`.

### `mcu/dev/xxx.h`

If the microcontroller is similar to an existing one, you can extend or override its definitions:

```cpp
#ifndef MCU_DEV_ATMEGA48PA_H
#define MCU_DEV_ATMEGA48PA_H

#ifndef MCU_IO_H
#    error "Include <mcu/io.h> instead of this file."
#endif

#include "mcu/dev/atmega48a.h"
#include "mcu/Reg.h"
#include <stdint.h>

namespace mcu::atmega48pa {
using namespace atmega48a;

// NOTE: BODS and BODSE only for picoPower devices
using BODS  = RB6;
using BODSE = RB5;
using MCUCR = RWReg<0x35 + 0x20, uint8_t, BODS, BODSE, PUD, IVSEL, IVCE>;

}

#endif
```

If you are not extending an existing definition, you need to define the interrupt vector:

```cpp

#ifndef INTERRUPT_VECTOR
#    define INTERRUPT_VECTOR

#    define INT_RESET 0
#    define INT_INT0 1
#    define INT_INT1 2
#    define INT_PCINT0 3
#    define INT_PCINT1 4
#    define INT_PCINT2 5
#    define INT_WDT 6
// ...

#endif

```

All definitions should be enclosed within a unique namespace:

```cpp

namespace mcu::atmega48a {
  // ...
}
```

You then define all components, registers, and pins for the microcontroller.

## Adding a new sensor component

To add a new sensor, create a header file in `include/component/sensor/xxx.h`.

## How to add new sensor to the app?

In `main.cpp`, create a `struct` that inherits from `SensorBase`. The `SensorBase` template parameters include the measured data type and flags that indicate whether the sensor supports enable/disable and watch methods.

- The `enable`method is called at startup and when the sensor is re-enabled.
- The `disable` method is called when the sensor is disabled.
- The `watch` method is called after every successful measurement.

The `app_t` type is defined in `main.cpp` and takes at least two arguments:

1. The number of values stored in the ring buffer (oldest values are replaced as new ones arrive).
2. The sensor, followed by additional sensors.

### Example

```cpp

struct JoystickData {
    uint8_t x;
    uint8_t y;
};

struct JoystickSensor : SensorBase<JoystickData, SensorFlags::HAS_ENABLE | SensorFlags::HAS_WATCH> {
    // Shortcut to base
    using Base = SensorBase<JoystickData, SensorFlags::HAS_ENABLE | SensorFlags::HAS_WATCH>;

    static optional_data_t measure() {
        sensor::prepare_for_read();

        JoystickData data;
        data.x = sensor::read_x();
        data.y = sensor::read_y();
        return optional_data_t::some(data);
    }

    static void enable() {
        io::DDRB::enable_output<io::PINB5>();
        io::PORTB::set_low<io::PINB5>();
    }

    static void disable() { }

    static void usart_send(const data_t& data) {
        Base::usart_send(data.x);
        Base::usart_send(data.y);
    }

    static void watch(const data_t& data) {
        if (data.x > 0xE0) {
            io::PORTB::set_high<io::PINB5>();

        } else {
            io::PORTB::set_low<io::PINB5>();
        }
    }
};

using app_t = App<5, JoystickSensor, SomeSensor, AnotherSensor>;
```
