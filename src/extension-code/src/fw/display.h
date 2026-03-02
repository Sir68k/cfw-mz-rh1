#pragma once

#include "core.h"

#define OLED_HANDLER_IDX_MINI_VU 22
#define OLED_HANDLER_IDX_CODEC 4

#define DISP_TEXT_NORMAL 0
#define DISP_TEXT_BLINK  4

struct t_disp_buff {
    uint8_t data[96];  // 0 - 95
    uint8_t aa;        // 96, 0x60 
    uint8_t len;       // 97, 0x61
    uint8_t encoding;  // 98, 0x62
    uint8_t y;         // 99, 0x63
    uint8_t b;        // 100, 0x64
    uint8_t b_1;
    uint8_t b_2;
    uint8_t b_3;
    uint32_t c;
    uint8_t d[6];
    uint8_t a_0;
    uint8_t a;
} __attribute__((packed));

struct t_disp_buff_str {
    uint32_t len;
    uint32_t x;
    char data[20];
} __attribute__((packed));


static volatile struct t_disp_buff_str *g_disp_buff_str_tbl = (volatile struct t_disp_buff_str *)(0x00805768);
static volatile uint8_t *device_power_mode_b = (volatile uint8_t *)(0x008005b0);

static const func2** fw_oled_display_handlers = (const func2**)(0x000cbcc0);

// Get pointer to a display text buffer
typedef struct t_disp_buff* ftype_get_disp_buffer(uint32_t idx);
static ftype_get_disp_buffer* get_disp_buffer = (ftype_get_disp_buffer*)(THUMB_ADDR(0x00038d6c));

// Render display text buffer
typedef void ftype_disp_buffer(uint32_t idx, uint32_t a, uint32_t b);
static ftype_disp_buffer* disp_buffer = (ftype_disp_buffer*)(THUMB_ADDR(0x000b0d6e));

// Partially reset display text buffer parameters
typedef void ftype_disp_reset_buffer(uint32_t idx);
static ftype_disp_reset_buffer* disp_reset_buffer = (ftype_disp_reset_buffer*)(THUMB_ADDR(0x000aeb1a));

typedef void ftype_cpy_disp_buff_1(uint32_t idx, uint32_t* res);
static ftype_cpy_disp_buff_1* cpy_disp_buff_1 = (ftype_cpy_disp_buff_1*)(THUMB_ADDR(0x000aebd0));

typedef void ftype_cpy_disp_buff_2(uint32_t idx, uint32_t x);
static ftype_cpy_disp_buff_2* cpy_disp_buff_2 = (ftype_cpy_disp_buff_2*)(THUMB_ADDR(0x000aeb98));

typedef void ftype_prepare_text_for_scroll(uint32_t idx);
static ftype_prepare_text_for_scroll* prepare_text_for_scroll = (ftype_prepare_text_for_scroll*)(THUMB_ADDR(0x000b0da8));

typedef void ftype_disp_start_scroll(uint32_t p1,uint32_t p2);
static ftype_disp_start_scroll* disp_start_scroll = (ftype_disp_start_scroll*)(THUMB_ADDR(0x000b0dc2));

typedef uint32_t ftype_queue_disp_cmd(void* buff, uint32_t len);
static ftype_queue_disp_cmd* queue_disp_cmd = (ftype_queue_disp_cmd*)(THUMB_ADDR(0x000b6648));

typedef uint32_t ftype_disp_enqueue_cmd(uint32_t id, void* arg, uint32_t x);
static ftype_disp_enqueue_cmd* fw_disp_enqueue_cmd = (ftype_disp_enqueue_cmd*)(THUMB_ADDR(0x0003d424));