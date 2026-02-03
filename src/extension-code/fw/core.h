#pragma once

#include <stdint.h>

typedef uint32_t func(void);
typedef uint32_t func1(uint32_t a);
typedef uint32_t func2(uint32_t a, uint32_t b);
typedef uint32_t func3(uint32_t a, uint32_t b, uint32_t c);
typedef uint32_t func4(uint32_t a, uint32_t b, uint32_t c, uint32_t d);

#define VOL32 volatile uint32_t*
#define VOL16 volatile uint16_t*
#define VOL8 volatile uint8_t*

#define THUMB_ADDR(A)  ((A | 1))
