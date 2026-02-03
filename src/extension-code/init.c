#include <stdint.h>

extern uint32_t __data_load__;
extern uint32_t __data_start__;
extern uint32_t __data_end__;

extern uint32_t __bss_start__;
extern uint32_t __bss_end__;

// This code loads the .data and .bss sections of our firmware extension
// It needs to be called early on when the firmware starts
// A good patching area in the firmware is the tron loading functions where there is a lot of dead code,
// enough space where a jump to our init can be added


static volatile uint32_t* some_init_val = (volatile uint32_t*) 0x02002fb8;


void init()
{
    uint32_t *src = (uint32_t *)&__data_load__;
    uint32_t *dst = (uint32_t *)&__data_start__;
    uint32_t *end = (uint32_t *)&__data_end__;

    while (dst < end) {
        *dst++ = *src++;
    }

    dst = &__bss_start__;

    while (dst < &__bss_end__) {
        *dst++ = 0;
    }

    // we are overwriting the function at 0x0000bfc0, so we need to replica its functionality
    // happily its just a single write
    *some_init_val = 0;
}
