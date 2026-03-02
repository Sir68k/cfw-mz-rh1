#pragma once

#include <stdint.h>

#include "fw/display.h"
#include "fw/sys.h"
#include "fw/tracks.h"

#define DISPLAY_WIDTH 18
#define MAX_NAME_SIZE 48

#define BATTERY_INDICATOR_SIZE 3

#define SCROLL_GAP    6
#define START_PAUSE   20
#define SCROLL_SPEED  3

#define UI_TEXT_LOADING " Loading"

#define TRACK_ITEM_NONE        0x0
#define TRACK_ITEM_TRACK_NAME  0x1
#define TRACK_ITEM_GROUP_NAME  0x2
#define TRACK_ITEM_DISC_NAME   0x3
#define TRACK_ITEM_ARTIST_NAME 0x4
#define TRACK_ITEM_ALBUM_NAME  0x5
#define TRACK_ITEM_TIME        0x6

#define TRACK_ITEM_MAX         TRACK_ITEM_TIME
#define TRACK_ITEM_MASK        0x00FFFFFF
#define TRACK_ITEM_OMASK       0xFF000000
#define OLED_HANDLER_MARKER    0xFFFF0000
#define OLED_HANDLER_MASK      0x0000FFFF

#define DEFAULT_DISPLAY_0_LAYOUT (NIBBLE(TRACK_ITEM_ALBUM_NAME,  0) | \
                                  NIBBLE(TRACK_ITEM_ARTIST_NAME, 1) | \
                                  NIBBLE(TRACK_ITEM_GROUP_NAME,  2) | \
                                  NIBBLE(TRACK_ITEM_DISC_NAME,   3) | \
                                  NIBBLE(TRACK_ITEM_TIME,        4))

#define DEFAULT_DISPLAY_1_LAYOUT (NIBBLE(TRACK_ITEM_TRACK_NAME,  0))


struct scroll_state {
    uint16_t text_length;
    uint16_t visible_length;
    uint16_t scroll_index;
    uint8_t pause;
    uint8_t speed;
    char buffer[300];
};

struct track_view_state {
    //uint32_t magic;
    int32_t track_id;
    int32_t time_m;
    int32_t time_s;
    struct scroll_state states[2];
    uint8_t last_playback_state;
    uint8_t invalidated;
};

#define REMOVE_ICON_IF_EMPTY(len, buf, idx)       \
    do {                                          \
        if ((len) == 0) {                         \
            (idx) -= 3;                           \
        }                                         \
    } while (0)
    
void track_init();
extern struct track_view_state track_view_state;
