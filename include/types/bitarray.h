#ifndef TYPES_BITARRAY_H
#define TYPES_BITARRAY_H

#include "concepts.h"
#include "types/traits.h"
#include <stdint.h>

namespace types {

template <uint32_t Size, unsigned_type Storage = uint8_t> class bitarray {
private:
    static constexpr uint32_t bits_per_single = sizeof(Storage) * 8;

public:
    static constexpr uint32_t count = (Size + bits_per_single - 1) / bits_per_single;
    static_assert(count < 256);
    using storage_t = Storage[count];

    void set(uint8_t i) {
        const uint8_t index  = i / bits_per_single;
        const uint8_t offset = i % bits_per_single;

        m_storage[index] |= 1 << offset;
    }

    void set_all() { set_value(0xFF); }

    void clear_all() { set_value(0); }

    void clear(uint8_t i) {
        const uint8_t index  = i / bits_per_single;
        const uint8_t offset = i % bits_per_single;

        m_storage[index] &= ~(1 << offset);
    }

    [[nodiscard]] bool get(uint8_t i) const {
        const uint8_t index  = i / bits_per_single;
        const uint8_t offset = i % bits_per_single;

        return (m_storage[index] & (1 << offset)) != 0;
    }

    Storage get_raw(uint8_t i) const { return m_storage[i]; }

    [[nodiscard]] consteval uint8_t storage_size() const { return count; }

    void force_write(uint8_t i, Storage value) { m_storage[i] = value; }

private:
    storage_t m_storage;

    void set_value(uint8_t value) {
        for (uint8_t i = 0; i < count; ++i) {
            m_storage[i] = value;
        }
    }
};

}

#endif
