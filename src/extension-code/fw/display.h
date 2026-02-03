#pragma once

#include "core.h"

struct t_disp_buff {
    uint8_t data[96];
    uint8_t aa;
    uint8_t len;
    uint8_t x;
    uint8_t y;
    uint8_t other[16];
};

// Get pointer to a display text buffer
typedef struct t_disp_buff* ftype_get_disp_buffer(uint32_t idx);
static ftype_get_disp_buffer* get_disp_buffer = (ftype_get_disp_buffer*)(THUMB_ADDR(0x00038d6c));

// Render display text buffer
typedef void ftype_disp_buffer(uint32_t idx, uint32_t a, uint32_t b);
static ftype_disp_buffer* disp_buffer = (ftype_disp_buffer*)(THUMB_ADDR(0x000b0d6e));

// Partially reset display text buffer parameters
typedef void ftype_disp_reset_buffer(uint32_t idx);
static ftype_disp_reset_buffer* disp_reset_buffer = (ftype_disp_reset_buffer*)(THUMB_ADDR(0x000aeb1a));