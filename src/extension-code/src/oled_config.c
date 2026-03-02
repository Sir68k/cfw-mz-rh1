#include "fw/core.h"
#include "fw/display.h"
#include "cfw_config.h"

uint8_t cfw_disp_buff_0x30[2];

static uint8_t oled_config_to_brightness(uint8_t val)
{
    switch (val)
    {
    case CFW_CONFIG_OLED_BRIGHTNESS_VERY_LOW:
        return 0x01;
    case CFW_CONFIG_OLED_BRIGHTNESS_LOW:
        return 0x04;
    case CFW_CONFIG_OLED_BRIGHTNESS_MEDIUM:
        return 0x18;
    default:
    case CFW_CONFIG_OLED_BRIGHTNESS_HIGH:
        return 0x1b;
    }
}

uint32_t cfw_disp_cmd_0x30_set_brightness(uint8_t *p)
{
    cfw_disp_buff_0x30[0] = 0x30;
    if ((*p >> 3 & 1) == 0)
        cfw_disp_buff_0x30[1] = oled_config_to_brightness(cfw_config.cfw_config_oled_brightness_high);
    else
        cfw_disp_buff_0x30[1] = oled_config_to_brightness(cfw_config.cfw_config_oled_brightness_low);

    return queue_disp_cmd(cfw_disp_buff_0x30, 2);
}


uint8_t prev_mode = 0;

__attribute__((naked))
void cfw_enable_blackout()
{
    asm volatile(
        "push {r0-r4, lr}\n" ::: "memory");

    // base struct ptr in r4, store it before gcc messes with it
    register void *conf asm("r4");
    asm volatile("" : "=r"(conf));

    uint8_t current = *((uint8_t *)conf + 0xC);
    uint8_t new = 2;
    
    if (current == 3 || current == 4)
        prev_mode = current;
    
    if (current == 3 && cfw_config.cfw_config_oled_auto_off == CFW_CONFIG_OLED_MODE_DIM_ONLY)
        new = 3;

    if (current == 4 && cfw_config.cfw_config_oled_usb_mode == CFW_CONFIG_OLED_MODE_NORMAL)
        new = 4;

    *((uint8_t *)conf + 0xC) = new;

    asm volatile(
        "pop {r0-r4, pc}\n" ::: "memory");
}

static uint32_t empty = 0;
void cfw_bat_disp_enqueue_cmd(uint32_t cmd, void* buff, uint32_t n) {
    if ( device_power_mode_b[0] == 2 && 
        ((prev_mode == 3 && cfw_config.cfw_config_oled_auto_off == CFW_CONFIG_OLED_MODE_FULLY_OFF && device_power_mode_b[0] == 2) ||
         (prev_mode == 4 && cfw_config.cfw_config_oled_usb_mode == CFW_CONFIG_OLED_MODE_FULLY_OFF && device_power_mode_b[0] == 2) ))
        buff = &empty;
    
    fw_disp_enqueue_cmd(cmd, buff, n);
}