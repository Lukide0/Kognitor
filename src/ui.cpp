#include "ui.h"
#include <microstd/int_types.h>
#include <microstd/mcu/io.h>
#include <microstd/types/array.h>

using namespace microstd::mcu;

void UI::init_counter() {
    io::DDRD::unset_bits<io::DDRD2, io::DDRD3>();
    io::PORTD::unset_bits<io::PORTD2, io::PORTD3>();

    m_counter_clk = (io::PIND::read() & io::PIND3::bit) != 0;
}

void UI::update_counter() {
    bool clk = (io::PIND::read() & io::PIND3::bit) != 0;
    if (clk != m_counter_clk) {
        bool dt = (io::PIND::read() & io::PIND2::bit) != 0;
        if (dt != clk) {
            m_counter += 1;
        } else {
            m_counter -= 1;
        }

        request_update();
    }

    m_counter_clk = clk;
}

#include "lcd.h"

void UI::init() {
    init_counter();
    lcd_init(LCD_DISP_ON);
}

void UI::update() {
    m_prev_counter = count();
    update_counter();

    if (!m_need_update) {
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
    }

    m_need_update = false;
}

void UI::default_menu() {
    // update menu item selection
    if (m_prev_counter < m_counter) {
        m_menu_item += 1;
        m_menu_item %= 3;
    } else {
        if (m_menu_item > 0) {
            m_menu_item -= 1;
        } else {
            m_menu_item = 2;
        }
    }

    auto menu_items = microstd::types::to_array<const char*>("Sensors", "Interval", "Exit");
    for (uint8_t i = 0; i < menu_items.size(); ++i) {
        lcd_gotoxy(6, (i * 2) + 2);
        lcd_puts(menu_items[i]);
    }

    lcd_gotoxy(4, (m_menu_item * 2) + 2);
    lcd_putc('*');
}

void UI::interval_menu() { }

void UI::sensors_menu() { }
