MEMORY
{
  # flash    (rx)  : ORIGIN = 0x00000000, LENGTH = 0x000A0000
  # sram     (rwx) : ORIGIN = 0x00800000, LENGTH = 0x00008000
  # dram     (rwx) : ORIGIN = 0x02000000, LENGTH = 0x01000000

  # Flash free zone: 000E_1000 - 000E_B000 -> A000 (40_960, ~40k) bytes
  # S-RAM safe zone: 0080_5e00 - 0080_7800 -> 1A00 (06_656,  ~6k) bytes
  # D-RAM safe zone: 0200_8900 - 0200_e300 -> 5A00 (23_040, ~23k) bytes

  s_flash  (rx)  : ORIGIN = 0x000E1000, LENGTH = 0x0000A000
  s_sram   (rwx) : ORIGIN = 0x00805e00, LENGTH = 0x00001A00
  s_dram   (rwx) : ORIGIN = 0x02008900, LENGTH = 0x00005A00
}

SECTIONS
{
    __cfw_start__ = LOADADDR(.text);

    .text :
    {
        *(.text.init)
        *(.text*)
        *(.rodata*)
        . = ALIGN(4);
    } > s_flash

    .data : AT (LOADADDR(.text) + SIZEOF(.text))
    {
        __data_start__ = .;
        *(.data*)
        __data_end__ = .;
    } > s_sram

    __data_load__ = LOADADDR(.data);

    .bss (NOLOAD) :
    {
        . = ALIGN(4);
        __bss_start__ = .;
        *(.bss*)
        *(COMMON)
        __bss_end__ = .;
    } > s_sram
}
