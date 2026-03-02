#pragma once

#define ICON_ALBUM_L   ((char)0x86)
#define ICON_ALBUM_R   ((char)0x87)

#define ICON_ARTIST_L  ((char)0x84)
#define ICON_ARTIST_R  ((char)0x85)

#define ICON_GROUP_L   ((char)0xAA)
#define ICON_GROUP_R   ((char)0xAB)

#define ICON_DISC_L    ((char)0xBA)
#define ICON_DISC_R    ((char)0xBB)


#define APPEND_ICON(buf, idx, L, R) \
    do {                            \
        (buf)[(idx)++] = (L);       \
        (buf)[(idx)++] = (R);       \
        (buf)[(idx)++] = ' ';       \
    } while (0)
