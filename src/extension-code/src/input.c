#include "fw/core.h"
#include "cfw_config.h"

#define KEY_VOL_MINUS_MAX 0x0B
#define KEY_VOL_MINUS_KEY 0x53

#define KEY_VOL_PLUS_MAX 0x2A
#define KEY_VOL_PLUS_KEY 0x51

#define KEY_TMARK_MAX 0x6F
#define KEY_TMARK_KEY 0x31

#define KEY_GROUP_PLUS_KEY 0x64

uint8_t cfw_process_inp_state_key2(uint32_t raw_adc_key_value) {
    if (raw_adc_key_value <= KEY_VOL_MINUS_MAX)
        return KEY_VOL_MINUS_KEY;
    if (raw_adc_key_value <= KEY_VOL_PLUS_MAX)
        return KEY_VOL_PLUS_KEY;
    if (raw_adc_key_value <= KEY_TMARK_MAX)
        return cfw_config.cfw_tmark_override ? KEY_GROUP_PLUS_KEY : KEY_TMARK_KEY;

    return 0;
}