#include "ui.h"
#include "com/usart.h"
#include <microstd/int_types.h>
#include <microstd/mcu/io.h>
#include <microstd/types/array.h>

using namespace microstd::mcu;

constexpr microstd::uint8_t MENU_OFFSET_X       = 4;
constexpr microstd::uint8_t MENU_INDICATOR_SIZE = 2;
constexpr char MENU_INDICATOR_SYMBOL            = '*';

constexpr auto MENU_ITEM_OFFSET_X = MENU_OFFSET_X + MENU_INDICATOR_SIZE;
constexpr auto MENU_ITEM_Y_SIZE   = 2;
constexpr auto MENU_ITEM_OFFSET_Y = 1;

void UI::init_counter() {
    io::DDRD::unset_bits<io::DDRD2, io::DDRD3, io::DDRD4>();
    io::PORTD::unset_bits<io::PORTD2, io::PORTD3>();
    // pull up resistor for button
    io::PORTD::set_bits<io::PORTD4>();

    m_counter_clk = (io::PIND::read() & io::PIND3::bit) != 0;
}

void UI::update_counter() {
    constexpr microstd::uint8_t debounce_threshold = 10;

    auto pin = io::PIND::read();

    // button is up
    if ((pin & io::PIND4::bit) != 0) {

        if (m_btn_counter > 0) {
            m_btn_counter -= 1;

            m_btn_state_prev = m_btn_state;
            m_btn_state      = BtnState::Up;

            // button press
            if (m_btn_state_prev == BtnState::Down) {
                request_update(UPDATE_BUTTON);
            }
        }
    } else {
        if (m_btn_counter < debounce_threshold) {
            m_btn_counter += 1;
        }

        if (m_btn_counter >= debounce_threshold) {
            m_btn_state_prev = m_btn_state;
            m_btn_state      = BtnState::Down;
        }
    }

    bool clk = (pin & io::PIND3::bit) != 0;

    if (clk != m_counter_clk) {
        bool dt = (pin & io::PIND2::bit) != 0;
        if (dt != clk) {
            m_counter += 1;
        } else {
            m_counter -= 1;
        }

        request_update(UPDATE_SCROLL);
    }

    m_counter_clk = clk;
}

bool UI::btn_pressed() { return m_btn_state_prev == BtnState::Down && m_btn_state == BtnState::Up; }

void UI::update_value(value_t& value, value_t min, value_t max) const {
    if ((m_update & UPDATE_SCROLL) == 0) {
        return;
    }

    if (m_prev_counter < m_counter) {
        value += 1;
        if (value >= max) {
            value = min;
        }

    } else {
        if (value > min) {
            value -= 1;
        } else {
            value = max - 1;
        }
    }
}

void UI::update_menu_item(value_t length) { update_value(m_menu_item, 0, length); }

void UI::default_menu() {
    // update menu item selection
    update_menu_item(static_cast<item_t>(DefaultMenu::Len));

    if (btn_pressed()) {

        switch (static_cast<DefaultMenu>(m_menu_item)) {
        case DefaultMenu::Sensors:
            m_menu = Menu::Sensors;
            break;
        case DefaultMenu::Interval:
            m_menu = Menu::Interval;
            break;
        case DefaultMenu::Sleep:
            m_menu = Menu::Sleep;
            break;
        default:
            break;
        }
        m_menu_item = 0;
    }
}

void UI::interval_menu() {
    if (m_value_input) {
        switch (static_cast<IntervalMenu>(m_menu_item)) {
        case IntervalMenu::IntervalUnit:
            update_value(m_interval_unit, 0, static_cast<value_t>(IntervalUnit::Len));
            break;
        case IntervalMenu::IntervalValue:
            update_value(m_interval, 1, 100);
            break;
        default:
            m_value_input = false;
            break;
        }
    } else {
        update_menu_item(static_cast<item_t>(IntervalMenu::Len));
    }

    if (btn_pressed()) {
        switch (static_cast<IntervalMenu>(m_menu_item)) {
        case IntervalMenu::IntervalUnit:
        case IntervalMenu::IntervalValue:
            m_value_input ^= true;
            break;

        case IntervalMenu::Save:
            m_menu        = Menu::Default;
            m_menu_item   = static_cast<item_t>(DefaultMenu::Interval);
            m_value_input = false;

            m_interval           = m_interval_real;
            m_interval_unit_real = m_interval_unit;

            switch (static_cast<IntervalUnit>(m_interval_unit)) {
            case Seconds:
                m_adapter.set_interval(m_adapter.ctx, m_interval);
                break;
            case Minutes:
                m_adapter.set_interval(m_adapter.ctx, static_cast<uint32_t>(m_interval) * 60);
                break;
            case Hours:
                m_adapter.set_interval(m_adapter.ctx, static_cast<uint32_t>(m_interval) * 60 * 60);
                break;
            default:
                break;
            }
            break;
        case IntervalMenu::Restore:
            m_menu        = Menu::Default;
            m_menu_item   = static_cast<item_t>(DefaultMenu::Interval);
            m_value_input = false;

            m_interval_unit = m_interval_unit_real;
            m_interval      = m_interval_real;
            break;
        default:
            m_menu_item   = 0;
            m_value_input = false;
            break;
        }
    }
}

void UI::sensors_menu() {
    if (m_value_input) {
        switch (static_cast<SensorsMenu>(m_menu_item)) {
        case SensorsMenu::Sensor:
            update_value(m_sensor_id, 0, m_sensor_count);
            break;
        default:
            m_value_input = false;
            break;
        }
    } else {
        update_menu_item(static_cast<item_t>(SensorsMenu::Len));
    }

    if (btn_pressed()) {
        switch (static_cast<SensorsMenu>(m_menu_item)) {
        case SensorsMenu::Sensor:
            m_value_input ^= true;
            break;

        case SensorsMenu::SensorEnabled:
            m_adapter.set_enable(m_adapter.ctx, m_sensor_id, !m_adapter.is_enabled(m_adapter.ctx, m_sensor_id));
            break;

        case SensorsMenu::SensorWatch:
            m_adapter.set_watch(m_adapter.ctx, m_sensor_id, !m_adapter.is_watched(m_adapter.ctx, m_sensor_id));
            break;

        case SensorsMenu::Exit:
            m_menu        = Menu::Default;
            m_menu_item   = static_cast<item_t>(DefaultMenu::Sensors);
            m_value_input = false;
            break;
        default:
            m_menu_item   = 0;
            m_value_input = false;
            break;
        }
    }
}

#include "lcd.h"

void UI::sleep_menu() {
    // NOTE: This function is called only when the method "request_update" is called.
    constexpr auto NO = ~YES;
    lcd_sleep(NO);

    m_menu      = Menu::Default;
    m_menu_item = static_cast<item_t>(DefaultMenu::Sleep);
}

template <uint8_t Row> void display_menu_item_single(const char* item) {
    lcd_gotoxy(MENU_ITEM_OFFSET_X, (Row * MENU_ITEM_Y_SIZE) + MENU_ITEM_OFFSET_Y);
    lcd_puts(item);
}

template <uint8_t I = 0, typename... Rest> void display_menu_item(const char* item, Rest... rest) {
    display_menu_item_single<I>(item);

    if constexpr (sizeof...(rest) > 0) {
        display_menu_item<I + 1>(rest...);
    }
}

void display_menu_indicator(microstd::uint8_t selected) {
    lcd_gotoxy(MENU_OFFSET_X, (selected * MENU_ITEM_Y_SIZE) + MENU_ITEM_OFFSET_Y);
    lcd_putc(MENU_INDICATOR_SYMBOL);
}

template <typename... Items> void display_menu(microstd::uint8_t selected, Items... items) {
    display_menu_item(items...);
    display_menu_indicator(selected);
}

void UI::update_interval(uint32_t interval) {
    if (interval < 60) {
        m_interval_unit_real = IntervalUnit::Seconds;
        m_interval_real      = interval;
    } else if (interval < (60UL * 60)) {
        m_interval_unit_real = IntervalUnit::Minutes;
        m_interval_real      = interval / 60;
    } else if (interval < (60UL * 60 * 99)) {
        m_interval_unit_real = IntervalUnit::Hours;
        m_interval_real      = interval / (60UL * 60);
    } else {
        m_interval_unit_real = IntervalUnit::Hours;
        m_interval_real      = 99;
    }

    m_interval      = m_interval_real;
    m_interval_unit = m_interval_unit_real;

    m_value_input = false;
}

void UI::init(uint32_t interval, AppAdapter adapter, uint8_t sensors_count) {

    init_counter();
    lcd_init(LCD_DISP_ON);

    update_interval(interval);

    m_adapter      = adapter;
    m_sensor_count = sensors_count;
}

void UI::update() {
    m_prev_counter = counter();
    update_counter();

    if (m_update == UPDATE_NONE) {
        return;
    }

    lcd_clrscr();
    lcd_home();

    switch (m_menu) {
    case Default:
        default_menu();
        break;
    case Sensors:
        sensors_menu();
        break;
    case Interval:
        interval_menu();
        break;
    case Sleep:
        sleep_menu();
        break;
    }

    draw_menu();
    m_update = UPDATE_NONE;
}

void UI::draw_interval_menu() {
    // interval unit
    {
        display_menu_item_single<0>("Unit:");
        bool value_input = m_value_input && m_menu_item == 0;

        if (value_input) {
            lcd_putc('[');
        } else {
            lcd_putc(' ');
        }

        switch (static_cast<IntervalUnit>(m_interval_unit)) {
        case Seconds:
            lcd_puts("seconds");
            break;
        case Minutes:
            lcd_puts("minutes");
            break;
        case Hours:
            lcd_puts(" hours ");
            break;
        default:
            break;
        }

        if (value_input) {
            lcd_putc(']');
        }
    }

    // interval value
    {
        display_menu_item_single<1>("Value:");
        bool value_input = m_value_input && m_menu_item == 1;

        if (value_input) {
            lcd_putc('[');
        } else {
            lcd_putc(' ');
        }

        uint8_t a = m_interval % 10;
        uint8_t b = m_interval / 10;

        lcd_putc('0' + b);
        lcd_putc('0' + a);

        if (value_input) {
            lcd_putc(']');
        }
    }

    // restore
    { display_menu_item_single<2>("Restore & exit"); }

    // exit
    { display_menu_item_single<3>("Save & exit"); }

    // indicator
    display_menu_indicator(m_menu_item);
}

void UI::draw_sensors_menu() {
    // sensor id
    {
        display_menu_item_single<0>("Sensor:");
        bool value_input = m_value_input && m_menu_item == 0;

        if (value_input) {
            lcd_putc('[');
        } else {
            lcd_putc(' ');
        }

        uint8_t a = m_sensor_id % 10;
        uint8_t b = m_sensor_id / 10;

        lcd_putc('0' + b);
        lcd_putc('0' + a);

        if (value_input) {
            lcd_putc(']');
        }
    }

    // enabled
    {
        display_menu_item_single<1>("Enabled:");

        lcd_putc('[');
        if (m_adapter.is_enabled(m_adapter.ctx, m_sensor_id)) {
            lcd_putc('X');
        } else {
            lcd_putc(' ');
        }

        lcd_putc(']');
    }

    // watch
    {
        display_menu_item_single<2>("Watch:");

        lcd_putc('[');
        if (m_adapter.is_watched(m_adapter.ctx, m_sensor_id)) {
            lcd_putc('X');
        } else {
            lcd_putc(' ');
        }
        lcd_putc(']');
    }

    // exit
    { display_menu_item_single<3>("Exit"); }

    // indicator
    display_menu_indicator(m_menu_item);
}

void UI::draw_menu() {

    switch (m_menu) {
    case Default:
        display_menu(m_menu_item, "Sensors", "Interval", "Exit");
        break;
    case Interval:
        draw_interval_menu();
        break;
    case Sensors:
        draw_sensors_menu();
        break;
    case Sleep:
        lcd_sleep(YES);
        break;
    }
}
