#ifndef COM_TWI_H
#define COM_TWI_H

#include "mcu/PinBit.h"
#include "mcu/Port.h"
#include "mcu/Reg.h"
#include <stdint.h>

namespace com {

using namespace mcu;

template <
    is_port PORT,
    is_ddr DDR,
    is_pinbit SDA,
    is_pinbit SCL,
    is_read_write_reg TWSR,
    is_read_write_reg TWBR,
    is_read_write_reg TWCR,
    is_read_write_reg TWDR>
    requires(SDA::template has<PinProp::I2C_SDA>() && SCL::template has<PinProp::I2C_SCL>())
struct TWIConfig {
    using port = PORT;
    using ddr  = DDR;
    using sda  = SDA;
    using scl  = SCL;
    using twsr = TWSR;
    using twbr = TWBR;
    using twcr = TWCR;
    using twdr = TWDR;
};

template <typename T>
concept is_twi_config = requires {
    typename T::port;
    requires is_port<typename T::port>;

    typename T::ddr;
    requires is_ddr<typename T::ddr>;

    typename T::sda;
    requires is_pinbit<typename T::sda>;

    typename T::scl;
    requires is_pinbit<typename T::scl>;

    typename T::twsr;
    requires is_read_write_reg<typename T::twsr>;

    typename T::twbr;
    requires is_read_write_reg<typename T::twbr>;

    typename T::twcr;
    requires is_read_write_reg<typename T::twcr>;

    typename T::twdr;
    requires is_read_write_reg<typename T::twdr>;
};

/**
 * @brief The 2-wire interface.
 */
template <is_twi_config Config> struct twi {
private:
    using PORT = typename Config::port;
    using DDR  = typename Config::ddr;
    using SDA  = typename Config::sda;
    using SCL  = typename Config::scl;
    using TWSR = typename Config::twsr;
    using TWBR = typename Config::twbr;
    using TWCR = typename Config::twcr;
    using TWDR = typename Config::twdr;

    using TWINT = RB7;
    using TWEA  = RB6;
    using TWSTA = RB5;
    using TWSTO = RB4;
    using TWWC  = RB3;
    using TWEN  = RB2;
    using TWIE  = RB0;

    static constexpr uint8_t STATUS_MASK  = 0b11111000;
    static constexpr auto STATUS_MASK_INV = static_cast<uint8_t>(~STATUS_MASK);

    static constexpr uint8_t READ_OP  = 1;
    static constexpr uint8_t WRITE_OP = 0;

public:
    enum class Status : uint8_t {
        START          = 0x08,
        START_REPEAT   = 0x10,
        SLA_W_ACK      = 0x18,
        SLA_W_NACK     = 0x20,
        DATA_SEND_ACK  = 0x28,
        DATA_SEND_NACK = 0x30,
        LOST           = 0x38,
        SLA_R_ACK      = 0x40,
        SLA_R_NACK     = 0x48,
        DATA_RECV_ACK  = 0x50,
        DATA_RECV_NACK = 0x58,
    };

    /**
     * @brief Prepares the address for a read or write operation.
     */
    template <bool Read> static constexpr uint8_t prepare_address(uint8_t address) {
        constexpr uint8_t OP = (Read) ? READ_OP : WRITE_OP;

        return (address << 1) | OP;
    }

    /**
     * @brief Initializes the TWI interface.
     *
     * @param freg The frequency to be used for TWI communication.
     */
    static void init(uint16_t freq) {
        DDR::template enable_input<SDA, SCL>();
        PORT::template disable_pullup<SDA, SCL>();

        // prescale = 1
        TWSR::write(0);

        const uint8_t value = ((F_CPU / freq) - 16) / 2;
        TWBR::write(value);
    }

    /**
     * @brief Starts a TWI communication.
     *
     * @returns The status of the TWI communication.
     */
    static Status start() {
        // send start condition
        TWCR::template write<TWINT, TWSTA, TWEN>();

        // wait for start condition to be sent
        while ((TWCR::read() & TWINT::bit) == 0) { }

        return read_status();
    }

    /**
     * @brief Stops a TWI communication.
     */
    static void stop() { TWCR::template write<TWINT, TWEN, TWSTO>(); }

    /**
     * @brief Reads the status of a TWI communication.
     *
     * @returns The status of the TWI communication.
     */
    static Status read_status() { return static_cast<Status>(TWSR::read() & STATUS_MASK); }

    /**
     * @brief Prepares the TWI interface for a write operation.
     *
     * @param address The address of the device.
     *
     * @returns The status of the TWI communication.
     */
    static Status prepare_write(uint8_t address) {
        const uint8_t value = prepare_address<false>(address);

        TWDR::write(value);

        TWCR::template write<TWINT, TWEN>();

        // wait for ACK/NACK
        while ((TWCR::read() & TWINT::bit) == 0) { }

        return read_status();
    }

    /**
     * @brief Sends data over the TWI interface.
     *
     * @param data The data to be sent.
     *
     * @returns The status of the TWI communication.
     */
    static Status send_data(uint8_t byte) {
        TWDR::write(byte);
        TWCR::template write<TWINT, TWEN>();

        while ((TWCR::read() & TWINT::bit) == 0) { }

        return read_status();
    }
};

}

#endif
