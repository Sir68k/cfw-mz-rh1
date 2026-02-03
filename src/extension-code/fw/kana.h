#include "core.h"

#define KANA_TBL_A_LEN 11
#define KANA_TBL_B_LEN 70

struct kana_a {
    char romn;
    char kana;
};

struct kana_b {
    char romn[2];
    char kana;
};


static volatile struct kana_a *c_katakana_tbl_a = (volatile struct kana_a*)0x000c3ccc;
static volatile struct kana_b *c_katakana_tbl_b = (volatile struct kana_b*)0x000c3ce2;


typedef uint32_t type_fw_kana_to_latin(void *in, void *out);
static type_fw_kana_to_latin* fw_kana_to_latin = (type_fw_kana_to_latin*)(THUMB_ADDR(0x00031308));


typedef int type_decode_katakana_name(
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
    int e);

static type_decode_katakana_name* fw_decode_katakana_name = (type_decode_katakana_name*)(THUMB_ADDR(0x00030b60));