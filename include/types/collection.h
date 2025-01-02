#ifndef TYPES_COLLECTION_H
#define TYPES_COLLECTION_H

#include "types/bool_type.h"
#include "types/conditional.h"
#include "types/traits.h"
#include <stdint.h>

namespace types {

namespace __detail {
    template <unsigned int I, typename T> struct Leaf {
        using type = T;
        T value;
    };

    template <unsigned int I, typename... Types> struct TupleImpl;

    template <unsigned int I> struct TupleImpl<I> { };

    template <unsigned int I, typename T, typename... Tail>
    struct TupleImpl<I, T, Tail...> : public Leaf<I, T>, public TupleImpl<I + 1, Tail...> { };

    template <typename T, typename Tuple> struct tuple_contains;

    template <typename T, unsigned int I, typename... Types> struct tuple_contains<T, TupleImpl<I, Types...>> {
        static constexpr bool value = (are_same_v<T, Types> || ...);
    };
}

template <typename... Types> using tuple = __detail::TupleImpl<0, Types...>;

template <unsigned int I, typename T, typename... Tail>
decltype(auto) tuple_get(__detail::TupleImpl<I, T, Tail...>& tuple) {
    return tuple.__detail::template Leaf<I, T>::value;
}

template <typename T, typename Tuple> using tuple_contains            = __detail::tuple_contains<T, Tuple>;
template <typename T, typename Tuple> constexpr bool tuple_contains_v = tuple_contains<T, Tuple>::value;

template <typename T, uint8_t Size> struct array {
    array() = default;

    constexpr array(T data[Size])
        : m_data(data) { }

    void write(uint8_t index, uint8_t data) { m_data[index] = data; }

    constexpr T read(uint8_t index) const { return m_data[index]; }

    constexpr T* get() { return m_data; }

    constexpr const T* get() const { return m_data; }

    [[nodiscard]] consteval uint8_t size() const { return Size; }

    constexpr T operator[](uint8_t index) const { return read(index); }

private:
    T m_data[Size];
};

}

#endif
