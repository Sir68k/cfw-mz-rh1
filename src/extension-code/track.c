#include "track.h"
#include "icons.h"
#include "fw/kana.h"

// Flash free zone: 000E_1000 - 000E_B000 -> A000 (40_960, ~40k) bytes
//  SRAM safe zone: 0080_5e00 - 0080_7800 -> 1A00 (06_656, ~6k) bytes
//  DRAM safe zone: 0200_8900 - 0200_e300 -> 5A00 (23_040, ~23k) bytes

static struct app_state app_state;

// hex nibble lookup for testing
static const char hex_nibble_lookup[16] = {
    '0', '1', '2', '3',
    '4', '5', '6', '7',
    '8', '9', 'A', 'B',
    'C', 'D', 'E', 'F'
};

// capture calls to katakana decode, so that we can manually disable it
volatile int decode_katakana = 1;
int decode_katakana_name(
    char *buffer,
    char *out,
    int offset,
    int len,
    int *chars_written,
    int *pos,
    int y,
    int *x,
    int *partial,
    int *next_offset,
    int *final_state,
    int e) {
        int count = 0;

        if (decode_katakana) {
            return fw_decode_katakana_name(buffer, out, offset, len, chars_written, pos, y, x, partial, next_offset, final_state, e);
        } else {
            // directly copy latin title, without ^
            while (*buffer) {
                if (*buffer != '^') {
                    *out = *buffer;
                    count++;
                    out++;
                }
                buffer++;
            }

            *chars_written += count;
            *pos += count;
            *next_offset += count;
            *final_state = 0;

            *x = count;
            *partial = 0;
            return 0;
        }
}

static uint32_t sanitized_strlen(char *s)
{
    uint32_t len = 0;

    if (!s)
        return 0;

    while (*s) {
        unsigned char c = (unsigned char)*s;
        if (c < 0x20 || c > 0x7E) {
            *s = 'X';  // replace with space
        }
        s++;
        len++;
    }

    return len;
}

static uint32_t himd_get_name(uint32_t track_id, uint32_t type, char* buffer, uint32_t* len) {
    uint32_t partial = 0;
    uint32_t x = 0;
    uint32_t y = 1;
    uint32_t z = 0;

    struct himd_track_query query;
    query.buffer   = buffer;
    query.track_id = track_id;
    query.len      = MAX_NAME_SIZE;
    query.type     = type;
    query.encoding = 0x5;
    query.x        = 1;

    get_track_name_himd(&query, &x, &y, &z, 0);
    uint32_t l = sanitized_strlen(buffer);
    *len += l;
    return l;
}

static uint32_t md_get_name(uint32_t track_id, uint32_t type, char* buffer, uint32_t* len) {
    uint32_t partial = 0;
    uint32_t a=0, b=0, c=0;

    decode_katakana = 0;
    get_track_name(track_id, 0, MAX_NAME_SIZE, 0, buffer, MAX_NAME_SIZE, &partial, &a, &b, &c, type, 0);
    decode_katakana = 1;

    uint32_t l = sanitized_strlen(buffer);
    *len += l;
    return l;
}

static uint32_t sw_get_track_name(uint32_t isHiMD, uint32_t track_id, char* buffer, uint32_t* len) {
    if (isHiMD)
        return himd_get_name(track_id, T_NAME_HIMD_TRACK, buffer, len);
    else
        return md_get_name(track_id, T_NAME_HIMD_TRACK, buffer, len);
}

static uint32_t sw_get_group_name(uint32_t isHiMD, uint32_t track_id, char* buffer, uint32_t* len) {
    if (isHiMD) {
        track_id = get_group_id_for_himd_track(track_id);
        return himd_get_name(track_id, T_NAME_HIMD_GROUP, buffer, len);
    } else if (*current_group != 0)
        return md_get_name(track_id, T_NAME_HIMD_GROUP, buffer, len);
    else
        return 0;
}

static uint32_t sw_get_artist_name(uint32_t isHiMD, uint32_t track_id, char* buffer, uint32_t* len) {
    if (isHiMD)
        return himd_get_name(track_id, T_NAME_HIMD_ARTIST, buffer, len);
    else
        return 0;
}

static uint32_t sw_get_album_name(uint32_t isHiMD, uint32_t track_id, char* buffer, uint32_t* len) {
    if (isHiMD)
        return himd_get_name(track_id, T_NAME_HIMD_ALBUM, buffer, len);
    else
        return 0;
}

static uint32_t sw_get_disc_name(uint32_t isHiMD, uint32_t track_id, char* buffer, uint32_t* len) {
    if (isHiMD)
        return himd_get_name(0, T_NAME_HIMD_DISC, buffer, len);
    else
        return md_get_name(0, T_NAME_HIMD_TRACK, buffer, len);
}


static uint32_t make_track_string(char *buffer, uint32_t idx) {
    // todo: change with memset, based on sizeof(state.data)
    for (uint32_t o=0; o<256; o++) 
        buffer[o] = 0;

    uint32_t i = 0;

    uint32_t track  = *current_track;
    uint32_t isHiMD = *disc_type == DISC_TYPE_HIMD;

    if (idx == 1) {
        int_to_string(track, &buffer[i], 1, 0, 1);
        i = strlen(buffer);
        buffer[i++] = ' ';

        sw_get_track_name(isHiMD, track, &buffer[i], &i);
    } else {
        //buffer[i++] = ' ';

        // Album name
        APPEND_ICON(buffer, i, ICON_ALBUM_L, ICON_ALBUM_R);
        uint32_t l = sw_get_album_name(isHiMD, track, &buffer[i], &i);
        REMOVE_ICON_IF_EMPTY(l, buffer, i);

        // Artist name
        APPEND_ICON(buffer, i, ICON_ARTIST_L, ICON_ARTIST_R);
        l = sw_get_artist_name(isHiMD, track, &buffer[i], &i);
        REMOVE_ICON_IF_EMPTY(l, buffer, i);

        // Group name
        APPEND_ICON(buffer, i, ICON_GROUP_L, ICON_GROUP_R);
        l = sw_get_group_name(isHiMD, track, &buffer[i], &i);
        REMOVE_ICON_IF_EMPTY(l, buffer, i);

        // Disc name
        APPEND_ICON(buffer, i, ICON_DISC_L, ICON_DISC_R);
        l = sw_get_disc_name(isHiMD, track, &buffer[i], &i);
        REMOVE_ICON_IF_EMPTY(l, buffer, i);

        for (uint32_t o=0; o<10; o++) 
            buffer[i+o] = 0;
        
        // Timer
        int_to_string(app_state.time_m, &buffer[i], 1, 0, 1);
        i = strlen(buffer);
        buffer[i++] = ':';
        int_to_string(app_state.time_s, &buffer[i], 1, 0, 1);
        i = strlen(buffer);
    }

    for (uint32_t j = 0; j < SCROLL_GAP; j++)
        buffer[i++] = ' ';

    // if at any point a null char was introduced, replace it by a space
    for (uint32_t o=0; o<i; o++) 
        if (buffer[o] == 0)
            buffer[o] = ' ';

    // put final null char
    buffer[i] = 0;
    
    return i;
}

static void get_scroll_frame(char *display, struct scroll_state *state)
{
    char *src = &state->buffer[state->scroll_index];
    uint32_t remaining = state->text_length - state->scroll_index;

    if (remaining >= state->visible_length) {
        memcpy(display, src, state->visible_length);
    } else {
        // wrap manually
        memcpy(display, src, remaining);
        if (state->text_length - SCROLL_GAP > state->visible_length) 
            memcpy(display + remaining, state->buffer, state->visible_length - remaining);
    }

    display[state->visible_length] = '\0';

    if (++(state->speed) < SCROLL_SPEED) 
        return;

    state->speed = 0;

    if ((state->scroll_index == 0 || remaining == SCROLL_GAP + state->visible_length) && state->pause < START_PAUSE) {
        // pause at: start of string aligned at to the left, end of string aligned to the right
        state->pause++;
    } else {
        state->pause = 0;
        state->scroll_index++;
        if (state->scroll_index >= state->text_length)
            state->scroll_index = 0;
    }

    if (state->text_length - SCROLL_GAP <= state->visible_length) {
        state->scroll_index = 0;
    }
}

static void update_state() {
    // If we properly load .data and .bss we can remove the need for the magic value
    if (app_state.magic != MAGIC_VALUE) {
        app_state.magic = MAGIC_VALUE;
        app_state.track_id = -1;
        app_state.time_m = -1;
        app_state.time_s = -1;
        
        app_state.states[0].visible_length = (DISPLAY_WIDTH - BATTERY_INDICATOR_SIZE - 1);
        app_state.states[1].visible_length = DISPLAY_WIDTH;
    }

    uint32_t h,m,s;
    get_track_progress(1, 0, &h, &m, &s);
    uint32_t updated = 0;
    
    if (*current_track != app_state.track_id) {
        updated = 1;
        app_state.track_id = *current_track;

        app_state.states[0].scroll_index = 0;
        app_state.states[0].pause = 0;
        app_state.states[0].speed = 0;

        app_state.states[1].scroll_index = 0;
        app_state.states[1].pause = 0;
        app_state.states[1].speed = 0;
    }

    if (s != app_state.time_s) {
        updated = 1;
        app_state.time_s = s;
        app_state.time_m = m + h * 60;
    }

    if (updated) {
        app_state.states[0].text_length = make_track_string(app_state.states[0].buffer, 0);
        app_state.states[1].text_length = make_track_string(app_state.states[1].buffer, 1);
    }
}

uint32_t track_title_renderer(uint32_t p1, uint32_t idx) {
    struct t_disp_buff* volatile oled_buff = (struct t_disp_buff*) get_disp_buffer(idx);
    disp_reset_buffer(idx);

    update_state();

    if (app_state.track_id > 0) {
        oled_buff->data[0] = ' ';
        get_scroll_frame(idx ? oled_buff->data : &oled_buff->data[1], &app_state.states[idx]);
        oled_buff->len = DISPLAY_WIDTH;
    } else {
        memcpy(oled_buff->data, UI_TEXT_LOADING, sizeof(UI_TEXT_LOADING));
        oled_buff->len = sizeof(UI_TEXT_LOADING);
    }

    oled_buff->x = 0x00;
    disp_buffer(idx, 0, 0);

    return 0;
}
