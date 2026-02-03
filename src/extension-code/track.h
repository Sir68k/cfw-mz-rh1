#include <stdint.h>

#include "fw/display.h"
#include "fw/sys.h"
#include "fw/tracks.h"

#define DISPLAY_WIDTH 18
#define MAX_NAME_SIZE 60

#define BATTERY_INDICATOR_SIZE 3

#define SCROLL_GAP    6
#define START_PAUSE   20
#define SCROLL_SPEED  3

#define MAGIC_VALUE 0xDEADBEEF

#define UI_TEXT_LOADING " Loading"

struct scroll_state {
    uint8_t text_length;
    uint8_t visible_length;
    uint8_t scroll_index;
    uint8_t pause;
    uint8_t speed;
    char buffer[256];
};

struct app_state {
    uint32_t magic;
    int32_t track_id;
    int32_t time_m;
    int32_t time_s;
    struct scroll_state states[2];
};

#define REMOVE_ICON_IF_EMPTY(len, buf, idx)       \
    do {                                          \
        if ((len) == 0) {                         \
            (idx) -= 3;                           \
        } else {                                  \
            (buf)[(idx)++] = ' ';                 \
            (buf)[(idx)++] = ' ';                 \
        }                                         \
    } while (0)
    