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

  /* LPC1788 ID is 0x281d3f47 = 673005383(decimal) */
  if (strcmp("673005383",(char*)s) == 0) {
    /* chip has 16 sectors of 4 kbytes, and 16 sectors of 32 kbytes
     * https://www.nxp.com/docs/en/user-guide/UM10470.pdf
     * Section 37.5 Sector numbers */
    ret = strdup("LPC1788");
    chip_sector_max = 29; /* last sector number */
    chip_sector = malloc(sizeof(chip_sector)*(chip_sector_max+1));
    chip_sector[0] =  0x00000;
    chip_sector[1] =  0x01000;
    chip_sector[2] =  0x02000;
    chip_sector[3] =  0x03000;
    chip_sector[4] =  0x04000;
    chip_sector[5] =  0x05000;
    chip_sector[6] =  0x06000;
    chip_sector[7] =  0x07000;
    chip_sector[8] =  0x08000;
    chip_sector[9] =  0x09000;
    chip_sector[10] = 0x0a000;
    chip_sector[11] = 0x0b000;
    chip_sector[12] = 0x0c000;
    chip_sector[13] = 0x0d000;
    chip_sector[14] = 0x0e000;
    chip_sector[15] = 0x0f000;
    chip_sector[16] = 0x10000;
    chip_sector[17] = 0x18000;
    chip_sector[18] = 0x20000;
    chip_sector[19] = 0x28000;
    chip_sector[20] = 0x30000;
    chip_sector[21] = 0x38000;
    chip_sector[22] = 0x40000;
    chip_sector[23] = 0x48000;
    chip_sector[24] = 0x50000;
    chip_sector[25] = 0x58000;
    chip_sector[26] = 0x60000;
    chip_sector[27] = 0x68000;
    chip_sector[28] = 0x70000;
    chip_sector[29] = 0x78000;
    chip_addr_min = 0; /* first address of flash memory */
    chip_addr_max = 0x7ffff; /* last address of flash memory */
    chip_ram_transfer_addr = 0x10000300; /* a start address of free 512 bytes of RAM to be used as buffer to transfer pages */
  }

  /* LPC1125 ID is 0x150080 = 1376384(decimal) */
  if (strcmp("1376384",(char*)s) == 0) {
    int i;
    /* chip has 16 sectors of 4 kbytes each (all sectors with same size) */
    ret = strdup("LPC1125");
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
