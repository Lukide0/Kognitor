#ifndef MCU_PARTTYPE_H
#define MCU_PARTTYPE_H

namespace mcu {

enum class PartType {
    PIN,
    PORT,
    DDR,
};

template <PartType Type, typename T>
concept is_part = requires {
    T::tag;
    requires T::tag == Type;
};

}

#endif
