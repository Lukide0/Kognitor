#ifndef TYPES_SENSORS_H
#define TYPES_SENSORS_H

#include "com/usart.h"
#include "concepts.h"
#include "types/bitarray.h"
#include "types/collection.h"
#include "types/conditional.h"
#include <stdint.h>

namespace types {

enum class SensorFlags : uint8_t {
    NONE       = 0,
    HAS_ENABLE = 1 << 0,
    HAS_WATCH  = 1 << 1,
};

consteval SensorFlags operator|(SensorFlags a, SensorFlags b) {
    return static_cast<SensorFlags>(static_cast<uint8_t>(a) | static_cast<uint8_t>(b));
}

consteval bool sensors_flags_has(SensorFlags flags, SensorFlags required) {
    return (static_cast<uint8_t>(flags) & static_cast<uint8_t>(required)) != 0;
}

template <typename T>
concept sensor_has_enable = requires {
    { T::enable() } -> same_as<void>;
    { T::disable() } -> same_as<void>;
};

template <typename T>
concept sensor_has_watch = requires(T::data_t data) {
    { T::watch(data) } -> same_as<void>;
};

template <typename T>
concept sensor = requires(T::data_t data) {
    typename T::data_t;
    T::flags;

    { T::flags } -> similar<SensorFlags>;
    { T::measure() } -> similar<typename T::data_t>;
    { T::usart_send(data) } -> same_as<void>;

    requires !sensors_flags_has(T::flags, SensorFlags::HAS_ENABLE) || sensor_has_enable<T>;
    requires !sensors_flags_has(T::flags, SensorFlags::HAS_WATCH) || sensor_has_watch<T>;
};

template <typename Data, SensorFlags Flags = SensorFlags::NONE> struct SensorBase {
    using data_t = Data;

    static constexpr SensorFlags flags = Flags;

    static void usart_send(uint8_t data) { com::usart::send(data); }

    static void usart_send(uint16_t data) {
        com::usart::send(static_cast<uint8_t>(data >> 8));
        com::usart::send(data);
    }
};

template <uint16_t CacheSize, sensor... Sensors>
    requires(sizeof...(Sensors) < 256)
class SensorsCollection {
private:
    using sensors_t                = tuple<Sensors...>;
    static constexpr uint8_t count = sizeof...(Sensors);

    using bitarray_t     = bitarray<count, uint8_t>;
    using sensors_data_t = tuple<array<typename Sensors::data_t, CacheSize>...>;

    using index_t = conditional_t<(CacheSize < 256), uint8_t, uint16_t>;

public:
    template <uint8_t I>
        requires(I < count)
    using sensor_get_t = tuple_element_t<I, sensors_t>;

    [[nodiscard]] bool is_enabled(uint8_t i) const { return m_enabled.get(i); }

    static consteval uint8_t bitarray_size() { return bitarray_t::count; }

    void enable(uint8_t i) {
        m_enabled.set(i);
        set_state<0, true>(i);
    }

    void disable(uint8_t i) {
        m_enabled.clear(i);
        set_state<0, false>(i);
    }

    void enable_all() {
        m_enabled.set_all();
        set_state_all<0, true>();
    }

    void disable_all() {
        m_enabled.clear_all();
        set_state_all<0, false>();
    }

    void disable_all_watch() { m_watch_enabled.clear_all(); }

    [[nodiscard]] bool is_enabled_watch(uint8_t i) const { return m_watch_enabled.get(i); }

    void enable_watch(uint8_t i) { m_watch_enabled.set(i); }

    void disable_watch(uint8_t i) { m_watch_enabled.clear(i); }

    [[nodiscard]] uint8_t enabled(uint8_t chunk_index) const { return m_enabled.get_raw(chunk_index); }

    [[nodiscard]] uint8_t enabled_watch(uint8_t chunk_index) const { return m_watch_enabled.get_raw(chunk_index); }

    [[nodiscard]] consteval uint8_t chunks_count() const { return m_enabled.storage_size(); }

    void usart_send(uint8_t i) const { usart_send_impl(i); }

    template <uint8_t I>
        requires(I < count)
    decltype(auto) measure() const {
        return tuple_get<I>(m_data).read(m_cache_index);
    }

    void usart_send_all(uint8_t i) const { usart_send_impl<0, true>(i); }

    void measure_all() {
        m_cache_old_index = m_cache_index;
        m_cache_index += 1;
        m_filled |= m_cache_index == (CacheSize - 1);
        m_cache_index %= CacheSize;
        measure_all_impl();
    }

    void force_write_enable(uint8_t chunk_index, uint8_t value) { m_enabled.force_write(chunk_index, value); }

    void force_write_watch(uint8_t chunk_index, uint8_t value) { m_watch_enabled.force_write(chunk_index, value); }

    [[nodiscard]] consteval uint8_t size() const { return count; }

    [[nodiscard]] bool filled() const { return m_filled; }

private:
    bitarray_t m_enabled;
    bitarray_t m_watch_enabled;
    sensors_data_t m_data;
    index_t m_cache_index = CacheSize - 1;
    index_t m_cache_old_index;
    bool m_filled = false;

    template <uint8_t I = 0> void measure_all_impl() {
        if (is_enabled(I)) {
            using sensor_t = sensor_get_t<I>;
            auto value     = sensor_t::measure();

            if constexpr (sensors_flags_has(sensor_t::flags, SensorFlags::HAS_WATCH)) {
                if (is_enabled_watch(I)) {
                    sensor_t::watch(value);
                }
            }

            tuple_get<I>(m_data).write(m_cache_index, value);
        } else {
            auto& arr = tuple_get<I>(m_data);
            auto prev = arr.read(m_cache_old_index);

            arr.write(m_cache_index, prev);
        }

        if constexpr (I + 1 < count) {
            measure_all_impl<I + 1>();
        }
    }

    template <uint8_t I = 0, bool all = false> void usart_send_impl(uint8_t i) const {
        using sensor_t = sensor_get_t<I>;

        if (I == i) {
            if constexpr (all) {
                send_all<I>();

            } else {
                sensor_t::usart_send(measure<I>());
            }
        } else if constexpr (I + 1 < count) {
            usart_send_impl<I + 1>(i);
        }
    }

    template <uint8_t I = 0, bool enable = true>
        requires(I < count)
    void set_state(uint8_t i) {
        if (I == i) {
            set_state<I, enable>();
        } else if constexpr (I + 1 < count) {
            set_state<I + 1, enable>(i);
        }
    }

    template <uint8_t I = 0, bool enable = true>
        requires(I < count)
    void set_state_all() {
        set_state<I, enable>();
        if constexpr (I + 1 < count) {
            set_state<I, enable>();
        }
    }

    template <uint8_t I = 0, bool enable = true>
        requires(I < count)
    void set_state() {
        using sensor_t = sensor_get_t<I>;
        if constexpr (sensors_flags_has(sensor_t::flags, SensorFlags::HAS_ENABLE)) {
            if constexpr (enable) {
                sensor_t::enable();
            } else {
                sensor_t::disable();
            }
        }
    }

    template <uint8_t I>
        requires(I < count)
    void send_all() const {

        using sensor_t = sensor_get_t<I>;

        uint8_t start = 0;
        if (m_filled) {
            start = m_cache_index + 1;
            start %= CacheSize;
        }

        auto& data = tuple_get<I>(m_data);
        for (; start != m_cache_index; start = (start + 1) % CacheSize) {
            sensor_t::usart_send(data.read(start));
        }
        sensor_t::usart_send(data.read(start));
    }
};

}

#endif
