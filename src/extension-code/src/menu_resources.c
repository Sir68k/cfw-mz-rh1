#include "fw/core.h"
#include "fw/resources.h"
#include "fw/sys.h"
#include "fw/tracks.h"
#include "menu_resources.h"

// Rewrite menu resource lookup

#define MENU_SPACE 0x20
#define MENU_START 0x8E
#define MENU_CLOSE 0x8F

#define TRACK_ERASE_STRING "Erase Track    "

int cfw_load_menu_string_oled(uint32_t id, char *dest, uint8_t *len, char *n, uint32_t p, uint32_t config)
{

    int res;
    int32_t offset;
    uint32_t tmp_len;
    uint32_t idx = 0;

    void *menu_data = fw_menu_strings_oled;

    if (((config >> 2) & 1) == 0 && ((id & 0xFF) != MENU_CFW_ID_MARKER))
    {
        res = fw_search_for_menu_id(id, &offset);
        if (res != 0)
            return res;
    }
    
    else if ((id & 0xFF) == MENU_CFW_ID_MARKER) {
        offset = 0;
        idx = 0;

        for (idx = 0; idx < (sizeof(menu_id_table) / sizeof(uint32_t)); idx++) {
            if (menu_id_table[idx] == id) {
                break;
            }
        }
        menu_data = (void*)menu_string_table;
    }
    else
    {
        offset = id;
        if (id > FW_MENU_LENGTH)
            return -1;
    }

    *dest = ((config & 8) != 0) ? MENU_START : MENU_SPACE;

    res = fw_load_string_resource(offset, menu_data, 0x10, 1, idx, dest + 1, len, n);


    if (id == 0x10500) { // if we are in the track erase menu, inject track number
        uint16_t trackId = *current_track;
        memcpy(dest + 1, TRACK_ERASE_STRING, sizeof(TRACK_ERASE_STRING));
        int_to_string(trackId, &dest[13], 1, 0, 0);
        *len = sizeof(TRACK_ERASE_STRING) - 1;
    }

    if ((config & 8) == 0)
    {
        tmp_len = *len + 1;
    }
    else
    {
        memset(dest + 1 + (*len), MENU_SPACE, 0x10 - (*len));
        dest[0x11] = MENU_CLOSE;
        tmp_len = 0x12;
    }

    *len = tmp_len;
    return res;
}