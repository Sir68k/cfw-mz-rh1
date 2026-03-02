#pragma once

#include "core.h"

typedef uint32_t ftype_strlen(const char *);
static ftype_strlen* strlen = (ftype_strlen*)(THUMB_ADDR(0x000b8070));

typedef void type_memcpy(void *dest, void *src, uint32_t n);
static type_memcpy* memcpy = (type_memcpy*)(THUMB_ADDR(0x000b7f08));

typedef void type_memset(void *dest, uint32_t val, uint32_t n);
static type_memset* memset = (type_memset*)(THUMB_ADDR(0x000b7f90));

typedef uint32_t type_disable_interrupts(void);
static type_disable_interrupts* disable_interrupts = (type_disable_interrupts*)(THUMB_ADDR(0x000009d8));

typedef void type_enable_interrupts(uint32_t);
static type_enable_interrupts* enable_interrupts = (type_enable_interrupts*)(THUMB_ADDR(0x000009ec));