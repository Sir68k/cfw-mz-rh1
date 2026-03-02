#include <stdint.h>
#include "cfw_config.h"
#include "track.h"

extern uint32_t __data_load__;
extern uint32_t __data_start__;
extern uint32_t __data_end__;

extern uint32_t __bss_start__;
extern uint32_t __bss_end__;


static volatile uint32_t* some_init_val = (volatile uint32_t*) 0x02002fb8;


void init()
{
    uint32_t *src = (uint32_t *)&__data_load__;
    uint32_t *dst = (uint32_t *)&__data_start__;
    uint32_t *end = (uint32_t *)&__data_end__;

    while (dst != end) {
        *dst++ = *src++;
    }

    dst = &__bss_start__;

    while (dst != &__bss_end__) {
        *dst++ = 0;
    }

    
    load_config_from_eeprom();
    track_init();

    // we are overwriting the function at 0x0000bfc0, so we need to replica its functionality
    // happily its just a single write
    *some_init_val = 0;
}