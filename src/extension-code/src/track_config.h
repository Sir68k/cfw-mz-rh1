#pragma once
#include "fw/core.h"
#include "track.h"

#define TRACK_STYLE_INFO_TIME_TRACK_0 DEFAULT_DISPLAY_0_LAYOUT
#define TRACK_STYLE_INFO_TIME_TRACK_1 DEFAULT_DISPLAY_1_LAYOUT

#define TRACK_STYLE_TIME_INFO_TRACK_0 (NIBBLE(TRACK_ITEM_ALBUM_NAME,  1) | \
                                       NIBBLE(TRACK_ITEM_ARTIST_NAME, 2) | \
                                       NIBBLE(TRACK_ITEM_GROUP_NAME,  3) | \
                                       NIBBLE(TRACK_ITEM_DISC_NAME,   4) | \
                                       NIBBLE(TRACK_ITEM_TIME,        0))
#define TRACK_STYLE_TIME_INFO_TRACK_1 (NIBBLE(TRACK_ITEM_TRACK_NAME,  0))

#define TRACK_STYLE_TRACK_TIME_INFO_0  (NIBBLE(TRACK_ITEM_TRACK_NAME,  0) | \
                                        NIBBLE(TRACK_ITEM_TIME,        1))
#define TRACK_STYLE_TRACK_TIME_INFO_1  (NIBBLE(TRACK_ITEM_ALBUM_NAME,  0) | \
                                        NIBBLE(TRACK_ITEM_ARTIST_NAME, 1) | \
                                        NIBBLE(TRACK_ITEM_GROUP_NAME,  2) | \
                                        NIBBLE(TRACK_ITEM_DISC_NAME,   3))

#define TRACK_STYLE_VU_TIME_TRACK_0   (OLED_HANDLER_MARKER | OLED_HANDLER_IDX_MINI_VU)
#define TRACK_STYLE_VU_TIME_TRACK_1   (NIBBLE(TRACK_ITEM_TRACK_NAME,  0) | \
                                       NIBBLE(TRACK_ITEM_TIME,        1))  

#define TRACK_STYLE_CODEC_TIME_TRACK_0 (OLED_HANDLER_MARKER | OLED_HANDLER_IDX_CODEC)
#define TRACK_STYLE_CODEC_TIME_TRACK_1 (NIBBLE(TRACK_ITEM_TRACK_NAME,  0) | \
                                       NIBBLE(TRACK_ITEM_TIME,         1))  



extern uint8_t cfw_config_track_style_mode;
uint32_t track_store_style_config();