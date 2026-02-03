#pragma once

#include "core.h"

typedef uint32_t ftype_strlen(const char *);
static ftype_strlen* strlen = (ftype_strlen*)(THUMB_ADDR(0x000b8070));

typedef void type_memcpy(void *dest, void *src, uint32_t n);
static type_memcpy* memcpy = (type_memcpy*)(THUMB_ADDR(0x000b7f08));