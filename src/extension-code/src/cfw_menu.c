#include "fw/core.h"
#include "fw/sys.h"
#include "fw/display.h"
#include "fw/menu.h"

#include "menu_resources.h"
#include "menu_macros.h"

#include "cfw_config.h"
#include "track_config.h"
/*
#define MENU_TEXT_ABOUT "By Sir68k"

void make_about_string(struct t_disp_buff *buff)
{
    memcpy(buff, MENU_TEXT_ABOUT, sizeof(MENU_TEXT_ABOUT));
    buff->data[sizeof(MENU_TEXT_ABOUT)] = 0;

    buff->len = sizeof(MENU_TEXT_ABOUT);
    buff->encoding = 0;
}

void cfw_menu_about(uint32_t disp, uint32_t idx)
{

    volatile struct t_disp_buff *buff;

    uint32_t s_mode_a;
    uint32_t s_mode_b;
    uint32_t x;
    uint8_t g_menu_entry_mode;
    uint8_t g_menu_entry_x;

    x = 0;
    g_menu_entry_mode = *fw_menu_entry_mode;
    g_menu_entry_x = *fw_menu_entry_x;
    buff = get_disp_buffer(idx);

    if (g_menu_entry_mode == 1)
    {
        if ((g_menu_entry_x & 1) == 0)
            return;

        disp_reset_buffer(idx);
        make_about_string(buff);

        disp_buffer(idx, buff->b, buff->c);
    }
    else
    {
        if ((g_menu_entry_mode != 2) && (g_menu_entry_mode != 3))
            return;

        if ((g_menu_entry_x & 1) == 0)
        {
            x = g_disp_buff_str_tbl[idx].x;
        }
        else
        {
            disp_reset_buffer(idx);
            cpy_disp_buff_1(idx, &x);
            disp_buffer(idx, 0, 0);
        }

        disp_reset_buffer(idx);
        make_about_string(buff);
        prepare_text_for_scroll(idx);

        s_mode_a = 1;
        if (g_menu_entry_mode != 2)
            s_mode_a = 2;

        s_mode_b = 2;
        if ((x >> 3 & 1) == 0)
            s_mode_b = 0;

        disp_start_scroll(s_mode_a, s_mode_b);
    }

    cpy_disp_buff_2(idx, 0);
}

#define menu_cb_mc_a (const void*) THUMB_ADDR(0x000436d0)
#define menu_cb_mc_b (const void*) THUMB_ADDR(0x00043746)

static func1 * menu_callback_d = (func1*)THUMB_ADDR(0x000aa478);

uint32_t menu_register_event(uint32_t value) {

    if (value == MENU_KEY_STOP || value == MENU_KEY_MENU) {
        return menu_callback_d(MENU_KEY_STOP);
    }

    return 1;
}

static const struct t_menu_callback cfw_menu_callbacks[] = {
    { menu_register_event, 0x43 },
    { menu_register_event, 0x41 },

    { menu_register_event, 0x05 },
    { menu_register_event, 0x07 },
    { menu_register_event, 0x409 },

    { menu_register_event, 0x21 },
    { menu_register_event, 0x01 },
    { menu_register_event, 0x414 },
    { menu_register_event, 0x415 },
    { menu_register_event, 0x416 },
    { menu_register_event, 0x417 },
    { menu_register_event, 0x419 },

    { menu_cb_mc_b, 0x1C },
    { menu_cb_mc_b, 0x49 },
    { menu_cb_mc_b, 0x445 },
    { menu_cb_mc_b, 0x446 },
    { menu_cb_mc_b, 0x10083C },
    { menu_cb_mc_b, 0x83 },

    { (void *)0x00, 0x00 },
};

static const uint32_t config[] = {
    0x02000004,
    0x2D,

    0x11208,
    0x2B,

    0x00,
    0x2C,

    0x00,
    0x00,
};
*/



MENU_VALUE_START(oled_auto)
MENU_VALUE_ENTRY(MENU_RESOURCE_ID(OLED_NORMAL),    CFW_CONFIG_OLED_MODE_NORMAL)
MENU_VALUE_ENTRY(MENU_RESOURCE_ID(OLED_DIM_ONLY),  CFW_CONFIG_OLED_MODE_DIM_ONLY)
MENU_VALUE_ENTRY(MENU_RESOURCE_ID(OLED_FULLY_OFF), CFW_CONFIG_OLED_MODE_FULLY_OFF)
MENU_VALUE_END(oled_auto, cfw_config.cfw_config_oled_auto_off, store_config_to_eeprom)

MENU_VALUE_START(oled_usb)
MENU_VALUE_ENTRY(MENU_RESOURCE_ID(OLED_U_NORMAL),    CFW_CONFIG_OLED_MODE_NORMAL)
MENU_VALUE_ENTRY(MENU_RESOURCE_ID(OLED_U_FULLY_OFF), CFW_CONFIG_OLED_MODE_FULLY_OFF)
MENU_VALUE_END(oled_usb, cfw_config.cfw_config_oled_usb_mode, store_config_to_eeprom)

MENU_VALUE_START(oled_brightness_l)
MENU_VALUE_ENTRY(MENU_RESOURCE_ID(OLED_BR_L_L),  CFW_CONFIG_OLED_BRIGHTNESS_LOW)
MENU_VALUE_ENTRY(MENU_RESOURCE_ID(OLED_BR_L_M),  CFW_CONFIG_OLED_BRIGHTNESS_MEDIUM)
MENU_VALUE_ENTRY(MENU_RESOURCE_ID(OLED_BR_L_H),  CFW_CONFIG_OLED_BRIGHTNESS_HIGH)
MENU_VALUE_END(oled_brightness_l, cfw_config.cfw_config_oled_brightness_low, store_config_to_eeprom)

MENU_VALUE_START(oled_brightness_h)
MENU_VALUE_ENTRY(MENU_RESOURCE_ID(OLED_BR_H_L),  CFW_CONFIG_OLED_BRIGHTNESS_LOW)
MENU_VALUE_ENTRY(MENU_RESOURCE_ID(OLED_BR_H_M),  CFW_CONFIG_OLED_BRIGHTNESS_MEDIUM)
MENU_VALUE_ENTRY(MENU_RESOURCE_ID(OLED_BR_H_H),  CFW_CONFIG_OLED_BRIGHTNESS_HIGH)
MENU_VALUE_END(oled_brightness_h, cfw_config.cfw_config_oled_brightness_high, store_config_to_eeprom)

MENU_VALUE_START(track_style)
MENU_VALUE_ENTRY(MENU_RESOURCE_ID(TRACK_STYLE1),  1)
MENU_VALUE_ENTRY(MENU_RESOURCE_ID(TRACK_STYLE2),  2)
MENU_VALUE_ENTRY(MENU_RESOURCE_ID(TRACK_STYLE3),  3)
MENU_VALUE_ENTRY(MENU_RESOURCE_ID(TRACK_STYLE4),  4)
MENU_VALUE_ENTRY(MENU_RESOURCE_ID(TRACK_STYLE5),  5)
MENU_VALUE_END(track_style, cfw_config_track_style_mode, track_store_style_config)

MENU_VALUE_START(tmark_override)
MENU_VALUE_ENTRY(MENU_RESOURCE_ID(TMARK_NORM),  0)
MENU_VALUE_ENTRY(MENU_RESOURCE_ID(TMARK_SKIP),  1)
MENU_VALUE_END(tmark_override, cfw_config.cfw_tmark_override, store_config_to_eeprom)

MENU_START(oled)
MENU_ITEM_VALUE(MENU_RESOURCE_ID(OLED_AUTO_OFF), oled_auto)
MENU_ITEM_VALUE(MENU_RESOURCE_ID(OLED_USB), oled_usb)
MENU_ITEM_VALUE(MENU_RESOURCE_ID(OLED_BR_LOW), oled_brightness_l)
MENU_ITEM_VALUE(MENU_RESOURCE_ID(OLED_BR_HIGH), oled_brightness_h)
MENU_END(oled)

uint8_t cfw_config_about;
MENU_VALUE_START(about)
MENU_VALUE_ENTRY(MENU_RESOURCE_ID(ABOUT_A), 0x00)
MENU_VALUE_ENTRY(MENU_RESOURCE_ID(ABOUT_B), 0x01)
MENU_VALUE_ENTRY(MENU_RESOURCE_ID(ABOUT_C), 0x02)
MENU_VALUE_ENTRY(MENU_RESOURCE_ID(ABOUT_D), 0x03)
MENU_VALUE_END_SIMPLE(about, cfw_config_about)

MENU_START(cfw)
MENU_ITEM_SUBMENU(MENU_RESOURCE_ID(OLED), oled)
MENU_ITEM_VALUE(MENU_RESOURCE_ID(TRACK_STYLE), track_style)
MENU_ITEM_VALUE(MENU_RESOURCE_ID(TMARK), tmark_override)
MENU_ITEM_VALUE(MENU_RESOURCE_ID(ABOUT), about)
MENU_END(cfw)