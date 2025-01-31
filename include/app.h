#ifndef APP_H
#define APP_H

#include "com/usart.h"
#include "component/timer.h"
#include "mcu/Interrupt.h"
#include "mcu/io.h"
#include "mcu/Timer.h"
#include "types/collection.h"
#include "types/sensors.h"
#include <stdint.h>
#include <util/delay.h>

extern uint32_t g_seconds;

template <uint16_t CacheSize = 1, types::sensor... Sensors> class App {
private:
    enum State : uint8_t {
        NORMAL = 0x0,

        ENABLE_SENSOR  = 'e',
        DISABLE_SENSOR = 'd',

        SET_INTERVAL = 's',

        SET_SENSOR_WATCH   = 'w',
        CLEAR_SENSOR_WATCH = 'c',

        LIST_SENSORS_STATE = 'l',

        SENSOR_READ = 'r',

        EXPORT_CONFIG = 'E',
        IMPORT_CONFIG = 'I',
    };

    using sensors_t   = types::SensorsCollection<CacheSize, Sensors...>;
    using timer_t     = component::NormalTimer<io::TIMER1>;
    using timer_delay = component::MsUnit<1000>;

    using usart_timer_delay = component::MsUnit<10>;

    static constexpr uint8_t config_size = (sensors_t::bitarray_size() * 2) + (sizeof(uint32_t) / sizeof(uint8_t));

public:
    void run(uint16_t baudrate);

private:
    State state_normal();
    State state_enable_sensor();
    State state_disable_sensor();
    State state_set_interval();
    State state_set_sensor_watch();
    State state_clear_sensor_watch();
    State state_list_sensors_state();
    State state_sensor_read();
    State state_export_config();
    State state_import_config();
    void try_measure();

    static bool read_int(uint8_t& out) {
        types::array<uint8_t, 3> buff;
        if (!read_buff<buff.size()>(buff)) {
            return false;
        }

        uint16_t tmp = 0;

        for (uint8_t i = 0; i < buff.size(); ++i) {
            const uint8_t byte = buff[i];
            if (byte < '0' || byte > '9') {
                return false;
            }

            tmp = tmp * 10 + byte - '0';
        }

        out = tmp;
        return tmp < 0xFF;
    }

    template <uint8_t Size> static bool read_buff(types::array<uint8_t, Size>& out) {
        using com::usart::try_read_timeout;
        using component::TimerClockSource;
        using io::TIMER0;

        for (uint8_t i = 0; i < Size; ++i) {
            if (!try_read_timeout<TIMER0, TimerClockSource::CLK_1024, usart_timer_delay>(out.get()[i])) {
                return false;
            }
        }

        return true;
    }

    template <typename Fn> void sensor_job(Fn callback) {
        uint8_t id;

        if (read_int(id) && id < m_sensors.size()) {
            callback(id);
        }
    }

private:
    sensors_t m_sensors;
    uint32_t m_delay = 5;
};

#define IMPL_STATE(NAME)                                    \
    template <uint16_t CacheSize, types::sensor... Sensors> \
    inline App<CacheSize, Sensors...>::State App<CacheSize, Sensors...>::state_##NAME()

template <uint16_t CacheSize, types::sensor... Sensors> inline void App<CacheSize, Sensors...>::run(uint16_t baudrate) {
    using namespace com;
    usart::init(baudrate);

    m_sensors.enable_all();

    timer_t::start<component::TimerClockSource::CLK_1024, timer_delay>();

    timer_t::enable_interrupt();

    enable_interrupts();

    State state = State::NORMAL;
    while (true) {
        try_measure();

        switch (state) {
        case State::NORMAL:
            state = state_normal();
            break;
        case State::ENABLE_SENSOR:
            state = state_enable_sensor();
            break;
        case State::DISABLE_SENSOR:
            state = state_disable_sensor();
            break;
        case State::SET_INTERVAL:
            state = state_set_interval();
            break;
        case State::SET_SENSOR_WATCH:
            state = state_set_sensor_watch();
            break;
        case State::CLEAR_SENSOR_WATCH:
            state = state_clear_sensor_watch();
            break;
        case State::LIST_SENSORS_STATE:
            state = state_list_sensors_state();
            break;
        case State::SENSOR_READ:
            state = state_sensor_read();
            break;
        case State::EXPORT_CONFIG:
            state = state_export_config();
            break;
        case State::IMPORT_CONFIG:
            state = state_import_config();
            break;
        }
    }
}

template <uint16_t CacheSize, types::sensor... Sensors> inline void App<CacheSize, Sensors...>::try_measure() {
    disable_interrupts();
    if (g_seconds >= m_delay) {
        m_sensors.measure_all();
        g_seconds = 0;
    }
    enable_interrupts();
}

IMPL_STATE(normal) {
    uint8_t byte;

    if (!com::usart::try_read(byte)) {
        return State::NORMAL;
    }

    switch (byte) {
    case State::ENABLE_SENSOR:
    case State::DISABLE_SENSOR:
    case State::SET_INTERVAL:
    case State::SET_SENSOR_WATCH:
    case State::CLEAR_SENSOR_WATCH:
    case State::LIST_SENSORS_STATE:
    case State::SENSOR_READ:
    case State::IMPORT_CONFIG:
    case State::EXPORT_CONFIG:
        return static_cast<State>(byte);
    default:
        return State::NORMAL;
    }
}

IMPL_STATE(enable_sensor) {
    sensor_job([this](uint8_t id) { m_sensors.enable(id); });

    return State::NORMAL;
}

IMPL_STATE(disable_sensor) {
    sensor_job([this](uint8_t id) { m_sensors.disable(id); });

    return State::NORMAL;
}

IMPL_STATE(set_interval) {
    types::array<uint8_t, 3> buff;
    if (!read_buff<buff.size()>(buff)) {
        return State::NORMAL;
    }

    uint8_t time = 0;

    for (uint8_t i = 0; i < buff.size() - 1; ++i) {
        const auto digit = buff[i];
        if (digit > '9' || digit < '0') {
            return State::NORMAL;
        }

        time *= 10;
        time += digit - '0';
    }

    switch (buff.read(buff.size() - 1)) {
    case 'S':
    case 's':
        m_delay = time;
        break;
    case 'M':
    case 'm':
        m_delay = static_cast<uint32_t>(time) * 60;
        break;
    case 'H':
    case 'h':
        m_delay = static_cast<uint32_t>(time) * 60 * 60;
        break;
    default:
        break;
    }

    return State::NORMAL;
}

IMPL_STATE(set_sensor_watch) {
    sensor_job([this](uint8_t id) { m_sensors.enable_watch(id); });

    return State::NORMAL;
}

IMPL_STATE(clear_sensor_watch) {
    sensor_job([this](uint8_t id) { m_sensors.disable_watch(id); });

    return State::NORMAL;
}

IMPL_STATE(list_sensors_state) {
    com::usart::send(m_sensors.size());
    com::usart::send(m_sensors.chunks_count());

    for (uint8_t id = 0; id < m_sensors.chunks_count(); ++id) {
        com::usart::send(m_sensors.enabled(id));
    }

    return State::NORMAL;
}

IMPL_STATE(sensor_read) {
    sensor_job([this](uint8_t id) { m_sensors.usart_send(id); });

    return State::NORMAL;
}

IMPL_STATE(export_config) {

    uint8_t checksum = 0;

    // sensors state
    for (uint8_t id = 0; id < m_sensors.chunks_count(); ++id) {
        uint8_t tmp = m_sensors.enabled(id);
        com::usart::send(tmp);

        checksum += tmp;
    }

    // watch state
    for (uint8_t id = 0; id < m_sensors.chunks_count(); ++id) {
        uint8_t tmp = m_sensors.enabled_watch(id);
        com::usart::send(tmp);

        checksum += tmp;
    }

    // delay
    constexpr uint8_t size = (sizeof(uint32_t) / sizeof(uint8_t));
    for (uint8_t i = 0; i < size; ++i) {
        uint8_t tmp = (m_delay >> (8 * i));
        com::usart::send(tmp);
        checksum += tmp;
    }

    com::usart::send(checksum);

    return State::NORMAL;
}

IMPL_STATE(import_config) {

    types::array<uint8_t, config_size> config_buff;
    if (!read_buff<config_size>(config_buff)) {
        return State::NORMAL;
    }

    const uint8_t checksum = config_buff.read(config_size - 1);
    uint8_t sum            = 0;
    for (uint8_t i = 0; i < config_size; ++i) {
        sum += config_buff[i];
    }

    if (sum != checksum) {
        return State::NORMAL;
    }

    for (uint8_t i = 0; i < m_sensors.chunks_count(); ++i) {
        m_sensors.force_write_enable(i, config_buff[i]);
        m_sensors.force_write_watch(i, config_buff[i + m_sensors.chunks_count()]);
    }

    uint32_t delay = config_buff[config_size - 4] << (8 * 0);
    delay |= static_cast<uint32_t>(config_buff[config_size - 3]) << (8 * 1);
    delay |= static_cast<uint32_t>(config_buff[config_size - 2]) << (8 * 2);
    delay |= static_cast<uint32_t>(config_buff[config_size - 1]) << (8 * 3);

    m_delay = delay;

    return State::NORMAL;
}

SIGNAL(INT_TIMER1_COMPA);

#endif
