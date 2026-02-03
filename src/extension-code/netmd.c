#include "fw/core.h"

static VOL8 fw_netmd_factory_buffer = ((VOL8)0x0200633c);
static VOL32 fw_netmd_inp_len = ((VOL32)0x02003444);
static VOL32 fw_netmd_out_len = ((VOL32)0x02003448);

void usb_factory_set_response(uint8_t code, uint32_t length)
{
    fw_netmd_factory_buffer[3] = code;
    *fw_netmd_out_len = length;
    fw_netmd_factory_buffer[0] = 9;
}

void usb_factory_set_failure()
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
