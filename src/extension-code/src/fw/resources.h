#include "core.h"

#define FW_MENU_LENGTH 171
static char** fw_menu_strings_oled = (char**) (0x000d0f30);

typedef int type_search_for_menu_id(uint32_t needle, uint32_t* idx);
static type_search_for_menu_id* fw_search_for_menu_id = (type_search_for_menu_id*)(THUMB_ADDR(0x000b5cd0));

typedef int type_load_string_resource(uint32_t block,char **tbl,uint32_t size,uint32_t x,uint32_t y,char *dest,uint8_t *len,char *z);
static type_load_string_resource* fw_load_string_resource = (type_load_string_resource*)(THUMB_ADDR(0x000b5c88));

