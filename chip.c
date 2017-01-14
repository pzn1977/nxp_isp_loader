#include <stdlib.h>
#include <string.h>

#include "chip.h"

uint32_t *chip_sector; /* initial addresses of flash sector pages */
uint16_t chip_sector_max = 0;
uint32_t chip_addr_min = 0;
uint32_t chip_addr_max = 0;
uint32_t chip_ram_transfer_addr = 0;

char * chip_flash_configure (uint8_t * s) {
  char * ret = NULL;

  /* LPC1115 ID is 0x00050080 = 327808(decimal) */
  if (strcmp("327808",(char*)s) == 0) {
    int i;
    /* chip has 16 sectors of 4 kbytes each (all sectors with same size) */
    ret = strdup("LPC1115");
    chip_sector_max = 15; /* last sector number */
    chip_sector = malloc(sizeof(chip_sector)*(chip_sector_max+1));
    for (i=0; i<=chip_sector_max; i++) {
      chip_sector[i] = 0x1000 * i; /* initial address of each sector */
    }
    chip_addr_min = 0; /* first address of flash memory */
    chip_addr_max = 0xffff; /* last address of flash memory */
    chip_ram_transfer_addr = 0x10000300; /* a start address of free 512 bytes of RAM to be used as buffer to transfer pages */
  }

  return ret;
}

int chip_addr2sect (uint32_t addr) {
  int i, ret = 0;
  if (addr < chip_addr_min) return -1;
  if (addr > chip_addr_max) return -1;
  for (i=0; i<=chip_sector_max; i++) {
    if (addr >= chip_sector[i]) ret = i;
  }
  return ret;
}
