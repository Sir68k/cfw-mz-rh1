MEMORY
{
  rom      (rx)  : ORIGIN = 0x00000000, LENGTH = 0x000A0000
  sram1    (rwx) : ORIGIN = 0x00800000, LENGTH = 0x00008000
  sram2    (rwx) : ORIGIN = 0x00810000, LENGTH = 0x00008000
  dram     (rwx) : ORIGIN = 0x02000000, LENGTH = 0x01000000
  app      (rwx) : ORIGIN = 0x02000000, LENGTH = 0x00010000
}

SECTIONS
{
    .text :
    {
        *(.text.startup.main)
        *(.text*)
        *(.rodata*)
        *(.data*)
        *(.bss*)
    } > app 
}
