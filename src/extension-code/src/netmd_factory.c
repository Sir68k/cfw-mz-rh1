#include "fw/core.h"
#include "fw/sys.h"
#include "cfw_config.h"

static VOL8 fw_netmd_factory_buffer = ((VOL8)0x0200633c);
static VOL32 fw_netmd_inp_len = ((VOL32)0x02003444);
static VOL32 fw_netmd_out_len = ((VOL32)0x02003448);

static void usb_factory_set_response(uint8_t code, uint32_t length)
{
    fw_netmd_factory_buffer[3] = code;
    *fw_netmd_out_len = length;
    fw_netmd_factory_buffer[0] = 9;
}

static void usb_factory_set_failure()
{
    *fw_netmd_out_len = *fw_netmd_inp_len;
    fw_netmd_factory_buffer[0] = 10;
    fw_netmd_factory_buffer[3] = 0xff;
}

static VOL8 hw_version = ((VOL8)0x00801241);
static VOL8 fw_version = ((VOL8)0x00000b30);

// extended version commands
void cfw_factory_cmd_0x12_get_version()
{
    uint8_t mode = fw_netmd_factory_buffer[3];

    if (mode == 0xff)
    {
            // Hardware version
            fw_netmd_factory_buffer[4] = *hw_version;
            fw_netmd_factory_buffer[5] = 0x02;

            // base version
            fw_netmd_factory_buffer[6] = 0xa0;
            fw_netmd_factory_buffer[7] = 0x10;

            usb_factory_set_response(0, 8);
    }
    else
    {
        usb_factory_set_failure();
    }
}

void cfw_factory_cmd_0x13_get_version()
{
    uint8_t mode = fw_netmd_factory_buffer[3];

    if (mode == 0xff)
    {
        uint8_t idx = fw_netmd_factory_buffer[4];
        if (idx == 0)
        {
            // base version
            fw_netmd_factory_buffer[5] = 0xa0;
            fw_netmd_factory_buffer[6] = 0x10;

            // cfw version
            fw_netmd_factory_buffer[7] = fw_version[0];
            fw_netmd_factory_buffer[8] = fw_version[1];

            // cfw-netmd capability
            fw_netmd_factory_buffer[9] = 0;

            // author name
            fw_netmd_factory_buffer[10] = 'S';
            fw_netmd_factory_buffer[11] = 'i';
            fw_netmd_factory_buffer[12] = 'r';
            fw_netmd_factory_buffer[13] = '6';
            fw_netmd_factory_buffer[14] = '8';
            fw_netmd_factory_buffer[15] = 'k';
            fw_netmd_factory_buffer[16] = 0;
            fw_netmd_factory_buffer[17] = 0;

            usb_factory_set_response(0, 18);
        }
        else
        {
            fw_netmd_factory_buffer[0] = 10;
            fw_netmd_factory_buffer[3] = 0xf4;
            *fw_netmd_out_len = 5;
        }
    }
    else
    {
        usb_factory_set_failure();
    }
}

#define CFW_NETMD_FCMD_MEM_READ     0x01
#define CFW_NETMD_FCMD_MEM_WRITE    0x02
#define CFW_NETMD_FCMD_EEPROM_READ  0x03
#define CFW_NETMD_FCMD_EEPROM_WRITE 0x04

#define CFW_NETMD_FCMD_EXEC         0x11

#define CFW_NETMD_FCMD_CONFIG_READ  0xF1
#define CFW_NETMD_FCMD_CONFIG_WRITE 0xF2

static inline uint32_t helper_read_word(volatile uint8_t *p) {
    return ((uint32_t)p[0])       |
           ((uint32_t)p[1] << 8)  |
           ((uint32_t)p[2] << 16) |
           ((uint32_t)p[3] << 24);
}

static inline uint16_t helper_read_hword(volatile uint8_t *p) {
    return ((uint16_t)p[0])       |
           ((uint16_t)p[1] << 8);
}



// 00 18 FF FF IDX
void cfw_factory_cmd_0xff_cfw_handler() 
{
    uint8_t idx = fw_netmd_factory_buffer[4];

    switch (idx) {
        case CFW_NETMD_FCMD_MEM_READ: {
            //      00 01 02 03 04 05 06 07  08 09 0A 0B   0C 0D 0E 0F  10
            // IN:  00 18 FF FF 01 XX XX XX [S0 S1 S2 S3] [L0 L1 L2 L3]
            // OUT: 00 18 FF FF 01 XX XX XX [S0 S1 S2 S3] [L0 L1 L2 L3] [DATA]
            uint8_t* src = (uint8_t*) helper_read_word(&fw_netmd_factory_buffer[0x8]);
            uint8_t* dst = (uint8_t*)&fw_netmd_factory_buffer[0x10];
            uint32_t len = helper_read_word(&fw_netmd_factory_buffer[0xC]);
            memcpy(dst, src, len);
            usb_factory_set_response(0, 0x10 + len);
            break;
        }
        case CFW_NETMD_FCMD_MEM_WRITE: {
            //      00 01 02 03 04 05 06 07  08 09 0A 0B   0C 0D 0E 0F  10
            // IN:  00 18 FF FF 02 XX XX XX [D0 D1 D2 D3] [L0 L1 L2 L3] [DATA]
            // OUT: 00 18 FF FF 02 XX XX XX [D0 D1 D2 D3] [L0 L1 L2 L3]
            uint8_t* src = (uint8_t*)&fw_netmd_factory_buffer[0x10];
            uint8_t* dst = (uint8_t*) helper_read_word(&fw_netmd_factory_buffer[0x8]);
            uint32_t len = helper_read_word(&fw_netmd_factory_buffer[0xC]);
            memcpy(dst, src, len);
            usb_factory_set_response(0, 0x10);
            break;
        }
        case CFW_NETMD_FCMD_EEPROM_READ: {
            //      00 01 02 03 04 05 06 07  08 09   0A 0B  0C
            // IN:  00 18 FF FF 03 XX XX XX [S0 S1] [L0 L1]
            // OUT: 00 18 FF FF 03 XX XX XX [S0 S1] [L0 L1] [DATA]
            uint16_t src = helper_read_hword(&fw_netmd_factory_buffer[0x08]);
            uint8_t* dst = (uint8_t*)&fw_netmd_factory_buffer[0x0C];
            uint16_t len = helper_read_hword(&fw_netmd_factory_buffer[0x0A]);
            cfw_eeprom_read(src, dst, len);
            usb_factory_set_response(0, 0x0C + len);
            break;
        }
        case CFW_NETMD_FCMD_EEPROM_WRITE: {
            //      00 01 02 03 04 05 06 07  08 09   0A 0B  0C
            // IN:  00 18 FF FF 04 XX XX XX [D0 D1] [L0 L1] [DATA]
            // OUT: 00 18 FF FF 04 XX XX XX [D0 D1] [L0 L1]
            uint8_t* src = (uint8_t*)&fw_netmd_factory_buffer[0x0C];
            uint16_t dst = helper_read_hword(&fw_netmd_factory_buffer[0x08]);
            uint16_t len = helper_read_hword(&fw_netmd_factory_buffer[0x0A]);
            cfw_eeprom_write(dst, src, len);
            usb_factory_set_response(0, 0xC);
            break;
        }
        case CFW_NETMD_FCMD_CONFIG_READ: {
            //      00 01 02 03 04  05   06
            // IN:  00 18 FF FF F1 [B0]
            // OUT: 00 18 FF FF F1 [B0] [config for B0]

            if (fw_netmd_factory_buffer[5] != 0) {
                // B0 can be used to denote config versions in the future
                usb_factory_set_failure();
                break;
            }

            uint8_t* dst = (uint8_t*)&fw_netmd_factory_buffer[0x06];
            uint32_t len = sizeof(struct cfw_config);
            memcpy(dst, &cfw_config, len);
            usb_factory_set_response(0, 0x06 + len);

            break;
        }
        case CFW_NETMD_FCMD_CONFIG_WRITE: {
            //      00 01 02 03 04  05   06
            // IN:  00 18 FF FF F2 [B0] [config for B0]
            // OUT: 00 18 FF FF F2 [B0] 

            if (fw_netmd_factory_buffer[5] != 0) {
                // B0 can be used to denote config versions in the future
                usb_factory_set_failure();
                break;
            }

            uint8_t* src = (uint8_t*)&fw_netmd_factory_buffer[0x06];
            uint32_t len = sizeof(struct cfw_config);
            memcpy(&cfw_config, src, len);
            store_config_to_eeprom();
            usb_factory_set_response(0, 0x06);

            break;
        }
        default:
            usb_factory_set_failure();
            break;
    }
}

