#ifndef CONCEPTS_H
#define CONCEPTS_H

#include "types/traits.h"
#include <stdint.h>

/**
 * @brief Checks if two types are the same.
 *
 * @tparam T The first type.
 * @tparam U The second type.
 */
template <typename T, typename U>
concept same_as = types::are_same_v<T, U>;

/**
 * @brief Checks if two types are similar.
 *
 * Two types are similar when they are the same by removing 'const' and '&'.
 *
 * @tparam T The first type.
 * @tparam U The second type.
 */
template <typename T, typename U>
concept similar = types::are_similar_v<T, U>;

/**
 * @brief Checks if a type is unsigned.
 *
 * @tparam T The type.
 */
template <typename T>
concept unsigned_type = similar<T, uint8_t> || similar<T, uint16_t> || similar<T, uint32_t> || similar<T, uint64_t>;

#endif
