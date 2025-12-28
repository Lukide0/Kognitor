#ifndef UI_H
#define UI_H

#include <microstd/int_types.h>
#include <microstd/types/conditional.h>

struct AppAdapter {
    void* ctx;
    void (*set_interval)(void*, microstd::uint32_t);
    bool (*is_enabled)(void*, microstd::uint8_t);
    bool (*is_watched)(void*, microstd::uint8_t);

    void (*set_watch)(void*, microstd::uint8_t, bool);
    void (*set_enable)(void*, microstd::uint8_t, bool);
};

class UI {
public:
    void init(microstd::uint32_t interval, AppAdapter adapter, microstd::uint8_t sensors_count);
    void update();
    void update_interval(microstd::uint32_t interval);

    void request_update() { request_update(UPDATE_MANUAL); }

private:
    static constexpr microstd::uint8_t UPDATE_NONE   = 0;
    static constexpr microstd::uint8_t UPDATE_SCROLL = 1;
    static constexpr microstd::uint8_t UPDATE_BUTTON = 2;
    static constexpr microstd::uint8_t UPDATE_MANUAL = 4;

    void request_update(microstd::uint8_t type) { m_update |= type; }

    using item_t  = microstd::uint8_t;
    using value_t = microstd::uint8_t;

    enum Menu {
        Default,
        Sensors,
        Interval,
        Sleep,
    };

    enum class DefaultMenu {
        Sensors,
        Interval,
        Sleep,
        Len,
    };

    enum class IntervalMenu {
        IntervalUnit,
        IntervalValue,
        Restore,
        Save,
        Len,
    };

    enum class SensorsMenu {
        Sensor,
        SensorEnabled,
        SensorWatch,
        Exit,
        Len,
    };

    enum IntervalUnit : value_t {
        Seconds,
        Minutes,
        Hours,
        Len,
    };

    enum class BtnState : bool {
        Down,
        Up,
    };

    void default_menu();
    void sensors_menu();
    void interval_menu();
    void sleep_menu();

    void draw_menu();
    void draw_interval_menu();
    void draw_sensors_menu();

    void init_counter();
    void update_counter();

    void reset_counter() { m_counter = 0; }

    [[nodiscard]] char counter() const { return m_counter; }

    void update_menu_item(item_t length);

    void update_value(value_t& value, value_t min, value_t max) const;

    bool btn_pressed();

    microstd::uint8_t m_update = UPDATE_MANUAL;
    bool m_value_input         = false;
    item_t m_menu_item         = 0;

    Menu m_menu = Menu::Default;

    char m_counter      = 0;
    char m_prev_counter = 0;
    bool m_counter_clk  = false;

    // interval
    value_t m_interval;
    value_t m_interval_unit;

    value_t m_interval_real;
    value_t m_interval_unit_real;

    // sensor
    value_t m_sensor_id = 0;
    microstd::uint8_t m_sensor_count;

    BtnState m_btn_state      = BtnState::Up;
    BtnState m_btn_state_prev = BtnState::Up;

    microstd::uint8_t m_btn_counter = 0;

    AppAdapter m_adapter;
};

class EmptyUI { };

template <bool Enable> using ui_type = microstd::types::conditional_t<Enable, UI, EmptyUI>;

#endif
