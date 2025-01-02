#ifndef MCU_PORT_H
#define MCU_PORT_H

#include "mcu/PartType.h"
#include "mcu/Pin.h"
#include "mcu/PinBit.h"
#include "mcu/Reg.h"
#include <stdint.h>

namespace mcu {

template <uint32_t Addr, is_pin Pins> struct Port : RWReg<Addr, uint8_t> {
    using RWReg<Addr, uint8_t>::get_ptr;

    using pins                    = Pins;
    static constexpr PartType tag = PartType::PORT;

    template <is_pinbit... PinBits> static consteval bool contains() { return pins::template contains<PinBits...>(); }

    template <is_pinbit... PinBits>
    static void enable_pullup()
        requires(contains<PinBits...>())
    {
        *get_ptr() |= ((1 << PinBits::pos) | ...);
    }

    template <is_pinbit... PinBits>
    static void disable_pullup()
        requires(contains<PinBits...>())
    {
        *get_ptr() &= ~((1 << PinBits::pos) | ...);
    }

    template <is_pinbit... PinBits> static void set_low() { disable_pullup<PinBits...>(); }

    template <is_pinbit... PinBits> static void set_high() { enable_pullup<PinBits...>(); }
};

template <typename T>
concept is_port = is_read_write_reg<T> && is_part<PartType::PORT, T>;

template <uint32_t Addr, is_port Port> struct DDR : RWReg<Addr, uint8_t> {
    using RWReg<Addr, uint8_t>::get_ptr;

    using port                    = Port;
    static constexpr PartType tag = PartType::DDR;

    template <is_pinbit... PinBits> static consteval bool contains() { return port::template contains<PinBits...>(); }

    template <is_pinbit... PinBits>
    static void enable_output()
        requires(contains<PinBits...>())
    {
        *get_ptr() |= ((1 << PinBits::pos) | ...);
    }

    template <is_pinbit... PinBits>
    static void enable_input()
        requires(contains<PinBits...>())
    {
        *get_ptr() &= ~((1 << PinBits::pos) | ...);
    }
};

template <typename T>
concept is_ddr = is_read_write_reg<T> && is_part<PartType::DDR, T>;

}

#endif
