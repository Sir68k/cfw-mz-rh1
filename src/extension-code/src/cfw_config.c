#include "fw/sys.h"
#include "fw/eeprom.h"
#include "cfw_config.h"
#include "track.h"

static uint32_t update_block(void* dst, const void* src, uint32_t size) {
    uint32_t* dst_words = (uint32_t*)dst;
    const uint32_t* src_words = (const uint32_t*)src;

    uint32_t changed = 0;
    uint32_t word_count = size / 4;
    
    for (uint32_t i = 0; i < word_count; i++) {
        if (dst_words[i] != src_words[i]) {
            changed = 1;
            dst_words[i] = src_words[i];
        }
    }
    
    return changed;
}

struct cfw_config cfw_eeprom_data;
struct cfw_config cfw_config = {
    .cfw_config_oled_brightness_low = CFW_CONFIG_OLED_BRIGHTNESS_LOW,
    .cfw_config_oled_brightness_high = CFW_CONFIG_OLED_BRIGHTNESS_HIGH,
    .cfw_config_oled_auto_off = CFW_CONFIG_OLED_MODE_NORMAL,
    .cfw_config_oled_usb_mode = CFW_CONFIG_OLED_MODE_NORMAL,
    .v_5 = 0,
    .v_6 = 0,
    .v_7 = 0,
    .v_8 = 0,
    .cfw_tmark_override = 0,
    .v_10 = 0,
    .v_11 = 0,
    .v_12 = 0,
    .track_layout_display = {0, 0}
};

void load_config_from_eeprom() {
    if (fw_load_block_from_eeprom(CFW_BLOCK_ID) == 0)
        return; // invalid data, bad CRC

    if (cfw_eeprom_data.cfw_config_oled_brightness_low != 0)
        cfw_config.cfw_config_oled_brightness_low = cfw_eeprom_data.cfw_config_oled_brightness_low;

    if (cfw_eeprom_data.cfw_config_oled_brightness_high != 0)
        cfw_config.cfw_config_oled_brightness_high = cfw_eeprom_data.cfw_config_oled_brightness_high;

    if (cfw_eeprom_data.cfw_config_oled_auto_off != 0)
        cfw_config.cfw_config_oled_auto_off = cfw_eeprom_data.cfw_config_oled_auto_off;

    if (cfw_eeprom_data.cfw_config_oled_usb_mode != 0)
        cfw_config.cfw_config_oled_usb_mode = cfw_eeprom_data.cfw_config_oled_usb_mode;

    cfw_config.cfw_tmark_override = cfw_eeprom_data.cfw_tmark_override;

    cfw_config.track_layout_display[0] = cfw_eeprom_data.track_layout_display[0];
    cfw_config.track_layout_display[1] = cfw_eeprom_data.track_layout_display[1];
}

uint32_t store_config_to_eeprom() {
    if (update_block(&cfw_eeprom_data, &cfw_config, sizeof(cfw_config)))
        fw_write_block_to_eeprom(CFW_BLOCK_ID);

    return 1;
}

void cfw_eeprom_read(uint32_t src, void* buff, uint32_t len) {
    uint32_t v = disable_interrupts();
    fw_eeprom_read(src, buff, len);
    enable_interrupts(v);
}

void cfw_eeprom_write(uint32_t dst, void* buff, uint32_t len) {
    uint32_t v = disable_interrupts();
    fw_eeprom_write(dst, buff, len);
    enable_interrupts(v);
}