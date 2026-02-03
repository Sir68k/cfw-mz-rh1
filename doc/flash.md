# CXD2687 Flashing

The CXD2687 contains 1 MiB of internal Flash organized in 4 KiB sectors (4096 bytes).
All flash operations (unlock, erase, program) are performed using 16-bit word accesses.

## Control addresses

- `BASE + 0x5554` (command register A)
- `BASE + 0xAAAA` (command register B)

### Command sequences

Note: all values below are 16-bit writes. `BASE` is the chosen Flash alias base (see "Patch peripheral" below).

Erase a sector (sector = sector base address / offset):

```
write16(BASE + 0x5554, 0x00AA)
write16(BASE + 0x5554, 0x0055)
write16(BASE + 0xAAAA, 0x0080)
write16(BASE + 0xAAAA, 0x00AA)
write16(BASE + 0x5554, 0x0055)
write16(BASE + sector, 0x0070)
# Wait: read back until the word at (BASE + sector) == 0xFFFF
```

Program a 16-bit word at `addr` (address must be 16-bit aligned and within an erased sector):

```
write16(BASE + 0x5554, 0x00AA)
write16(BASE + 0x5554, 0x0055)
write16(BASE + 0xAAAA, 0x00A0)
write16(BASE + addr,   value)
# Wait: read back until the word at addr == value
```

### Patch peripheral and preferred base alias

The patch peripheral overlays the low 1 MiB of the address space with patched words (used in DFU and other modes). To avoid accidental interference during flashing with the patch unit use the flash alias mapped at `0x0100_0000` rather than `0x0000_0000` when performing raw flash operations; this alias maps to the flash as well but is not affected by the patch overlay.

Regardless, it is best that before performing low-level flash operations all active patches are disabled.
