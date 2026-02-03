#!/usr/bin/env python3
import fw_tools as fw

def main(bus=None, device_address=None, show_uuids=False):
    md_iface = fw.connect()
    fw.prep_auth(md_iface, himd=True)

    # Trigger RH1 DFU mode
    md_iface.sendFactoryCommand([0x18,0x62,0xff])

if __name__ == '__main__':
    main()