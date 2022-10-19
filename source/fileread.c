#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "fileread.h"

static int fd = -1;

int fileread_open (char * fname) {
  if (fd >= 0) {
    close (fd);
  }
  fd = open(fname,O_RDONLY);
  if (fd < 0) {
    return 0; /* failed */
  }
  return 1;
}

uint8_t * fileread_getpage512 () {
  static uint8_t buf[512];
  int i;
  if (fd < 0) {
    return NULL;
  }
  i = read(fd,buf,512);
  if (i <= 0) {
    return NULL;
  }
  if (i == 512) return buf;
  for (; i<512; i++) {
    buf[i] = 0; /* fill unused spaces on page with 0 */
  }
  return buf;
}

int fileread_getpage512_r (uint8_t * data) {
  int i;
  if (fd < 0) {
    return 0;
  }
  i = read(fd,data,512);
  if (i <= 0) {
    return 0;
  }
  return i;
}

void fileread_rewind () {
  if (fd >= 0) {
    lseek(fd, 0, SEEK_SET);
  }
}
