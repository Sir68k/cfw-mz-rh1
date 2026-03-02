#pragma once

#include <stdint.h>

#define member_size(type, member) (sizeof( ((type *)0)->member ))

typedef uint32_t func(void);
typedef uint32_t func1(uint32_t a);
typedef uint32_t func2(uint32_t a, uint32_t b);
typedef uint32_t func3(uint32_t a, uint32_t b, uint32_t c);
typedef uint32_t func4(uint32_t a, uint32_t b, uint32_t c, uint32_t d);

#define VOL32 volatile uint32_t*
#define VOL16 volatile uint16_t*
#define VOL8 volatile uint8_t*

#define THUMB_ADDR(A)  ((A | 1))

#define NIBBLE(value, index) (((uint32_t)(value) & 0xF) << ((index) * 4))
static const char hex_nibble_lookup[16] = {
    '0', '1', '2', '3',
    '4', '5', '6', '7',
    '8', '9', 'A', 'B',
    'C', 'D', 'E', 'F'
};
