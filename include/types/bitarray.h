#ifndef TYPES_BITARRAY_H
#define TYPES_BITARRAY_H

#include "concepts.h"
#include "types/traits.h"
#include <stdint.h>

namespace types {

/**
 * @brief Bit array
 *
 * @tparam Size The size of the bit array.
 * @tparam Storage The storage type.
 */
template <uint32_t Size, unsigned_type Storage = uint8_t> class bitarray {
private:
    static constexpr uint32_t bits_per_single = sizeof(Storage) * 8;

public:
    static constexpr uint32_t count = (Size + bits_per_single - 1) / bits_per_single;
    static_assert(count < 256);
    using storage_t = Storage[count];

    /**
     * @brief Set a bit in the bit array.
     *
     * @param i The index of the bit to set.
     */
    void set(uint8_t i) {
        const uint8_t index  = i / bits_per_single;
        const uint8_t offset = i % bits_per_single;

        m_storage[index] |= 1 << offset;
    }

    /**
     * @brief Set all bits in the bit array.
     */
    void set_all() { set_value(0xFF); }

    /**
     * @brief Clear all bits in the bit array.
     */
    void clear_all() { set_value(0); }

    /**
     * @brief Clear a bit in the bit array.
     *
     * @param i The index of the bit to clear.
     */
    void clear(uint8_t i) {
        const uint8_t index  = i / bits_per_single;
        const uint8_t offset = i % bits_per_single;

        m_storage[index] &= ~(1 << offset);
    }

    /**
     * @brief Get a bit from the bit array.
     *
     * @param i The index of the bit to get.
     * @return true if the bit is set, false otherwise.
     */
    [[nodiscard]] bool get(uint8_t i) const {
        const uint8_t index  = i / bits_per_single;
        const uint8_t offset = i % bits_per_single;

        return (m_storage[index] & (1 << offset)) != 0;
    }

    /**
     * @brief Get a raw value from the bit array.
     *
     * @param i The index of the value to get.
     * @return The raw value.
     */
    Storage get_raw(uint8_t i) const { return m_storage[i]; }

    /**
     * @brief Get the size of the bit array.
     *
     * @return The size of the bit array.
     */
    [[nodiscard]] consteval uint8_t storage_size() const { return count; }

    /**
     * @brief Force write a value to the bit array.
     *
     * @param i The index of the value to write.
     * @param value The value to write.
     */
    void force_write(uint8_t i, Storage value) { m_storage[i] = value; }

private:
    storage_t m_storage;

    /**
     * @brief Set all bits in the bit array to a value.
     *
     * @param value The value to set.
     */
    void set_value(uint8_t value) {
        for (uint8_t i = 0; i < count; ++i) {
            m_storage[i] = value;
        }
    }
};

}

#endif
