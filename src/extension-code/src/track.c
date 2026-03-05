#include "track.h"
#include "icons.h"
#include "fw/kana.h"
#include "cfw_config.h"

struct track_view_state track_view_state;

// Capture calls to katakana decode
// This allows us to disable it when fetching titles that we want to display
// on the OLED, as the display controller only supports latin characters
// patch for hook is in patches.toml

volatile int decode_katakana = 1;
int hook_decode_katakana_name(
    char *buffer,
    char *out,
    int offset,
    int len,
    int *chars_written,
    int *pos,
    int max,
    int *x,
    int *partial,
    int *next_offset,
    int *final_state,
    int e) {
        int count = 0;
        char *end = &buffer[max+1]; // this is safe; max has one subtracted from it

        if (decode_katakana) {
            return fw_decode_katakana_name(buffer, out, offset, len, chars_written, pos, max, x, partial, next_offset, final_state, e);
        } else {
            // re-enable automatically
            decode_katakana = 1;

            // directly copy latin title, without ^
            while (*buffer && buffer <= end) {
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


// strlen which also replaces unprintable characters
static uint32_t sanitized_strlen(unsigned char *s, uint32_t maxlen) {
    uint32_t len = 0;

    if (!s)
        return 0;

    while (*s && len < maxlen) {
        unsigned char c = *s;
        // replace unprintable chars
        if (c < 0x20 || c > 0x7E) {
            *s = '_';
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
    query.encoding = 5;
    query.x        = 1;

    get_track_name_himd(&query, &x, &y, &z, 0);
    uint32_t l = sanitized_strlen(buffer, MAX_NAME_SIZE);
    *len += l;
    return l;
}

static uint32_t md_get_name(uint32_t track_id, uint32_t type, char* buffer, uint32_t* len) {
    uint32_t partial = 0;
    uint32_t a=0, b=0, c=0;

    decode_katakana = 0;
    get_track_name(track_id, 0, MAX_NAME_SIZE, 0, buffer, MAX_NAME_SIZE, &partial, &a, &b, &c, type, 0);

    uint32_t l = sanitized_strlen(buffer, MAX_NAME_SIZE);
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


uint32_t int_len(uint32_t n) {
    if (n < 100) return 2;
    else if (n < 1000) return 3;
    else return 4;
}

static uint32_t make_track_string(char *buffer, uint32_t idx) {
    memset(buffer, 0, member_size(struct scroll_state, buffer));

    uint32_t i = 0;
    uint32_t track  = *current_track;
    uint32_t isHiMD = *disc_type == DISC_TYPE_HIMD;
    uint32_t layout_config = cfw_config.track_layout_display[idx] & TRACK_ITEM_MASK;
    uint8_t item_type;
    uint32_t item_slot;
    uint32_t l;
    
    if (layout_config == 0) {
        layout_config = (idx == 0) ? DEFAULT_DISPLAY_0_LAYOUT : DEFAULT_DISPLAY_1_LAYOUT;
    }
    
    while (layout_config > 0) {
        item_type = layout_config & 0xF;
        layout_config = layout_config >> 4;
        l = 0;
        
        if (item_type == TRACK_ITEM_TRACK_NAME) {
            int_to_string(track, &buffer[i], 1, 0, 1);
            i += int_len(track);
            buffer[i++] = ' ';
            l = sw_get_track_name(isHiMD, track, &buffer[i], &i);
        }
        
        if (item_type == TRACK_ITEM_GROUP_NAME) {
            APPEND_ICON(buffer, i, ICON_GROUP_L, ICON_GROUP_R);
            l = sw_get_group_name(isHiMD, track, &buffer[i], &i);
            REMOVE_ICON_IF_EMPTY(l, buffer, i);
        }
        
        if (item_type == TRACK_ITEM_DISC_NAME) {
            APPEND_ICON(buffer, i, ICON_DISC_L, ICON_DISC_R);
            l = sw_get_disc_name(isHiMD, track, &buffer[i], &i);
            REMOVE_ICON_IF_EMPTY(l, buffer, i);
        }
        
        if (item_type == TRACK_ITEM_ARTIST_NAME) {
            APPEND_ICON(buffer, i, ICON_ARTIST_L, ICON_ARTIST_R);
            l = sw_get_artist_name(isHiMD, track, &buffer[i], &i);
            REMOVE_ICON_IF_EMPTY(l, buffer, i);
        }
        
        if (item_type == TRACK_ITEM_ALBUM_NAME) {
            APPEND_ICON(buffer, i, ICON_ALBUM_L, ICON_ALBUM_R);
            l = sw_get_album_name(isHiMD, track, &buffer[i], &i);
            REMOVE_ICON_IF_EMPTY(l, buffer, i);
        }
        
        if (item_type == TRACK_ITEM_TIME) {
            int_to_string(track_view_state.time_m, &buffer[i], 1, 0, 1);
            i += int_len(track_view_state.time_m);
            buffer[i++] = ':';
            int_to_string(track_view_state.time_s, &buffer[i], 1, 0, 1);
            i += 2;
            l = 1;
        }

        if (layout_config != 0 && l != 0) {
            buffer[i++] = ' ';
            if (item_type != TRACK_ITEM_TIME)
                buffer[i++] = ' ';
        }
    }

    for (item_slot = 0; item_slot < SCROLL_GAP; item_slot++)
        buffer[i++] = ' ';

    // put final null char
    buffer[i] = '\0';
    
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

// The display thread calls this function regularly; we use it to
// invalidate our track cache in case that the TOC is still loading
uint32_t cfw_get_operation_state() {
    uint32_t state = *operation_state;

    if (state == 1) { // TOC is being read
        track_view_state.invalidated = 1;
    }

    return state;
}

static void track_update_state() {
    uint32_t updated = 0;
    uint32_t h,m,s;

    get_track_progress(1, 0, &h, &m, &s);

    if (track_view_state.invalidated) {
        track_view_state.track_id = 0;
        track_view_state.invalidated = 0;

        // we will only continue if we haven't had two invalidation requests in a row
        // this allows us to avoid regenerating the displays strings over and over
        return;
    }
    
    if (*current_track != track_view_state.track_id) {
        updated = 1;
        track_view_state.track_id = *current_track;

        track_view_state.states[0].scroll_index = 0;
        track_view_state.states[0].pause = 0;
        track_view_state.states[0].speed = 0;

        track_view_state.states[1].scroll_index = 0;
        track_view_state.states[1].pause = 0;
        track_view_state.states[1].speed = 0;

        track_view_state.invalidated = 0;
    }

    if (s != track_view_state.time_s) {
        updated = 1;
        track_view_state.time_s = s;
        track_view_state.time_m = m + h * 60;
    }

    if (updated) {
        track_view_state.states[0].text_length = make_track_string(track_view_state.states[0].buffer, 0);
        track_view_state.states[1].text_length = make_track_string(track_view_state.states[1].buffer, 1);
    }
}


uint32_t track_title_renderer(uint32_t p1, uint32_t idx) {
    uint32_t layout_config = cfw_config.track_layout_display[idx];
    if ((layout_config & OLED_HANDLER_MARKER) == OLED_HANDLER_MARKER) {
        return fw_oled_display_handlers[layout_config & OLED_HANDLER_MASK](p1, idx);
    }

    struct t_disp_buff* volatile oled_buff = (struct t_disp_buff*) get_disp_buffer(idx);
    disp_reset_buffer(idx);

    uint32_t playback_state = get_playback_state();
    if (playback_state != track_view_state.last_playback_state) {
        track_view_state.last_playback_state = playback_state;
        track_view_state.invalidated = 1; 
    }

    track_update_state();

    if (track_view_state.track_id > 0) {
        oled_buff->data[0] = ' ';
        get_scroll_frame(idx ? oled_buff->data : &oled_buff->data[1], &track_view_state.states[idx]);
        oled_buff->len = DISPLAY_WIDTH;
    } else {
        memcpy(oled_buff->data, UI_TEXT_LOADING, sizeof(UI_TEXT_LOADING));
        oled_buff->len = sizeof(UI_TEXT_LOADING);
    }

    oled_buff->encoding = 0;
    
    disp_buffer(idx, playback_state != 0 ? DISP_TEXT_BLINK : DISP_TEXT_NORMAL, 0);

    return 0;
}

void track_init() {
    track_view_state.invalidated = 0;
    track_view_state.time_m = -1;
    track_view_state.time_s = -1;
    track_view_state.track_id = -1;
    track_view_state.states[0].visible_length = DISPLAY_WIDTH - BATTERY_INDICATOR_SIZE - 1;
    track_view_state.states[1].visible_length = DISPLAY_WIDTH;
}