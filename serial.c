/* Serial port layer
 *
 * $Id: serial.c,v 1.6 2007-07-02 17:07:49 pzn Exp $
 *
 * (C) 2006 - Pedro Zorzenon Neto
 *
 * To use this this you must agree with its licence terms
 * License: GNU Lesser General Public License
 *          http://www.fsf.org/licensing/licenses/lgpl.html
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <sys/time.h>
#include <string.h>

#include "serial.h"
#include "assure.h"

serial_t * serial_init (char * path, unsigned int bps, unsigned int mode) {
  serial_t * self;
  struct termios term;
  speed_t spd = 0;
  int i;

  self = malloc (sizeof(serial_t));
  assure (self != NULL);
  
  self->initial_term = malloc (sizeof(struct termios));
  assure (self->initial_term != NULL);
  
  if (mode && SERIAL_AUTOFLUSH) {
    self->autoflush = 1;
  } else {
    self->autoflush = 0;
  }

  self->fd = open(path, O_RDWR);
  if (self->fd == -1) {
    /* open error */
    free (self);
    return NULL;
  }

  /* save terminal setting for restoring at serial_destroy */
  assure (tcgetattr (self->fd, self->initial_term) == 0);

  /* get current settings */
  assure (tcgetattr (self->fd, &term) == 0);

  /* input mode flags */
  term.c_iflag &= ~ ( BRKINT | ICRNL | IGNCR | INLCR | IGNPAR | INPCK |
		      PARMRK | ISTRIP | IXOFF | IXON);
  term.c_iflag |= IGNBRK;
  
  /* output mode flags */
  term.c_oflag &= ~ ( OPOST | ONLCR | OCRNL | ONOCR | ONLRET | OFILL |
		      OFDEL | NLDLY | CRDLY | TABDLY | BSDLY | VTDLY |
		      FFDLY);
  
  /* local mode flags */
  term.c_lflag &= ~ ( ECHO | ECHOE | ECHOK | ECHONL | ICANON | IEXTEN |
		      ISIG | NOFLSH | TOSTOP );
  
  /* control mode flags */
  term.c_cflag &= ~ ( HUPCL );
  term.c_cflag |= CREAD;

  /* parity settings */
  i = mode & ( SERIAL_PAR_NONE | SERIAL_PAR_ODD | SERIAL_PAR_EVEN |
	       SERIAL_PAR_MARK | SERIAL_PAR_SPACE | SERIAL_PAR_KEEP);
  switch(i) {
  case SERIAL_PAR_NONE:
    term.c_cflag &= ~PARENB;
    break;
  case SERIAL_PAR_ODD:
    term.c_cflag |= PARENB | PARODD;
    break;
  case SERIAL_PAR_EVEN:
    term.c_cflag |= PARENB;
    term.c_cflag &= PARODD;
    break;
  case SERIAL_PAR_KEEP:    
    break;
  case SERIAL_PAR_MARK: /* unsupported */
  case SERIAL_PAR_SPACE: /* unsuported */
  default:
    assure(0);
  }

  /* data bits settings */
  i = mode & ( SERIAL_BITS_8 | SERIAL_BITS_7 |
	       SERIAL_BITS_6 | SERIAL_BITS_5 | SERIAL_BITS_KEEP);
  switch(i) {
  case SERIAL_BITS_8:
    term.c_cflag &= ~(CS5 | CS6 | CS7 | CS8);
    term.c_cflag |= CS8;
    break;
  case SERIAL_BITS_7:
    term.c_cflag &= ~(CS5 | CS6 | CS7 | CS8);
    term.c_cflag |= CS7;
    break;
  case SERIAL_BITS_6:
    term.c_cflag &= ~(CS5 | CS6 | CS7 | CS8);
    term.c_cflag |= CS6;
    break;
  case SERIAL_BITS_5:
    term.c_cflag &= ~(CS5 | CS6 | CS7 | CS8);
    term.c_cflag |= CS5;
    break;
  case SERIAL_BITS_KEEP:
    break;
  default:
    assure(0);
  }

  /* stop bit settings */
  i = mode & ( SERIAL_STOP_1 | SERIAL_STOP_2 | SERIAL_STOP_KEEP);
  switch (i) {
  case SERIAL_STOP_1:
    term.c_cflag &= ~CSTOPB;
    break;
  case SERIAL_STOP_2:
    term.c_cflag |= CSTOPB;
    break;
  case SERIAL_STOP_KEEP:
    break;
  default:
    assure(0);
  }

  /* flow control */
  i = mode & ( SERIAL_FLOW_NONE | SERIAL_FLOW_HARD | SERIAL_FLOW_SOFT |
	       SERIAL_FLOW_KEEP );
  switch (i) {
  case SERIAL_FLOW_NONE:
    term.c_cflag |= CLOCAL;
    term.c_cflag &= ~CRTSCTS;
    break;
  case SERIAL_FLOW_HARD:
    term.c_cflag &= ~CLOCAL;
    term.c_cflag |= CRTSCTS;
    break;
  case SERIAL_FLOW_KEEP:
    break;
  case SERIAL_FLOW_SOFT: /* do not know if it makes sense non-canonical
			    XON/XOFF flow control... */
  default:
    assure(0);
  }
  
  /* control characters */
  //term.c_cc[VINTR] = 0;
  term.c_cc[VMIN] = 1;
  //term.c_cc[VQUIT] = 0;
  //term.c_cc[VSUSP] = 0;
  term.c_cc[VTIME] = 5;
  //term.c_cc[VSTART] = 0;
  //term.c_cc[VSTOP] = 0;
  
  /* set speed */
  switch (bps) {
  case SERIAL_300BPS: spd=B300; break;
  case SERIAL_1200BPS: spd=B1200; break;
  case SERIAL_2400BPS: spd=B2400; break;
  case SERIAL_4800BPS: spd=B4800; break;
  case SERIAL_9600BPS: spd=B9600; break;
  case SERIAL_19200BPS: spd=B19200; break;
  case SERIAL_38400BPS: spd=B38400; break;
  case SERIAL_57600BPS: spd=B57600; break;
  case SERIAL_115200BPS: spd=B115200; break;
  case SERIAL_230400BPS: spd=B230400; break;
  case SERIAL_460800BPS: spd=B460800; break;
  case SERIAL_500000BPS: spd=B500000; break;
  case SERIAL_576000BPS: spd=B576000; break;
  case SERIAL_921600BPS: spd=B921600; break;
  case SERIAL_1000000BPS: spd=B1000000; break;
  case SERIAL_1152000BPS: spd=B1152000; break;
  case SERIAL_1500000BPS: spd=B1500000; break;
  case SERIAL_2000000BPS: spd=B2000000; break;
  case SERIAL_2500000BPS: spd=B2500000; break;
  case SERIAL_3000000BPS: spd=B3000000; break;
  case SERIAL_3500000BPS: spd=B3500000; break;
  case SERIAL_4000000BPS: spd=B4000000; break;
  case SERIAL_BPS_KEEP: break;
  default: assure(0);
  }
  if (spd != SERIAL_BPS_KEEP) {
    assure(cfsetispeed (&term, spd) == 0);
    assure(cfsetospeed (&term, spd) == 0);
  }

  /* set terminal attributes */
  assure (tcsetattr(self->fd, TCSANOW, &term) == 0);
  
  return self;
}

void serial_destroy (serial_t * self) {
  if (self != NULL) {
    int i;
    i = tcsetattr (self->fd, TCSANOW, self->initial_term);
    if (i != 0) {
      /* could not restore terminal settings */
    }
    close (self->fd);
    free (self);
    self = NULL;
  }
}

int serial_has_data (serial_t * self,
		     long int timeout_sec,
		     long int timeout_usec) {
  fd_set rfds;
  struct timeval tv;
  
  FD_ZERO(&rfds);
  FD_SET(self->fd, &rfds);
  tv.tv_sec = timeout_sec;
  tv.tv_usec = timeout_usec;
  if (select(self->fd+1, &rfds, NULL, NULL, &tv) > 0) {
    return 1;
  }
  return 0;
}

int serial_getc (serial_t * self) {
  ssize_t s;
  unsigned char c;
  
  s = read(self->fd, &c, 1);
  
  if (s != 1) {
    return -1;
  }
  
  return (int) c;
}

void serial_putc (serial_t * self, unsigned char c) {
  if (write(self->fd, &c, 1) != 1) {
    /* write error */
  }
  if (self->autoflush) {
    fdatasync(self->fd);
  }
}

void serial_write (serial_t * self, unsigned char * s, unsigned int size) {
  if (write(self->fd, s, size) != size) {
    /* write error */
  }
  if (self->autoflush) {
    fdatasync(self->fd);
  }
}

void serial_puts (serial_t * self, unsigned char * s) {
  int size;
  size = strlen((char*)s);
  if (write(self->fd, s, size) != size) {
    /* write error */
  }
  if (self->autoflush) {
    fdatasync(self->fd);
  }
}

void serial_flush (serial_t * self) {
  fdatasync(self->fd);
}

void serial_discard (serial_t * self, int max) {
  int i;
  do {
    i = serial_has_data (self, 0, 0);
    if (i) {
      serial_getc(self);
      if (max > 0) max--;
      if (max == 0) i = 0; /* force a stop */
    }
  } while (i);
}

#ifdef SERIAL_TEST
int main() {
  serial_t * st;
  int a = 0, i = 0, hd;

  st = serial_init("/dev/ttyS0", SERIAL_9600BPS,
		   SERIAL_BITS_8 | SERIAL_PAR_NONE |
		   SERIAL_STOP_1 | SERIAL_FLOW_NONE |
		   SERIAL_AUTOFLUSH);
  assure(st != NULL);

  serial_puts(st, "\r\nStarting\r\n", 12);

  while (i < 10) {
    printf("loop %i\n",i++);
    hd = serial_has_data (st, 2, 0);
    if (hd == 1) {
      a = serial_getc (st);
      if (a != -1) {
	printf("read  0x%02x",a);
	if ((a>31) && (a<128)) {
	  printf(" '%c'",a);
	}
	printf("\n");
      }
      
      /* if char is uppercase, return the respective lowercase */
      /* else, return a dot (.) */
      if (( a >= 'A') && (a <= 'Z')) {
	serial_putc(st, a+0x20);
      } else {
	serial_putc(st, '.');
      }
    }
  }
  
  serial_puts(st, "\r\nDone\r\n", 8);
  
  serial_destroy(st);
  return 0;
}
#endif /* x */
