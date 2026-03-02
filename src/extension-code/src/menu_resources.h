#pragma once

#define MENU_CFW_ID_MARKER 0xFF

#define MENU_STRING_LIST(X)            \
    X(CFW,           0x000000FF, "CFW Settings"  ) \
    \
    X(OLED,          0x000001FF, "OLED"          ) \
    \
    X(OLED_AUTO_OFF, 0x000101FF, "Auto Off"      ) \
    X(OLED_NORMAL,   0x010101FF, "Normal"        ) \
    X(OLED_DIM_ONLY, 0x020101FF, "Dim Only"      ) \
    X(OLED_FULLY_OFF,0x030101FF, "Fully Off"     ) \
    \
    X(OLED_USB,        0x000201FF, "In USB mode"   ) \
    X(OLED_U_NORMAL,   0x010201FF, "Normal"        ) \
    X(OLED_U_FULLY_OFF,0x030201FF, "Fully Off"     ) \
    \
    X(OLED_BR_LOW,   0x000301FF, "Brightness L"  ) \
    X(OLED_BR_L_L,   0x020301FF, "Low"       ) \
    X(OLED_BR_L_M1,  0x030301FF, "Medium 1"  ) \
    X(OLED_BR_L_M2,  0x040301FF, "Medium 2"  ) \
    X(OLED_BR_L_H,   0x050301FF, "High"      ) \
    \
    X(OLED_BR_HIGH,  0x000401FF, "Brightness H"  ) \
    X(OLED_BR_H_L,   0x020401FF, "Low"       ) \
    X(OLED_BR_H_M1,  0x030401FF, "Medium 1"  ) \
    X(OLED_BR_H_M2,  0x040401FF, "Medium 2"  ) \
    X(OLED_BR_H_H,   0x050401FF, "High"      ) \
    \
    X(NETMD,         0x000002FF, "NetMD"          ) \
    \
    X(WINUSB_DESCR,  0x000102FF, "WinUSB Decr."  ) \
    X(WINUSB_D_OFF,  0x010102FF, "Off"           ) \
    X(WINUSB_D_ON,   0x020102FF, "On"            ) \
    \
    X(SP_SPEEDUP,    0x000202FF, "SP Speedup"    ) \
    X(SP_SPEEDUP_OFF,0x010202FF, "Off"           ) \
    X(SP_SPEEDUP_ON, 0x020202FF, "On"            ) \
    \
    X(NETMD_CHECKOUT, 0x000302FF, "Checkout"      ) \
    X(CHECKOUT_NORMAL,0x010302FF, "Normal"        ) \
    X(CHECKOUT_BYPASS,0x020302FF, "Bypass"        ) \
    \
    X(TMARK,         0x000005FF, "T-Mark"         ) \
    X(TMARK_NORM,    0x000105FF, "T-Mark"         ) \
    X(TMARK_SKIP,    0x000205FF, "Group Skip"     ) \
    \
    X(TRACK_STYLE,    0x000008FF, "TrackInfoStyle"   ) \
    X(TRACK_STYLE1,   0x000108FF, "INFO+TIME|TRACK"  ) \
    X(TRACK_STYLE2,   0x000208FF, "TIME+INFO|TRACK"  ) \
    X(TRACK_STYLE3,   0x000308FF, "TRACK+TIME|INFO"  ) \
    X(TRACK_STYLE4,   0x000408FF, "VU|TRACK+TIME"    ) \
    X(TRACK_STYLE5,   0x000508FF, "CODEC|TRACK+TIME" ) \
    \
    X(ABOUT,         0x00000FFF, "About"         ) \
    X(ABOUT_A,       0x00010FFF, "CFW by Sir68k  \x8A" ) \
    X(ABOUT_B,       0x00020FFF, "minidisc.wiki  \x8A" ) \
    X(ABOUT_C,       0x00030FFF, "Pisco Sour Time\x8A" ) \
    X(ABOUT_D,       0x00040FFF, "sir68k.re      \x8A" )

#define DECL_STR(name, id, text) static const char menu_str_##name[] = text;
MENU_STRING_LIST(DECL_STR)

#define PTR_ENTRY(name, id, text) menu_str_##name,
static const char *const menu_string_table[] = {
    MENU_STRING_LIST(PTR_ENTRY)
};

#define IDX_ENTRY(name, id, text) id,
static const uint32_t menu_id_table[] = {
    MENU_STRING_LIST(IDX_ENTRY)
};

#define ENUM_ENTRY(name, id, text) MENU_IDX_##name = id,
enum
{
    MENU_STRING_LIST(ENUM_ENTRY)
};

#define MENU_RESOURCE_ID(name) MENU_IDX_##name