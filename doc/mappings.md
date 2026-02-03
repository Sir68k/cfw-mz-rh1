# RH1 Hardware details

## Memory Map

### In normal MCU mode:

| **START ADDRESS** | **END ADDRESS** | **SIZE** | **DESCRIPTION**                                              |
|-------------------|----------------|---------|--------------------------------------------------------------|
| `0x0000_0000`     | `0x000F_FFFF`  | 1MiB    | MCU Flash ROM (viewed through patch peripheral)              |
| `0x0001_0000`     | `0x007F_FFFF`  | 7MiB    | 7 repeated copies of 1MiB MCU Flash ROM (no patch peripheral)|
| `0x0080_0000`     | `0x0080_7FFF`  | 32KiB   | SRAM                                                        |
| `0x0081_0000`     | `0x0081_7FFF`  | 32KiB   | BOOT ROM (mapped to `0x0000_0000` when HSALF=LOW)           |
| `0x0100_0000`     | `0x017F_FFFF`  | 8MiB    | 8 repeated copies of 1MiB MCU Flash ROM (no patch peripheral)|
| `0x0180_0000`     | `0x01FF_FFFF`  | ~8MiB   | Unmapped, random data                                       |
| `0x0200_0000`     | `0x027F_FFFF`  | 8MiB    | DRAM                                                        |
| `0x0280_0000`     | `0x02FF_FFFF`  | 8MiB?   | DRAM as well? (TODO: check if writable)                     |
| `0x0300_0000`     | `0x0303_FFFF`  | 256KiB  | Basic MCU (SR11) peripherals                                 |
| `0x0380_0000`     | `0x0380_FFFF`  | 64KiB   | Advanced MCU (SR11) peripherals (Patches/DMA/USB/VME bridge) |
| `0x0600_0000`     | `0x07FF_FFFF`  | ~32MiB  | Unmapped, random data                                       |




**Notes:**
- VME ROM not visible


### In boot ROM mode (HSALF pulled low):

| **START ADDRESS** | **END ADDRESS** | **SIZE** | **DESCRIPTION**                                              |
|-------------------|----------------|---------|--------------------------------------------------------------|
| `0x0000_0000`     | `0x0000_7FFF`  | 32KiB   | BOOT ROM                                                    |
| `0x0080_0000`     | `0x0080_7FFF`  | 32KiB   | SRAM                                                        |
| `0x0081_0000`     | `0x0081_7FFF`  | 32KiB   | BOOT ROM (alias)                                            |
| `0x0100_0000`     | `0x017F_FFFF`  | 8MiB    | 8 repeated copies of 1MiB MCU Flash ROM (no patch peripheral)|
| `0x0200_0000`     | `0x027F_FFFF`  | 8MiB    | DRAM                                                        |
| `0x0280_0000`     | `0x02FF_FFFF`  | 8MiB?   | DRAM as well? (TODO: check if writable)                     |


### JTAG, connected to MCU:

| **START ADDRESS** | **END ADDRESS** | **DESCRIPTION** |
|-------------------|----------------|-----------------|
| `0x0000_0000`     | -              | VME ROM         |


**Notes:**
- VME: Discover what the ROM is, how it is mapped, if it is flash, and if the content changes per MCU FW version


---

**Version notes:**

*1.00p70 - BEr 1.2*
```
0300f560
00 00 12 00
```

*1.00pA0 - BEr 1.3*
```
0300f560
00 00 13 00
```