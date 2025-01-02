#ifndef TYPES_TRAITS_H
#define TYPES_TRAITS_H

#include "types/bool_type.h"

namespace types {

//-- is_same ------------------------------------------------------------------
template <typename T, typename V> struct are_same : false_type { };

template <typename T> struct are_same<T, T> : true_type { };

template <typename T, typename V> constexpr auto are_same_v = are_same<T, V>::value;

//-- remove_ref ---------------------------------------------------------------
template <typename T> struct remove_ref {
    using type = T;
};

template <typename T> struct remove_ref<T&> {
    using type = T;
};

template <typename T> struct remove_ref<T&&> {
    using type = T;
};

//-- remove_const -------------------------------------------------------------
template <typename T> struct remove_const {
    using type = T;
};

template <typename T> struct remove_const<const T> {
    using type = T;
};

//-- remove_ptrs --------------------------------------------------------------
template <typename T> struct remove_ptrs {
    using type = T;
};

template <typename T> struct remove_ptrs<T*> {
    using type = remove_ptrs<T>;
};

//-- remove_cref --------------------------------------------------------------
template <typename T> struct remove_cref {
    using type = remove_const<T>::type;
};

template <typename T> struct remove_cref<T&&> {
    using type = remove_const<T>::type;
};

template <typename T> struct remove_cref<T&> {
    using type = remove_const<T>::type;
};

template <typename T> using remove_cref_t = remove_cref<T>::type;

//-- is_similar ---------------------------------------------------------------
template <typename T, typename V> struct are_similar {
    static constexpr bool value = are_same_v<remove_cref_t<T>, remove_cref_t<V>>;
};

template <typename T, typename V> constexpr auto are_similar_v = are_similar<T, V>::value;

}

#endif
