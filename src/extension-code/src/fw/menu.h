#pragma once
#include "core.h"

#define fw_menu_main_callbacks (void *)0x000c7c7c
#define fw_menu_spct_callbacks (void *)0x000c93ec

#define fw_menu_6_rec_settings (void *)0x000c9554
#define fw_menu_0_edit (void *)0x000c80d4
#define fw_menu_5_speed_ctl (void *)0x000c93a4
#define fw_menu_b_title_inp (void *)0x000c854c
#define fw_menu_8_options (void *)0x000c9aa4
#define fw_menu_202_playback_basic (void *)0x000c920c
#define fw_menu_302_playback_repeat (void *)0x000c925c

static volatile uint8_t *fw_menu_entry_mode = (volatile uint8_t *)(0x008057a0);
static volatile uint8_t *fw_menu_entry_x = (volatile uint8_t *)(0x008057a1);


struct __attribute__((packed)) __attribute__((aligned(4))) t_basic_menu
{
    const uint32_t id;
    const void *callbacks;
    const void *config;
    const uint32_t type;
};


union Value {
   const void* ptr;
   uint32_t uint;
   int32_t sint;
};


struct __attribute__((packed)) __attribute__((aligned(4))) t_menu_config_entry
{
    const union Value value;
    const uint32_t type;
};

struct __attribute__((packed)) __attribute__((aligned(4))) t_menu_value_entry
{
    const uint32_t id;
    const uint32_t value;
};

struct __attribute__((packed)) __attribute__((aligned(4))) t_menu_value_config
{
    const void* entries;
    const void* ptr_a;
    const void* ptr_b;
    const uint32_t x;
    const uint32_t y;
    const uint32_t len;
};

struct __attribute__((packed)) __attribute__((aligned(4))) t_menu_callback
{
    const void *code;
    const uint32_t id;
};


#define MENU_KEY_UP 0x07
#define MENU_KEY_DOWN 0x09
#define MENU_KEY_PLAY 0x05
#define MENU_KEY_STOP 0x01
#define MENU_KEY_VOL_M 0x14
#define MENU_KEY_VOL_P 0x16
#define MENU_KEY_MENU 0x19

#define MENU_CFG_SUB_MENU 0x01
#define MENU_CFG_VAL_MENU 0x03
#define MENU_CFG_CHANGE_HOOK 0x14
