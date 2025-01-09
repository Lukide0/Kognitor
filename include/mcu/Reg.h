#ifndef MCU_REG_H
#define MCU_REG_H

#include "concepts.h"
#include "mcu/PartType.h"
#include "types/collection.h"
#include "types/conditional.h"
#include "types/traits.h"
#include <stdint.h>

namespace mcu {

template <uint8_t Bit, bool Write = true, bool Read = true> struct RegBit {
    static constexpr uint8_t bit = 1 << Bit;
    static constexpr bool write  = Write;
    static constexpr bool read   = Read;
};

template <typename T>
concept is_regbit = requires {
    T::bit;
    T::write;
    T::read;

    { T::bit } -> similar<uint8_t>;
    { T::write } -> similar<bool>;
    { T::read } -> similar<bool>;
};

using RB0 = RegBit<0>;
using RB1 = RegBit<1>;
using RB2 = RegBit<2>;
using RB3 = RegBit<3>;
using RB4 = RegBit<4>;
using RB5 = RegBit<5>;
using RB6 = RegBit<6>;
using RB7 = RegBit<7>;

using RB_ALL = types::tuple<RB0, RB1, RB2, RB3, RB4, RB5, RB6, RB7>;

template <uint32_t Addr, bool Read, bool Write, typename Data = uint8_t, is_regbit... Bits> struct RegBase {
    static constexpr uint32_t reg = Addr;
    using data_t                  = Data;

    using bits = types::conditional_t<sizeof...(Bits) == 0, RB_ALL, types::tuple<Bits...>>;

    template <is_regbit... Bs> static consteval bool contains() { return (types::tuple_contains_v<Bs, bits> || ...); }

    static data_t read()
        requires(Read)
    {
        return *get_ptr();
    }

    static void write(data_t data)
        requires(Write)
    {
        *get_ptr() = data;
    }

    template <is_regbit... Bs>
    static void write()
        requires(Write && sizeof...(Bs) > 0 && contains<Bs...>() && (Bs::write && ...))
    {
        *get_ptr() = (Bs::bit | ...);
    }

    static void set_bits(data_t data)
        requires(Write)
    {
        *get_ptr() |= data;
    }

    template <is_regbit... Bs>
    static void set_bits()
        requires(Write && sizeof...(Bs) > 0 && contains<Bs...>() && (Bs::write && ...))
    {
        *get_ptr() |= (Bs::bit | ...);
    }

    static void unset_bits(data_t data)
        requires(Write)
    {
        *get_ptr() &= ~data;
    }

    template <is_regbit... Bs>
    static void unset_bits()
        requires(Write && sizeof...(Bs) > 0 && contains<Bs...>() && (Bs::write && ...))
    {
        *get_ptr() &= ~(Bs::bit | ...);
    }

    static volatile data_t* get_ptr() { return reinterpret_cast<volatile data_t*>(reg); }
};

template <uint32_t Addr, typename Data, is_regbit... Bits> using WReg = RegBase<Addr, false, true, Data, Bits...>;

template <uint32_t Addr, typename Data, is_regbit... Bits> using RReg = RegBase<Addr, true, false, Data, Bits...>;

template <uint32_t Addr, typename Data, is_regbit... Bits> using RWReg = RegBase<Addr, true, true, Data, Bits...>;

template <typename T>
concept is_reg = requires {
    T::reg;
    typename T::data_t;

    { T::reg } -> similar<uint32_t>;
    { T::get_ptr() } -> same_as<volatile typename T::data_t*>;
};

template <typename T>
concept is_write_reg = requires(T::data_t data) {
    requires is_reg<T>;

    { T::write(data) } -> same_as<void>;
    { T::set_bits(data) } -> same_as<void>;
    { T::unset_bits(data) } -> same_as<void>;
};

template <typename T>
concept is_read_reg = requires {
    requires is_reg<T>;

    { T::read() } -> same_as<typename T::data_t>;
};

template <typename T>
concept is_read_write_reg = is_read_reg<T> && is_write_reg<T>;

template <typename T, typename Data>
concept is_data_reg = requires {
    requires is_read_write_reg<T>;
    requires same_as<typename T::data_t, Data>;
};

}
#endif
