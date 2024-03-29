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

  if (strcmp("654381362",(char*)s) == 0) {
    /* https://www.nxp.com/docs/en/user-guide/UM10470.pdf
     * Section 37.5 Sector numbers */
    ret = strdup("LPC1774");
    chip_sector_max = 17; /* last sector number */
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
    chip_addr_min = 0; /* first address of flash memory */
    chip_addr_max = 0x1ffff; /* last address of flash memory */
    chip_ram_transfer_addr = 0x10000300; /* a start address of free 512 bytes of RAM to be used as buffer to transfer pages */
  }

  if ((strcmp("672997187",(char*)s) == 0) ||
      (strcmp("672995139",(char*)s) == 0) ||
      (strcmp("655957827",(char*)s) == 0)) {
    /* https://www.nxp.com/docs/en/user-guide/UM10470.pdf
     * Section 37.5 Sector numbers */
    if (strcmp("672997187",(char*)s) == 0) ret = strdup("LPC1786");
    if (strcmp("672995139",(char*)s) == 0) ret = strdup("LPC1785");
    if (strcmp("655957827",(char*)s) == 0) ret = strdup("LPC1776");
    chip_sector_max = 21; /* last sector number */
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
    chip_addr_min = 0; /* first address of flash memory */
    chip_addr_max = 0x3ffff; /* last address of flash memory */
    chip_ram_transfer_addr = 0x10000300; /* a start address of free 512 bytes of RAM to be used as buffer to transfer pages */
  }

  if ((strcmp("673005383",(char*)s) == 0) ||
      (strcmp("673003335",(char*)s) == 0) ||
      (strcmp("655966023",(char*)s) == 0) ||
      (strcmp("655963975",(char*)s) == 0)) {
    /* https://www.nxp.com/docs/en/user-guide/UM10470.pdf
     * Section 37.5 Sector numbers */
    if (strcmp("673005383",(char*)s) == 0) ret = strdup("LPC1788");
    if (strcmp("673003335",(char*)s) == 0) ret = strdup("LPC1787");
    if (strcmp("655966023",(char*)s) == 0) ret = strdup("LPC1778");
    if (strcmp("655963975",(char*)s) == 0) ret = strdup("LPC1777");
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

  /* LPC11U34FBD48/311 ID is 0x03D440 = 250944(decimal)
   * from NXP UM10462.pdf page 389 table 361 LPC11Uxx device identification numbers */
  if (strcmp("250944",(char*)s) == 0) {
    int i;
    /* chip has 16 sectors of 4 kbytes each (all sectors with same size) */
    ret = strdup("LPC11U34FBD48/311 or LPC11U34FHN33/311");
    chip_sector_max = 9; /* last sector number */
    chip_sector = malloc(sizeof(chip_sector)*(chip_sector_max+1));
    for (i=0; i<=chip_sector_max; i++) {
      chip_sector[i] = 0x1000 * i; /* initial address of each sector */
    }
    chip_addr_min = 0; /* first address of flash memory */
    chip_addr_max = 0x9fff; /* last address of flash memory */
    chip_ram_transfer_addr = 0x10000300; /* a start address of free 512 bytes of RAM to be used as buffer to transfer pages */
  }

  /* LPC1113FBD48/301 is 0x0434102B or 0x2532102B */
  if ((strcmp("70520875",(char*)s) == 0) ||
      (strcmp("624037931",(char*)s) == 0)) {
    int i;
    /* chip has 6 sectors of 4 kbytes each (all sectors with same size) */
    ret = strdup("LPC1113FBD48/301");
    chip_sector_max = 5; /* last sector number */
    chip_sector = malloc(sizeof(chip_sector)*(chip_sector_max+1));
    for (i=0; i<=chip_sector_max; i++) {
      chip_sector[i] = 0x1000 * i; /* initial address of each sector */
    }
    chip_addr_min = 0; /* first address of flash memory */
    chip_addr_max = 0x5fff; /* last address of flash memory */
    chip_ram_transfer_addr = 0x10000300; /* a start address of free 512 bytes of RAM to be used as buffer to transfer pages */
  }

  /* LPC1114/302 ID is 0x2540102b = 624955435(decimal) */
  if (strcmp("624955435",(char*)s) == 0) {
    int i;
    /* chip has 8 sectors of 4 kbytes each (all sectors with same size) */
    ret = strdup("LPC1114/302");
    chip_sector_max = 7; /* last sector number */
    chip_sector = malloc(sizeof(chip_sector)*(chip_sector_max+1));
    for (i=0; i<=chip_sector_max; i++) {
      chip_sector[i] = 0x1000 * i; /* initial address of each sector */
    }
    chip_addr_min = 0; /* first address of flash memory */
    chip_addr_max = 0x7fff; /* last address of flash memory */
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
