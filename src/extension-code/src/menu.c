#include "fw/core.h"
#include "fw/display.h"
#include "fw/menu.h"
#include "menu_resources.h"
#include "cfw_menu.h"


MENU_START(playback)
MENU_ITEM_CUSTOM(0x202, fw_menu_main_callbacks, fw_menu_202_playback_basic,  3)
MENU_ITEM_CUSTOM(0x302, fw_menu_main_callbacks, fw_menu_302_playback_repeat, 3)
MENU_END(playback)

MENU_START(main)
MENU_ITEM_SUBMENU(0x02, playback)
MENU_ITEM_CUSTOM(0x06, fw_menu_main_callbacks, fw_menu_6_rec_settings, 1)
MENU_ITEM_CUSTOM(0x00, fw_menu_main_callbacks, fw_menu_0_edit,         1)
MENU_ITEM_CUSTOM(0x05, fw_menu_spct_callbacks, fw_menu_5_speed_ctl,    3)
MENU_ITEM_CUSTOM(0x0B, fw_menu_main_callbacks, fw_menu_b_title_inp,    1)
MENU_ITEM_CUSTOM(0x08, fw_menu_main_callbacks, fw_menu_8_options,      1)
MENU_ITEM_SUBMENU(MENU_RESOURCE_ID(CFW), cfw)
MENU_END(main)
