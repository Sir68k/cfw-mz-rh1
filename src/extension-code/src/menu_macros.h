#pragma once

#include "menu_resources.h"


#define MENU_VALUE_START(name) \
    static const struct t_menu_value_entry menu_values_##name[] = {


#define MENU_VALUE_ENTRY(entry_id, value) \
    { entry_id, value },


#define MENU_VALUE_END(name, storage, changehook) \
    }; \
    static const struct t_menu_value_config config_values_##name = { \
        .entries = menu_values_##name, \
        .ptr_a = &storage, \
        .ptr_b = &storage, \
        .x = 0, \
        .y = 0, \
        .len = (sizeof(menu_values_##name) / sizeof(struct t_menu_value_entry)), \
    }; \
    static const struct t_menu_config_entry menu_cfg_##name[] = { \
        {.type=MENU_CFG_VAL_MENU, .value.ptr=&config_values_##name}, \
        {.type=MENU_CFG_CHANGE_HOOK, .value.ptr=changehook}, \
        {.type=0x28, .value.uint=0x20}, \
        {.type=0x2c, .value.uint=0x01}, \
        {0, 0} \
    };

#define MENU_VALUE_END_SIMPLE(name, storage) \
    }; \
    static const struct t_menu_value_config config_values_##name = { \
        .entries = menu_values_##name, \
        .ptr_a = &storage, \
        .ptr_b = &storage, \
        .x = 0, \
        .y = 0, \
        .len = (sizeof(menu_values_##name) / sizeof(struct t_menu_value_entry)), \
    }; \
    static const struct t_menu_config_entry menu_cfg_##name[] = { \
        {.type=MENU_CFG_VAL_MENU, .value.ptr=&config_values_##name}, \
        {0, 0} \
    };

#define MENU_START(name) \
    __attribute__((used)) \
    static const struct t_basic_menu menu_items_##name[] = {


#define MENU_ITEM_SUBMENU(item_id, submenu) \
    { \
        .id = item_id, \
        .callbacks = fw_menu_main_callbacks, \
        .config = &menu_cfg_##submenu, \
        .type = 1, \
    },

#define MENU_ITEM_VALUE(item_id, value_menu) \
    { \
        .id = item_id, \
        .callbacks = fw_menu_main_callbacks, \
        .config = menu_cfg_##value_menu, \
        .type = 3, \
    },

#define MENU_ITEM_CUSTOM(item_id, cb, cfg, t) \
    { \
        .id = item_id, \
        .callbacks = cb, \
        .config = cfg, \
        .type = t, \
    },

#define MENU_END(name) \
    { \
        .id = 0xFFFFFFFF, \
        .callbacks = (void *)0, \
        .config = (void *)0, \
        .type = 0, \
    }, \
    }; \
    const struct t_menu_config_entry menu_cfg_##name[] = { \
        {.type=0x2d, .value.uint=0x2000004}, \
        {.type=MENU_CFG_SUB_MENU, .value.ptr=&menu_items_##name}, \
        {0, 0} \
    };


#define MENU_END_CONFIG(name, cfg_type, cfg_val) \
    { \
        .id = 0xFFFFFFFF, \
        .callbacks = (void *)0, \
        .config = (void *)0, \
        .type = 0, \
    }, \
    }; \
    const struct t_menu_config_entry menu_cfg_##name[] = { \
        {.type = cfg_type, .value.uint = cfg_val}, \
        {.type = MENU_CFG_SUB_MENU, .value.ptr = &menu_items_##name}, \
        {0, 0} \
    };

#define MENU_REF(name) menu_cfg_##name

#define MENU_EXPORT(name) extern const struct t_menu_config_entry MENU_REF(name)[]