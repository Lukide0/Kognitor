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

    template <unsigned int I, typename... Types> struct TupleElementImpl;

    template <typename T, typename... Tail> struct TupleElementImpl<0, T, Tail...> {
        using type = T;
    };

    template <unsigned int I, typename T, typename... Tail> struct TupleElementImpl<I, T, Tail...> {
        using type = typename TupleElementImpl<I - 1, Tail...>::type;
    };

    template <unsigned int I, typename T> struct TupleElement;

    template <unsigned int I, typename... Types> struct TupleElement<I, TupleImpl<0, Types...>> {
        using type = TupleElementImpl<I, Types...>::type;
    };

}

template <typename... Types> using tuple = __detail::TupleImpl<0, Types...>;

template <unsigned int I, typename T, typename... Tail> auto& tuple_get(__detail::TupleImpl<I, T, Tail...>& tuple) {
    return tuple.__detail::template Leaf<I, T>::value;
}

template <unsigned int I, typename T, typename... Tail> const auto& tuple_get(const __detail::TupleImpl<I, T, Tail...>& tuple) {
    return tuple.__detail::template Leaf<I, T>::value;
}

template <unsigned int I, typename Tuple> using tuple_element_t = __detail::TupleElement<I, Tuple>::type;

template <typename T, typename Tuple> using tuple_contains                   = __detail::tuple_contains<T, Tuple>;
template <typename T, typename Tuple> inline constexpr bool tuple_contains_v = tuple_contains<T, Tuple>::value;

template <typename T, uint16_t Size> struct array {
    using index_t = conditional_t<(Size < 256), uint8_t, uint16_t>;

    array() = default;

    constexpr array(T data[Size])
        : m_data(data) { }

    void write(index_t index, T data) { m_data[index] = data; }

    constexpr T read(index_t index) const { return m_data[index]; }

    constexpr T* get() { return m_data; }

    constexpr const T* get() const { return m_data; }

    [[nodiscard]] consteval index_t size() const { return Size; }

    constexpr T operator[](index_t index) const { return read(index); }

private:
    T m_data[Size];
};

}

#endif
