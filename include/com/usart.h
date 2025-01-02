#ifndef COM_USART_H
#define COM_USART_H

#include "bits.h"
#include "mcu/io.h"
#include "types/collection.h"
#include <stdint.h>

#ifndef F_CPU
#    error F_CPU must be defined
#endif

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

void send(uint8_t byte) {
    using namespace io;

    while ((UCSR0A::read() & UDRE0::bit) == 0) { }

    UDR0::write(byte);
}

uint8_t read_poll() {
    using namespace io;
    while ((UCSR0A::read() & RXC0::bit) == 0) { }

    return UDR0::read();
}

template <uint8_t Size> uint8_t read_buffer(types::array<uint8_t, Size>& buffer) {
    using namespace io;
    uint8_t read = 0;
    while ((UCSR0A::read() & RXC0::bit) != 0) {
        buffer.write(read, UDR0::read());
        read += 1;
    }

    return read;
}

void send(const char* msg) {
    while (*msg != '\0') {
        send(*msg);
        ++msg;
    }
}

}

#endif
