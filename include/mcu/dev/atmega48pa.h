#ifndef MCU_DEV_ATMEGA48PA_H
#define MCU_DEV_ATMEGA48PA_H

#ifndef MCU_IO_H
#    error "Include <mcu/io.h> instead of this file."
#endif

#include "mcu/dev/atmega48a.h"
#include "mcu/Reg.h"
#include <stdint.h>

namespace mcu::atmega48pa {
using namespace atmega48a;

// NOTE: BODS and BODSE only for picoPower devices
using BODS  = RB6;
using BODSE = RB5;
using MCUCR = RWReg<0x35 + 0x20, uint8_t, BODS, BODSE, PUD, IVSEL, IVCE>;

}

#endif
