#ifndef TYPES_OPTIONAL_H
#define TYPES_OPTIONAL_H

namespace types {

template <typename T> class optional_t {
public:
    static constexpr optional_t none() { return optional_t(); }

    static constexpr optional_t some(T data) { return optional_t(data); }

    [[nodiscard]] constexpr bool has_value() const { return m_has_value; }

    [[nodiscard]] constexpr T value() const { return m_data; }

private:
    T m_data;
    bool m_has_value;

    constexpr optional_t()
        : m_has_value(false) { }

    constexpr optional_t(T data)
        : m_data(data)
        , m_has_value(true) { }
};
}

#endif
