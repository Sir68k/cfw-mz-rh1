#include "fw/core.h"

#define fw_menu_main_callbacks (void*) 0x000c7c7c
#define fw_menu_spct_callbacks (void*) 0x000c93ec

#define fw_menu_6_rec_settings (void*) 0x000c9554
#define fw_menu_0_edit         (void*) 0x000c80d4
#define fw_menu_5_speed_ctl    (void*) 0x000c93a4
#define fw_menu_b_title_inp    (void*) 0x000c854c
#define fw_menu_8_options      (void*) 0x000c9aa4

#define fw_menu_202_playback_basic  (void*) 0x000c920c
#define fw_menu_302_playback_repeat (void*) 0x000c925c

struct __attribute__((packed)) __attribute__((aligned(4))) t_basic_menu {
    uint32_t id;
    const void* callbacks;
    const void* menu_ref;
    uint32_t type;
};

struct __attribute__((packed)) __attribute__((aligned(4))) t_basic_menu_ref {
    const void* menu_tbl;
    uint32_t type;
    uint32_t x;
    uint32_t y;
};

const struct t_basic_menu cfw_playback_menu[] = {
    {
        .id         = 0x202, // String = Track Mode
        .callbacks  = fw_menu_main_callbacks,
        .menu_ref   = fw_menu_202_playback_basic,
        .type       = 3,
    },
    {
        .id         = 0x302,
        .callbacks  = fw_menu_main_callbacks,
        .menu_ref = fw_menu_302_playback_repeat,
        .type       = 3,
    },
    {
        .id         = 0xFFFFFFFF,
        .callbacks  = (void*) 0,
        .menu_ref   = (void*) 0,
        .type       = 0
    },
};

const struct t_basic_menu_ref cfw_menu_playback = {
    .menu_tbl = &cfw_playback_menu,
    .type = 1, .x = 0, .y = 0
};

const struct t_basic_menu cfw_custom_menu[] = {
    {
        .id         = 0x02,
        .callbacks  = fw_menu_main_callbacks,
        .menu_ref   = &cfw_menu_playback,
        .type       = 1
    },
    {
        .id         = 0x06,
        .callbacks  = fw_menu_main_callbacks,
        .menu_ref   = fw_menu_6_rec_settings,
        .type       = 1
    },
    {
        .id         = 0x00,
        .callbacks  = fw_menu_main_callbacks,
        .menu_ref = fw_menu_0_edit,
        .type       = 1
    },
    {
        .id         = 0x05,
        .callbacks  = fw_menu_spct_callbacks,
        .menu_ref = fw_menu_5_speed_ctl,
        .type       = 3
    },
    {
        .id         = 0x0B,
        .callbacks  = fw_menu_main_callbacks,
        .menu_ref = fw_menu_b_title_inp,
        .type       = 1
    },
    {
        .id         = 0x08,
        .callbacks  = fw_menu_main_callbacks,
        .menu_ref = fw_menu_8_options,
        .type       = 1
    },
    {
        .id         = 0xFFFFFFFF,
        .callbacks  = (void*) 0,
        .menu_ref   = (void*) 0,
        .type       = 0
    },
};