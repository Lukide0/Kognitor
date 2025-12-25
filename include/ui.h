#ifndef UI_H
#define UI_H

#include <microstd/int_types.h>
#include <microstd/types/conditional.h>

class UI {
public:
    void init();
    void update();

    void request_update() { m_need_update = true; }

    [[nodiscard]] char count() const { return m_counter; }

    void reset_counter() { m_counter = 0; }

private:
    enum Menu {
        Default,
        Sensors,
        Interval,
    };

    using item_t = microstd::uint8_t;

    void default_menu();
    void sensors_menu();
    void interval_menu();

    void init_counter();
    void update_counter();

    bool m_need_update = true;
    item_t m_menu_item = 0;
    Menu m_menu;

    char m_counter      = 0;
    char m_prev_counter = 0;
    bool m_counter_clk  = false;
};

class EmptyUI { };

template <bool Enable> using ui_type = microstd::types::conditional_t<Enable, UI, EmptyUI>;

#endif
