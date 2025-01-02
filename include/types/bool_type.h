#ifndef TYPES_BOOL_TYPE_H
#define TYPES_BOOL_TYPE_H

namespace types {

struct true_type {
    static constexpr bool value = true;
};

struct false_type {
    static constexpr bool value = false;
};

}

#endif
