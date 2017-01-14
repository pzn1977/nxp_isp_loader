#ifndef _CHIP_H
#define _CHIP_H

#include <stdint.h>

extern uint32_t *chip_sector;
extern uint16_t chip_sector_max;
extern uint32_t chip_addr_min;
extern uint32_t chip_addr_max;
extern uint32_t chip_ram_transfer_addr;

char * chip_flash_configure (uint8_t * s);
int chip_addr2sect (uint32_t addr);

#endif
