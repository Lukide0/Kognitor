#ifndef MCU_DEV_ATMEGA328_H
#define MCU_DEV_ATMEGA328_H

#ifndef MCU_IO_H
#    error "Include <mcu/io.h> instead of this file."
#endif

#include "mcu/dev/atmega168a.h"

namespace mcu {

namespace atmega328 = atmega168a;

}

#endif
