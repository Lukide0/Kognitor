#include "app.h"

#include <microstd/mcu/io.h>
#include <stdint.h>

uint32_t g_seconds = 0;

SIGNAL(INT_TIMER1_COMPA) { g_seconds += 1; }
