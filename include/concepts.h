#ifndef CONCEPTS_H
#define CONCEPTS_H

#include "types/traits.h"
#include <stdint.h>

template <typename T, typename U>
concept same_as = types::are_same_v<T, U>;

template <typename T, typename U>
concept similar = types::are_similar_v<T, U>;

template <typename T>
concept unsigned_type = similar<T, uint8_t> || similar<T, uint16_t> || similar<T, uint32_t> || similar<T, uint64_t>;

#endif
