#ifndef MCU_INTERRUPT_H
#define MCU_INTERRUPT_H

#define VECTOR(N) __vector_##N

#define SIGNAL(N) _SIGNAL(VECTOR(N))
#define INTERRUPT(N) _INTERRUPT(VECTOR(N))

#define _SIGNAL(VEC)                                                 \
    extern "C" void VEC(void) __attribute__((__signal__, __used__)); \
    void VEC(void)

#define _INTERRUPT(VEC)                                                 \
    extern "C" void VEC(void) __attribute__((__interrupt__, __used__)); \
    void VEC(void)

#define enable_interrupts() __asm__ __volatile__("sei")
#define disable_interrupts() __asm__ __volatile__("cli")

#endif
