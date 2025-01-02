#ifndef MCU_PIN_H
#define MCU_PIN_H

#include "mcu/PartType.h"
#include "mcu/PinBit.h"
#include "mcu/Reg.h"
#include "types/collection.h"
#include "types/count_to_type.h"
#include <stdint.h>

namespace mcu {

template <uint32_t Reg, is_pinbit... Pins>
struct Pin : RReg<Reg, typename types::count_to_types<sizeof...(Pins)>::type> {
    using pins                          = types::tuple<Pins...>;
    static constexpr uint8_t pins_count = sizeof...(Pins);

    static constexpr PartType tag = PartType::PIN;

    template <is_pinbit... OtherPins> static consteval bool contains() {
        return (types::tuple_contains_v<Pins, pins> && ...);
    }
};

template <typename T>
concept is_pin = is_read_reg<T> && is_part<PartType::PIN, T>;

}

#endif
