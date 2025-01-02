#ifndef MCU_IO_H
#define MCU_IO_H

#if defined(__AVR_ATmega48A__)
#    include "mcu/dev/atmega328p.h"
namespace io = mcu::atmega48a;

#elif defined(__AVR_ATmega48PA__)
#    include "mcu/dev/atmega48pa.h"
namespace io = mcu::atmega48pa;

#elif defined(__AVR_ATmega88A__)
#    include "mcu/dev/atmega88a.h"
namespace io = mcu::atmega88a;

#elif defined(__AVR_ATmega88PA__)
#    include "mcu/dev/atmega88pa.h"
namespace io = mcu::atmega88pa;

#elif defined(__AVR_ATmega168A__)
#    include "mcu/dev/atmega168a.h"
namespace io = mcu::atmega168a;

#elif defined(__AVR_ATmega168PA__)
#    include "mcu/dev/atmega168pa.h"
namespace io = mcu::atmega168pa;

#elif defined(__AVR_ATmega328__)
#    include "mcu/dev/atmega328.h"
namespace io = mcu::atmega328;

#elif defined(__AVR_ATmega328P__)
#    include "mcu/dev/atmega328p.h"
namespace io = mcu::atmega328p;

#else
#    error "Unsupported MCU"
#endif

#endif
