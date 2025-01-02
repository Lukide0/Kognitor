#ifndef MCU_DEV_ATMEGA88A_H
#define MCU_DEV_ATMEGA88A_H

#ifndef MCU_IO_H
#    error "Include <mcu/io.h> instead of this file."
#endif

#include "mcu/dev/atmega48a.h"
#include "mcu/Reg.h"
#include <stdint.h>

namespace mcu::atmega88a {

using namespace atmega48a;

using SPMIE  = RB7;
using RWWSB  = RegBit<6, false>;
using SIGRD  = RB5;
using RWWSRE = RB4;
using BLBSET = RB3;
using PGWRT  = RB2;
using PGERS  = RB1;
using SPMEN  = RB0;
using SPMCSR = RWReg<0x36 + 0x20, uint8_t, SPMIE, RWWSB, SIGRD, RWWSRE, BLBSET, PGWRT, PGERS, SPMEN>;

}

#endif
