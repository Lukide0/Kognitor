#ifndef COM_USART_H
#define COM_USART_H

#include "component/timer.h"
#include "mcu/io.h"
#include "mcu/Timer.h"
#include <stdint.h>

#ifndef F_CPU
#    error F_CPU must be defined
#endif

namespace com::usart {

/**
 * @brief Initializes the USART with the specified baud rate.
 *
 * @param baud The baud rate to be set for USART communication.
 */
void init(uint16_t baud);

/**
 * @brief Initializes the USART only for read with the specified baud rate.
 * @param baud The baud rate to be set for USART communication.
 */
void init_read(uint16_t baud);

/**
 * @brief Initializes the USART only for write with the specified baud rate.
 * @param baud The baud rate to be set for USART communication.
 */
void init_write(uint16_t baud);

/**
 * @brief Enables the RX (Receive) interrupt for USART.
 */
void enable_rx_interrupt();

/**
 * @brief Disables the RX (Receive) interrupt for USART
 */
void disable_rx_interrupt();

/**
 * @brief Enables the TX (Transmit) interrupt for USART.
 */
void enable_tx_interrupt();

/**
 * @brief Disables the TX (Transmit) interrupt for USART.
 */
void disable_tx_interrupt();

/**
 * @brief Sends a single byte over USART.
 *
 * @param byte The byte to be sent.
 */
void send(uint8_t byte);

/**
 * @brief Clears the read buffer by reading and discarding the data.
 */
void read_clear();

/**
 * @brief Polls and reads a single byte from USART.
 *
 * @return The read byte.
 */
uint8_t read_poll();

/**
 * @brief Attempts to read a byte from USART without blocking.
 *
 * @param data Reference to the variable where the read data will be stored.
 * @return true if a byte was read, false otherwise.
 */
bool try_read(uint8_t& data);

/**
 * @brief Reads a single byte from USART without checking if data is available.
 *
 * @return The read byte.
 */
inline uint8_t read_unsafe() { return io::UDR0::read(); }

/**
 * @brief Sends a null-terminated string over USART.
 *
 * @param msg Pointer to the null-terminated string to be sent.
 */
void send(const char* msg);

/**
 * @brief Sends a byte as two hexadecimal characters over USART.
 *
 * @param number The byte to be sent as hexadecimal.
 */
void send_hex(uint8_t number);

/**
 * @brief Attempts to read a byte from USART with a timeout.
 *
 * @tparam TimerInfo Timer info
 * @tparam Source The clock source
 * @tparam Unit The maximum time to wait
 *
 * @param data Reference to the variable where the read data will be stored.
 * @return true if a byte was read within the timeout, false otherwise.
 */
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
