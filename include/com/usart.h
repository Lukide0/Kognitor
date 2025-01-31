#ifndef COM_USART_H
#define COM_USART_H

#include "component/timer.h"
#include "mcu/io.h"
#include "mcu/Timer.h"
#include "types/collection.h"
#include <stdint.h>

#ifndef F_CPU
#    error F_CPU must be defined
#endif

namespace com::usart {

void init(uint16_t baud);
void init_read(uint16_t baud);
void init_write(uint16_t baud);

void enable_rx_interrupt();

void disable_rx_interrupt();
void enable_tx_interrupt();

void disable_tx_interrupt();
void send(uint8_t byte);
void read_clear();
uint8_t read_poll();

bool try_read(uint8_t& data);

inline uint8_t read_unsafe() { return io::UDR0::read(); }

void send(const char* msg);
void send_hex(uint8_t number);

template <mcu::timer_info TimerInfo, component::TimerClockSource Source, component::time_unit Unit>
bool try_read_timeout(uint8_t& data) {
    using namespace component;
    using namespace io;

    using timer_t = NormalTimer<TimerInfo>;
    timer_t::template start<Source, Unit>();

    do {
        if ((UCSR0A::read() & RXC0::bit) != 0) {
            data = UDR0::read();
            return true;
        }
    } while (!timer_t::match());

    return false;
}

}

#endif
