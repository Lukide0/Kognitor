#ifndef MCU_PINBIT_H
#define MCU_PINBIT_H

#include "concepts.h"
#include <stdint.h>

namespace mcu {

enum class PinProp {
    NONE      = 0,
    DIGITAL   = 1 << 0,
    ANALOG    = 1 << 1,
    SERIAL_TX = 1 << 2,
    SERIAL_RX = 1 << 3,
    SPI_SS    = 1 << 4,
    SPI_MOSI  = 1 << 5,
    SPI_MISO  = 1 << 6,
    SPI_SCK   = 1 << 7,
    I2C_SDA   = 1 << 8,
    I2C_SCL   = 1 << 9,
    PWM       = 1 << 10,
    INTERRUPT = 1 << 11,
};

consteval PinProp operator|(PinProp a, PinProp b) {
    return static_cast<PinProp>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
}

template <uint32_t N, PinProp Props = PinProp::NONE> struct PinBit {
    static constexpr uint32_t pos  = N;
    static constexpr PinProp props = Props;

    template <PinProp OtherProps> [[nodiscard]] static consteval bool has() {
        return (static_cast<uint32_t>(props) & static_cast<uint32_t>(OtherProps)) == static_cast<uint32_t>(OtherProps);
    }
};

template <typename T>
concept is_pinbit = requires {
    T::pos;
    T::props;

    { T::pos } -> similar<uint32_t>;
    { T::props } -> similar<PinProp>;

    { T::template has<PinProp::DIGITAL>() } -> same_as<bool>;
};

}
#endif
