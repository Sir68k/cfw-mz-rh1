#pragma once

#include "core.h"

#define DISC_TYPE_MD   1
#define DISC_TYPE_HIMD 0

#define T_NAME_TRACK 0
#define T_NAME_GROUP 1

#define T_NAME_HIMD_TRACK  0
#define T_NAME_HIMD_ARTIST 1
#define T_NAME_HIMD_ALBUM  2
#define T_NAME_HIMD_GROUP  3
#define T_NAME_HIMD_DISC   4

struct __attribute__((packed)) himd_track_query {
    char* buffer;       // 0
    uint16_t track_id;  // 4
    uint16_t len;       // 6
    uint8_t type;       // 8
    uint8_t encoding;   // 9
    uint16_t x; 
};

static volatile uint16_t* current_track = (volatile uint16_t*) 0x008020f0;
static volatile uint16_t* current_group = (volatile uint16_t*) 0x008020f8;

static volatile uint8_t*  disc_info = (volatile uint8_t*)  0x008006a5;
static volatile uint32_t* disc_type = (volatile uint32_t*) 0x00800654;

typedef uint32_t ftype_get_track_name(
    uint32_t idx, 
    uint32_t offset, 
    uint32_t len, 
    uint32_t x, 
    uint8_t* buff, 
    uint32_t max, 
    uint32_t* partial, 
    int32_t* a, 
    int32_t* res_offset, 
    int32_t* c, 
    int type, 
    int e
);
static ftype_get_track_name* get_track_name = (ftype_get_track_name*)(THUMB_ADDR(0x0003060c));

typedef uint32_t ftype_get_track_name_himd(
    struct himd_track_query* q, 
    void*, void*, void*, uint32_t);
static ftype_get_track_name_himd* get_track_name_himd = (ftype_get_track_name_himd*)(THUMB_ADDR(0x00031768));


typedef uint32_t ftype_get_group_id_for_himd_track(uint32_t track);
static ftype_get_group_id_for_himd_track* get_group_id_for_himd_track = (ftype_get_group_id_for_himd_track*)(THUMB_ADDR(0x006f966));


typedef uint32_t ftype_get_track_progress(
    uint32_t a,
    uint32_t b,
    uint32_t* h,
    uint32_t* m,
    uint32_t* s
);
static ftype_get_track_progress* get_track_progress = (ftype_get_track_progress*)(THUMB_ADDR(0x00039c1e));

typedef uint32_t ftype_int_to_string(
    uint32_t num,
    char* out,
    uint32_t x,
    uint32_t y,
    uint32_t z
);
static ftype_int_to_string* int_to_string = (ftype_int_to_string*)(THUMB_ADDR(0xb6170));

typedef uint32_t ftype_get_playback_state();
static ftype_get_playback_state* get_playback_state = (ftype_get_playback_state*)(THUMB_ADDR(0x39e50));


