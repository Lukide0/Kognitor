#ifndef TYPES_CONDITIONAL_H
#define TYPES_CONDITIONAL_H

namespace types {

template <bool Cond, typename T, typename F> struct conditional;

template <typename T, typename F> struct conditional<true, T, F> {
    using type = T;
};

template <typename T, typename F> struct conditional<false, T, F> {
    using type = F;
};

template <bool Cond, typename T, typename F> using conditional_t = conditional<Cond, T, F>::type;

}

#endif
