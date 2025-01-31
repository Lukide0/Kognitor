#include "com/usart.h"
#include "bits.h"
#include "mcu/io.h"
#include <stdint.h>

namespace com::usart {

void init(uint16_t baud) {
    using namespace io;

    const uint16_t ubbr = ((F_CPU / 16) / baud) - 1;

    UBRR0H::write(bits_higher(ubbr));
    UBRR0L::write(bits_lower(ubbr));
    UCSR0B::write<RXEN0, TXEN0>();

    // 8-bit + 1 stop bit
    UCSR0C::write<UCSZ01, UCSZ00>();
}

void init_read(uint16_t baud) {
    using namespace io;

    const uint16_t ubbr = ((F_CPU / 16) / baud) - 1;

    UBRR0H::write(bits_higher(ubbr));
    UBRR0L::write(bits_lower(ubbr));
    UCSR0B::write<RXEN0>();

    // 8-bit + 1 stop bit
    UCSR0C::write<UCSZ01, UCSZ00>();
}

void init_write(uint16_t baud) {
    using namespace io;

    const uint16_t ubbr = ((F_CPU / 16) / baud) - 1;

    UBRR0H::write(bits_higher(ubbr));
    UBRR0L::write(bits_lower(ubbr));
    UCSR0B::write<TXEN0>();

    // 8-bit + 1 stop bit
    UCSR0C::write<UCSZ01, UCSZ00>();
}

void enable_rx_interrupt() {
    using namespace io;
    UCSR0B::set_bits<RXCIE0>();
}

void disable_rx_interrupt() {
    using namespace io;
    UCSR0B::unset_bits<RXCIE0>();
}

void enable_tx_interrupt() {
    using namespace io;
    UCSR0B::set_bits<TXCIE0>();
}

void disable_tx_interrupt() {
    using namespace io;
    UCSR0B::unset_bits<TXCIE0>();
}

void send(uint8_t byte) {
    using namespace io;

    while ((UCSR0A::read() & UDRE0::bit) == 0) { }

    UDR0::write(byte);
}

void read_clear() {
    using namespace io;

    [[maybe_unused]] uint8_t tmp;
    while ((UCSR0A::read() & RXC0::bit) != 0) {
        tmp = UDR0::read();
    }
}

uint8_t read_poll() {
    using namespace io;
    while ((UCSR0A::read() & RXC0::bit) == 0) { }

    return UDR0::read();
}

bool try_read(uint8_t& data) {
    using namespace io;

    if ((UCSR0A::read() & RXC0::bit) == 0) {
        return false;
    }
    data = UDR0::read();
    return true;
}

void send(const char* msg) {
    while (*msg != '\0') {
        send(*msg);
        ++msg;
    }
}

void send_hex(uint8_t number) {
    uint8_t top    = (number >> 4) & 0xF;
    uint8_t bottom = number & 0xF;

    if (top >= 10) {
        top = (top - 10) + 'A';
    } else {
        top += '0';
    }

    if (bottom >= 10) {
        bottom = (bottom - 10) + 'A';
    } else {
        bottom += '0';
    }

    send(top);
    send(bottom);
}
}
