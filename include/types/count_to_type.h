#ifndef TYPES_COUNT_TO_TYPE_H
#define TYPES_COUNT_TO_TYPE_H

#include "types/conditional.h"
#include <stdint.h>

namespace types {

template <uint8_t bits> struct count_to_types {
    // clang-format off
    using type = conditional_t<(bits <= 8), uint8_t,
                    conditional_t<(bits <= 16), uint16_t,
                        conditional_t<(bits <= 32), uint32_t,
                            conditional_t<(bits <= 64), uint64_t, void>>>>;
    // clang-format on
};

}

#endif
