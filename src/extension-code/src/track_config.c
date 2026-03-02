#include "track_config.h"
#include "cfw_config.h"

uint8_t cfw_config_track_style_mode = 0;

static const uint32_t track_styles[][2] = {
    {TRACK_STYLE_INFO_TIME_TRACK_0,  TRACK_STYLE_INFO_TIME_TRACK_1},
    {TRACK_STYLE_TIME_INFO_TRACK_0,  TRACK_STYLE_TIME_INFO_TRACK_1},
    {TRACK_STYLE_TRACK_TIME_INFO_0,  TRACK_STYLE_TRACK_TIME_INFO_1},
    {TRACK_STYLE_VU_TIME_TRACK_0,    TRACK_STYLE_VU_TIME_TRACK_1  },
    {TRACK_STYLE_CODEC_TIME_TRACK_0, TRACK_STYLE_CODEC_TIME_TRACK_1},
};

uint32_t track_store_style_config() {
    uint32_t styles[2] = {0, 0};

    if (cfw_config_track_style_mode <= (sizeof(track_styles) / sizeof(uint32_t) / 2)) {
        styles[0] = track_styles[cfw_config_track_style_mode - 1][0];
        styles[1] = track_styles[cfw_config_track_style_mode - 1][1]; 
    }

    if (cfw_config.track_layout_display[0] != styles[0] || cfw_config.track_layout_display[1] != styles[1]) {
        cfw_config.track_layout_display[0] = styles[0];
        cfw_config.track_layout_display[1] = styles[1];
        store_config_to_eeprom();
        track_view_state.invalidated = 1;
    }

    return 1;
}