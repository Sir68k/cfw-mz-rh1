#include <stdint.h>

#define SECTOR_SIZE 0x1000

#define STATUS_SECTOR_ERASE  0x10000000
#define STATUS_SECTOR_WRITE  0x20000000
#define STATUS_FLASHING_DONE 0xCAFEBABE

#define VOL32 volatile uint32_t*
#define VOL16 volatile uint16_t*
#define VOL8 volatile uint8_t*

static VOL16 _rom_aaaa = (VOL16)0x0100AAAA;
static VOL16 _rom_5554 = (VOL16)0x01005554;

static VOL8 _patch_ctl_a = (VOL8)0x03804100;
static VOL8 _patch_ctl_b = (VOL8)0x03804110;

static VOL8 p_usb_ctl1 = (VOL8)0x0380c00e;
static VOL8 p_usb_ctl2 = (VOL8)0x0380c012;
static VOL8 p_usb_buff = (VOL8)0x0380c020;

static VOL8 p_gpio_led = (VOL8)0x03001070;
#define GPIO_LED_MASK 0xF7
#define GPIO_LED_SHIFT 3

struct __attribute__((packed)) PatchEntry {
    uint32_t enabled;
    uint32_t addr;
    uint32_t data;
    uint32_t x;
};

static volatile struct PatchEntry* const _patch_table = (struct PatchEntry*)0x03804000;

uint32_t disable_interrupts(void)
{
    uint32_t old_cpsr;
    uint32_t new_cpsr;

    asm volatile (
        "mrs %0, cpsr\n"
        "orr %1, %0, #0xC0\n"
        "msr cpsr_c, %1\n"
        : "=r" (old_cpsr), "=r" (new_cpsr)
        :
        : "memory", "cc"
    );

    return old_cpsr;
}

void enable_interrupts(uint32_t cpsr)
{
    asm volatile (
        "msr cpsr_c, %0"
        :
        : "r" (cpsr)
        : "memory", "cc"
    );
}

void usb_send_word(uint32_t word)
{
    *p_usb_ctl1 = 0;
    *p_usb_ctl2 |= 0x40;
    *p_usb_buff = (word >>  0) & 0xFF;
    *p_usb_buff = (word >>  8) & 0xFF;
    *p_usb_buff = (word >> 16) & 0xFF;
    *p_usb_buff = (word >> 24) & 0xFF;
    *p_usb_ctl1 = 0;
    *p_usb_ctl2 |= 8;
    *p_usb_ctl2 |= 2;
}

void sector_erase(VOL16 sector) {
    *_rom_aaaa = 0xAA;
    *_rom_5554 = 0x55;
    *_rom_aaaa = 0x80;
    *_rom_aaaa = 0xAA;
    *_rom_5554 = 0x55;
    *sector    = 0x70; // erase sector written to
    while (*sector != 0xFFFF) {};
}

void sector_write(VOL16 dest, uint16_t val) {
    *_rom_aaaa = 0xAA;
    *_rom_5554 = 0x55;
    *_rom_aaaa = 0xA0;

    *dest = val;
    while (*dest != val) {};
}

void toggle_patches(uint8_t enable) {
    //*_patch_ctl_b = enable ? 8 : 0;

    *_patch_ctl_a = 0x5;
    *_patch_ctl_a = 0xc;
    _patch_table[0].enabled = enable;
    _patch_table[1].enabled = enable;
    _patch_table[2].enabled = enable;
    _patch_table[3].enabled = enable;
    *_patch_ctl_a = 0x5;
    *_patch_ctl_a = 0x9;
}

int sector_needs_updating(const volatile void *a, const volatile void *b)
{
    const uint32_t *p1 = (const uint32_t *)a;
    const uint32_t *p2 = (const uint32_t *)b;

    const uint32_t words = SECTOR_SIZE / sizeof(uint32_t);

    for (uint32_t i = 0; i < words; i++) {
        if (p1[i] != p2[i]) {
            return 1;  // different
        }
    }

    return 0;  // equal
}

uint32_t led = 0;

uint32_t main(void) {

    uint32_t sreg = disable_interrupts();
    toggle_patches(0);

    uint32_t    src_addr  = 0x02100000, 
                length    = 0x00100000 - 0x1000, 
                dest_addr = 0x01000000;

    uint32_t start_sector = dest_addr & ~(SECTOR_SIZE - 1);
    uint32_t end_addr     = dest_addr + length;
    uint32_t sector       = start_sector;

    while (sector < end_addr) {
        if (sector_needs_updating((VOL32)sector,(VOL32)src_addr)) {
            usb_send_word(sector | STATUS_SECTOR_WRITE);
            sector_erase((VOL16) sector);

            for (uint32_t i=0; i<SECTOR_SIZE / sizeof(uint16_t); i++) {
                sector_write(&((VOL16) sector)[i], ((VOL16) src_addr)[i]);
            }

            led = !led;
            *p_gpio_led = (*p_gpio_led & GPIO_LED_MASK) | (led << GPIO_LED_SHIFT);
        }

        src_addr += SECTOR_SIZE;
        sector += SECTOR_SIZE;
    }

    *p_gpio_led = (*p_gpio_led & GPIO_LED_MASK);
    usb_send_word(STATUS_FLASHING_DONE);
    
    toggle_patches(1);
    enable_interrupts(sreg);

    return STATUS_FLASHING_DONE;
}