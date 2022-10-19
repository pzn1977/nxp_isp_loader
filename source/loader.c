#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

#include "serial.h"
#include "chip.h"
#include "fileread.h"

/* hardcode ANSI codes for RED and GREEN messages for ERROR and OK */
#define MSG_ERR "\x1b[31;1mERROR\x1b[0m"
#define MSG_OK  "\x1b[32;1mOK\x1b[0m"

// Receive buffer size
#define BUFSIZE 64

// COMMAND buffer size
#define COMMAND_BUFFER 64

// ECHO is emable by default
bool ECHO_ENABLED = true;

int send_command_and_receive_response(struct serial * dev, unsigned char* command, uint8_t* buf);

void print_dbg(char* t, uint8_t* s, int i) {
  int j;
  printf("\n%s-hex:",t);
  for (j=0; j<i; j++) {
    printf(" %02x",s[j]);
  }
  printf("\n");
  printf("%s-str: [",t);
  for (j=0; j<i; j++) {
    if ((s[j] >= 0x20) && (s[j] <= 0x7f))
      printf("%c",s[j]);
    else
      printf("~");
  }
  printf("]\n");
}

int recv_line(serial_t* dev, unsigned char* s, int maxsize) {
  int i = 0;
  while ((i < (maxsize-1)) && (serial_has_data (dev, 0, 250000))) {
    int c;
    c = serial_getc (dev);
    if (c >= 0) {
      s[i++] = c;
      if (c == 0x0a) {
	s[i] = 0;
	return i;
      }
    }
  }
  s[i] = 0;
  return i;
}
int verifysector512(serial_t* dev, uint32_t addr, uint8_t* data, int size) {
  uint8_t s[COMMAND_BUFFER], buf[BUFSIZE], dec[COMMAND_BUFFER];
  int i, j, k, match, offset = 0;

  printf("Verify addr 0x%06x: ", addr);

  sprintf((char*)s, "R %d 512\r\n", addr+offset);
  i = send_command_and_receive_response(dev, (unsigned char*)s, &buf[0]);
  print_dbg("commandR:", buf, i);
  if ((i != 3) || (memcmp(buf,"0\r\n",3) != 0)) {
    printf(MSG_ERR " at command 'R'\n");
    print_dbg("debug:", buf, i);
    return 0;
  }

  while (offset < 512) {
    printf("."); fflush(stdout);
    i = recv_line(dev, buf, BUFSIZE);
    j = 0;//nxp_uudecode_block(buf, dec);
    if (j == 0) {
      printf(MSG_ERR " at decode data %d\n",offset);
      return 0;
    }

    match = 1;
    for (k=0; k<j; k++) {
      if (offset+k < size) {
	      if (data[offset+k] != dec[k]) match = 0;
      }
    }
    if (! match) {
      int i;
      printf(MSG_ERR " MISMATCH!\nchip: 0x%06x",addr+offset);
      for (i=0; i<j; i++) {
        if (dec[i] == data[offset+i]) {
          printf(" ");
        } else {
          printf(" [");
        }
        printf("%02x",dec[i]);
        if (dec[i] != data[offset+i]) printf("]");
      }
      printf("\nfile: 0x%06x",addr+offset);

      for (i=0; i<j; i++) {
        if (dec[i] == data[offset+i]) {
          printf(" ");
        } else {
          printf(" [");
        }
        printf("%02x",data[offset+i]);
        if (dec[i] != data[offset+i]) printf("]");
      }
      printf("\n");
      return 0;
    }

    offset += j;

  }

  /* read the checksum and discard it
   * it does not make sense to check the checksum if all bytes were
   * previously compared one-by-one */
  i = recv_line(dev, buf, BUFSIZE);

  serial_puts (dev, (unsigned char*)"OK\r");
  printf("."); fflush(stdout);

  printf(" " MSG_OK "\n"); fflush(stdout);
  
  return 1;
}

int writesector512(serial_t* dev, uint32_t addr, uint8_t* data) {
  char s[COMMAND_BUFFER] = {0};
  uint8_t buf[BUFSIZE] = {0};
  int i, sector;

  sector = chip_addr2sect (addr);
  printf("Write sector %d addr 0x%06x \n", sector, addr);

  // Write data to RAM
  sprintf(s,"W %d 512\r\n", chip_ram_transfer_addr);
  printf("Write to RAM command: %s", s);
  i = send_command_and_receive_response(dev, (unsigned char*)s, &buf[0]);
  if ((i != 3) || (memcmp(buf,"0\r\n",3) != 0)) {
    printf(MSG_ERR " at command 'W'\n");
    print_dbg ("debug:", buf, i);
    return 0;
  }

  for (i = 0; i < 512; i ++) {
    serial_puts(dev, data+i);
    printf("."); fflush(stdout);
  }

  // Prepare sector
  sprintf(s, "P %d %d\r\n", sector, sector);
  printf("\nPrepare sector: %s\n", s);
  i = send_command_and_receive_response(dev, (unsigned char*)s, &buf[0]);
  if ((i != 3) || (memcmp(buf, "0\r\n", 3) != 0)) {
    printf(MSG_ERR " at command 'P'\n");
    print_dbg ("debug:", buf, i);
    return 0;
  }
  
  // Copy data from RAM to Flash
  sprintf(s, "C %d %d 512\r\n", addr, chip_ram_transfer_addr);
  printf("\nCopy data from RAM to Flash: %s", s);
  i = send_command_and_receive_response(dev, (unsigned char*)s, &buf[0]);
  if ((i != 3) || (memcmp(buf,"0\r\n",3) != 0)) {
    printf(MSG_ERR " at command 'C'\n");
    print_dbg ("debug:", buf, i);
    return 0;
  }
  printf(" " MSG_OK "\n");
  
  return 1;
}


void serial_destroy_and_init (serial_t ** dev, char * serialport, int bps) {
  if (*dev != NULL) serial_destroy(*dev);
  *dev = serial_init (serialport, bps,
		      SERIAL_BITS_8 | SERIAL_PAR_NONE |
		      SERIAL_STOP_1 | SERIAL_FLOW_NONE |
		      SERIAL_AUTOFLUSH);
  if (*dev == NULL) {
    printf(MSG_ERR " could not open serial port '%s'\n",serialport);
    exit(1);
  }
}

int is_a_number(char *s) {
  int i = 0;
  if (s[0] == 0) return 0; /* empty string */
  while (s[i] != 0) {
    if (s[i] < '0') return 0;
    if (s[i] > '9') return 0;
    i++;
  }
  return 1; /* non-empty string with only 0-9 chars */
}

int send_command_and_receive_response(struct serial * dev, unsigned char* command, uint8_t* buf)
{
    serial_puts(dev, command);
    usleep(10000);
    // receive echo
    if (ECHO_ENABLED) recv_line(dev, buf, BUFSIZE);
    // receive response
    memset(buf, 0, BUFSIZE);
    return recv_line(dev, buf, BUFSIZE);
}

int main (int argc, char ** argv) {
  serial_t * dev = NULL;
  uint8_t buf[BUFSIZE] = {0};
  int i = 0;
  uint32_t initial_addr = 0;
  int read_only = 0;
  int verify = 1;
  int erase_all = 0;
  int bps = 0;
  char * chip_id = NULL;

  if (argc < 6) {
    printf("Usage: %s serial-port serial-bps xtal-khz file.bin initial-address [mode] [chip-id]\n"
      "Example: %s /dev/ttyS0 19200 4000 main.bin 0x0000\n"
      " [mode] operation mode available options:\n"
      "    write: erase + write. do not read/verify\n"
      "    verify: read/verify only\n"
      "    erase-all: erase all sectors of chip prior to write.\n"
      "               this is useful to disable previous code-protect.\n"
      "    without specifying mode: erase + write + verify\n"
      " [chip-id] check chip model (and abort program if mismatch).\n"
      "    chip-id is the decimal number found at NXP docs \"part \n"
      "    identification numbers\". Example: 654381362 is LPC1774.\n"
      "    Hint: use any number to force a mismatch and show detected\n"
      "    chip-id, or check 'chip.c' source code.\n"
      "Exit status: 0 = ok\n"
      "             2 = error at chip detect phase\n"
      "             3 = error at compare/verify procedure\n"
      "             4 = chip-id mismatch\n"
      "             1 = other errors\n"
      "Developed by Pedro Zorzenon Neto - www.pzn.com.br\n",
      argv[0],argv[0]);
    return 1;
  }

  if (argc >= 7) {
    if (strcmp(argv[6],"verify")==0) {
      read_only = 1;
    } else if (strcmp(argv[6],"write")==0) {
      verify = 0;
    } else if (strcmp(argv[6],"erase-all")==0) {
      erase_all = 1;
    } else if (is_a_number(argv[6])) {
      chip_id = strdup(argv[6]);
    } else {
      printf("unknown argument '%s'\n",argv[6]);
      return 1;
    }
  }

  if (argc >= 8) {
    if (is_a_number(argv[7])) {
      chip_id = strdup(argv[7]);
    } else {
      printf("unknown argument '%s'\n",argv[7]);
      return 1;
    }
  }

  if (argc >= 9) {
    printf("Error: too many command line arguments.\n"
      "Run with no arguments to show usage help.\n");
    return 1;
  }

  initial_addr = strtol(argv[5], NULL, 0);
  if ((initial_addr & 0x1ff) != 0) {
    printf(MSG_ERR " initial address '%d' is not multiple of 512\n",
      initial_addr);
    return 1;
  }

  if (! fileread_open (argv[4])) {
    printf(MSG_ERR " could not open file '%s' for reading\n",argv[4]);
    return 1;
  }

  bps = serial_bps(atoi(argv[2]));
  if (bps == 0) {
    printf(MSG_ERR " unknown baud rate '%s'\n",argv[2]);
    return 1;
  } else {
    printf("bps: %i\n", bps);
  }

  serial_destroy_and_init (&dev, argv[1], bps);

  {
    int timeout = 10;
    while (timeout--) {
      printf("SYNC stage 1: ");
      serial_putc(dev, '?');
      i = recv_line(dev, buf, BUFSIZE);
      print_dbg("recv-sync1", buf, i);
      if ((i != 14) || (memcmp(buf,"Synchronized\r\n",14) != 0)) {
        if (timeout == 0) {
          printf("failed. timeout. " MSG_ERR "\n");
          return 2;
        }
        printf("failed. please reset microcontroller!\n");
        sleep (1);
        serial_destroy_and_init (&dev, argv[1], bps);
        } 
        else {
          printf(MSG_OK "\n");
          timeout = 0;
        }
    }
  }

  {
    printf("SYNC stage 2: ");
    i = send_command_and_receive_response(dev, (unsigned char*)"Synchronized\r\n", &buf[0]);
    print_dbg("recv-sync2", buf, i);
    if ((i != 4) || (memcmp(buf, "OK\r\n", 4) != 0)) {
      printf(MSG_ERR "\n");
      return 2;
    }
    printf(MSG_OK "\n");
  }


  {
      int clk = 0;
      char s[30];
      clk = atoi(argv[3]);
      sprintf(s,"%d\r\n",clk);
      printf("SET Clock %d Khz: ",clk);
      i = send_command_and_receive_response(dev, (unsigned char*)s, &buf[0]);
      print_dbg("recv-clk", buf, i);
      /* according to docs, the chip should answer "OK", however, I got some
      * chips that answer "Synchronized" in this stage... */
      if ((i != 4) || (memcmp(buf, "OK\r\n", 4) != 0)) {
        printf(MSG_ERR "\n");
        return 1;
      }
      printf(MSG_OK "\n");
  }

#if 1
  {
    printf("Disable local-echo: ");
    i = send_command_and_receive_response(dev, (unsigned char*)"A 0\r\n", &buf[0]);
    print_dbg("recv-disable-echo", buf, i);
    if ((i != 3) || (memcmp(buf,"0\r\n",3) != 0)) {
      printf(MSG_ERR "\n");
      return 1;
    }
    printf(MSG_OK "\n");
    ECHO_ENABLED = false;
  }
#endif

  {
    printf("Identify microcontroller: ");
    i = send_command_and_receive_response(dev, (unsigned char*)"J\r\n", &buf[0]);
    print_dbg("identify mcu", buf, i);
    if ((i != 3) || (memcmp(buf,"0\r\n", 3) != 0)) {
      printf(MSG_ERR "\n");
      return 1;
    }
    i = recv_line(dev, buf, BUFSIZE);
    if (i < 3) {
      printf(MSG_ERR "\n");
    }
  }

  {
    int i;
    char * chip;
    for (i=0; i<strlen((char*)buf); i++) {
      if (buf[i] < 0x20) buf[i] = 0;
    }
    chip = chip_flash_configure (buf);
    if (chip == NULL) {
      printf("(unknownID)='%s' " MSG_ERR "\n",buf);
      return 1;
    }
    printf("%s",chip);
  }
  printf(" " MSG_OK "\n");

  if (chip_id != NULL) {
    if (strcmp(chip_id,(char*)buf) != 0) {
      printf("Chip-id mismatch expect='%s' found='%s' " MSG_ERR "\n",
        chip_id, buf);
      return 4;
    }
    printf("Chip-id match '%s' " MSG_OK "\n", chip_id);
  }

  if (read_only == 0) {
    printf("Unlock flash: ");
    i = send_command_and_receive_response(dev, (unsigned char*)"U 23130\r\n", &buf[0]);
    print_dbg("Unlock flash", buf, i);
    if ((i != 3) || (memcmp(buf,"0\r\n",3) != 0)) {
      printf(MSG_ERR "\n");
      return 1;
    }
    printf(MSG_OK "\n");
  }

  int sec_first;
  int sec_last;

  if (erase_all) {
    sec_first = 0;
    sec_last = chip_sector_max;
  } else {
    /* discover initial and final sectors for erasing */
    int size = 0;
    fileread_rewind();
    while (fileread_getpage512() != NULL) size += 512;
    printf("Binary file size: %d\n", size);

    sec_first = chip_addr2sect (initial_addr);
    sec_last = chip_addr2sect (initial_addr+size-1);

    if ((sec_first < 0) || (sec_last < 0)) {
      printf(MSG_ERR "flash sector calculate error\n");
      return 1;
    }
  }


#if 1
  if (read_only == 0) {
    printf("Prepare sectors %d to %d: ", sec_first, sec_last);
    char s[32];
    sprintf(s,"P %d %d\r\n", sec_first, sec_last);
    i = send_command_and_receive_response(dev, (unsigned char*)s, &buf[0]);
    print_dbg("recv-prepare", buf, i);
    if ((i != 3) || (memcmp(buf, "0\r\n", 3) != 0)) {
        printf(MSG_ERR "\n");
        return 1;
    }
    printf(MSG_OK "\n");
  }
#endif

#if 1
  if (read_only == 0) {
    printf("Erase Flash memory (sectors %d to %d): ", sec_first, sec_last);
    char s[32];
    sprintf(s, "E %d %d\r\n", sec_first, sec_last);
    i = send_command_and_receive_response(dev, (unsigned char*)s, &buf[0]);
    print_dbg("recv-erase", buf, i);
    if ((i != 3) || (memcmp(buf,"0\r\n", 3) != 0)) {
        if (erase_all) {
          printf(MSG_ERR "\n");
        } else {
          printf(MSG_ERR " - maybe CRP is on... "
            "try with 'erase-all' argument\n");
        }
        return 1;
    }
    printf(MSG_OK "\n");
  }
#endif

#if 1

  if (read_only == 0) {
    printf("Blank-Check (sectors %d to %d): ", sec_first, sec_last);
    char s[32];
    sprintf(s,"I %d %d\r\n",sec_first,sec_last);
    i = send_command_and_receive_response(dev, (unsigned char*)s, &buf[0]);
    if ((i != 3) || (memcmp(buf,"0\r\n",3) != 0)) {
      printf(MSG_ERR "\n");
      return 1;
    }
    printf(MSG_OK "\n");
  }
#endif
  

  if (read_only == 0) {
    uint32_t addr;
    int stop = 0;
    uint8_t * data;

    addr = initial_addr;
    fileread_rewind();
    while (! stop) {
      data = fileread_getpage512();
      if (buf != NULL) {
        if (writesector512(dev, addr, data) == 0) {
          return 1;
        }
        addr += 512;
      } else {
        stop = 1;
      }
    }
  }

  if (verify) {
    uint32_t addr;
    int stop = 0;
    addr = initial_addr;
    fileread_rewind();
    while (! stop) {
      uint8_t data[512];
      int size;
      size = fileread_getpage512_r (data);
      if (size > 0) {
        if (verifysector512(dev,addr,data,size) == 0) {
          return 3;
        }
        addr += 512;
      } else {
        stop = 1;
      }
    }
  }

  printf("Finished " MSG_OK ", please reboot microcontroler\n");
  return 0;
}
