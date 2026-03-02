#pragma once
#include "core.h"

typedef void ftype_write_block_to_eeprom(uint32_t idx);
static ftype_write_block_to_eeprom* fw_write_block_to_eeprom = (ftype_write_block_to_eeprom*)(THUMB_ADDR(0x00028ad0));

typedef uint32_t ftype_load_block_from_eeprom(uint32_t idx);
static ftype_load_block_from_eeprom* fw_load_block_from_eeprom = (ftype_load_block_from_eeprom*)(THUMB_ADDR(0x00028a0c));

typedef uint32_t ftype_eeprom_read(uint32_t src, void* buff, uint32_t len);
static ftype_eeprom_read* fw_eeprom_read = (ftype_eeprom_read*)(THUMB_ADDR(0x000263f8));

typedef uint32_t ftype_eeprom_write(uint32_t dst, void* buff, uint32_t len);
static ftype_eeprom_write* fw_eeprom_write = (ftype_eeprom_write*)(THUMB_ADDR(0x000263b4));

static func* fw_select_spi_eeprom = (func*)(THUMB_ADDR(0x0002675c));