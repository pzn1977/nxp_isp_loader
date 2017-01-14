/* Serial port layer
 *
 * $Id: serial.h,v 1.4 2007/01/26 02:21:59 pzn Exp $
 *
 * (C) 2006 - Pedro Zorzenon Neto
 *
 * To use this this you must agree with its licence terms
 * License: GNU Lesser General Public License
 *          http://www.fsf.org/licensing/licenses/lgpl.html
 */

#ifndef _SERIAL_H
#define _SERIAL_H

/* parity */
#define SERIAL_PAR_KEEP  (0x00 << 0) /* do not change parity */
#define SERIAL_PAR_NONE  (0x01 << 0)
#define SERIAL_PAR_ODD   (0x02 << 0)
#define SERIAL_PAR_EVEN  (0x03 << 0)
#define SERIAL_PAR_MARK  (0x04 << 0)
#define SERIAL_PAR_SPACE (0x05 << 0)

/* stop bits */
#define SERIAL_STOP_KEEP (0x00 << 3) /* do not change stop bits */
#define SERIAL_STOP_1    (0x01 << 3)
#define SERIAL_STOP_2    (0x02 << 3)

/* data bits */
#define SERIAL_BITS_KEEP (0x00 << 5) /* do not change data bits */
#define SERIAL_BITS_8    (0x01 << 5)
#define SERIAL_BITS_7    (0x02 << 5)
#define SERIAL_BITS_6    (0x03 << 5)
#define SERIAL_BITS_5    (0x04 << 5)

/* flow control */
#define SERIAL_FLOW_KEEP (0x00 << 8) /* do not change flow control */
#define SERIAL_FLOW_NONE (0x01 << 8)
#define SERIAL_FLOW_HARD (0x02 << 8)
#define SERIAL_FLOW_SOFT (0x03 << 8)

/* flushing */
#define SERIAL_AUTOFLUSH (0x01 << 10)

/* baud rate */
#define SERIAL_BPS_KEEP   0x00 /* do not change baud rate */
#define SERIAL_300BPS     0x01
#define SERIAL_1200BPS    0x02
#define SERIAL_2400BPS    0x03
#define SERIAL_4800BPS    0x04
#define SERIAL_9600BPS    0x05
#define SERIAL_19200BPS   0x06
#define SERIAL_38400BPS   0x07
#define SERIAL_57600BPS   0x08
#define SERIAL_115200BPS  0x09
#define SERIAL_230400BPS  0x0a
#define SERIAL_460800BPS  0x0b
#define SERIAL_500000BPS  0x0c
#define SERIAL_576000BPS  0x0d
#define SERIAL_921600BPS  0x0e
#define SERIAL_1000000BPS 0x0f
#define SERIAL_1152000BPS 0x10
#define SERIAL_1500000BPS 0x11
#define SERIAL_2000000BPS 0x12
#define SERIAL_2500000BPS 0x13
#define SERIAL_3000000BPS 0x14
#define SERIAL_3500000BPS 0x15
#define SERIAL_4000000BPS 0x16

struct serial {
  int fd;
  int autoflush;
  struct termios * initial_term;
};

#define serial_t struct serial

/* TODO: the following function has many asserts in case of problems.
 * need to change these asserts. */
serial_t * serial_init (char * path, unsigned int bps, unsigned int mode);

void serial_destroy (serial_t * self);

/* return 1 if there is at least one char on the receive buffer.
   if there is no char at the receive buffer, waits for it to arrive
   (for max timeout), and return 1 if received, or 0 if no char is
   in input buffer
   use timeout_sec=0, timeout_usec=0 if you just want to check
   buffer, but do not want to wait anytime. */
int serial_has_data (serial_t * self,
		     long int timeout_sec,
		     long int timeout_usec);

/* will block program and wait character, use serial_has_data before
   serial_getc if you do not want to block program */
int serial_getc (serial_t * self);

/* send a char to serial port */
void serial_putc (serial_t * self, unsigned char c);

/* send many chars to serial port */
void serial_write (serial_t * self, unsigned char * s, unsigned int size);

/* send a string to serial port, does not send \0 at the end */
void serial_puts (serial_t * self, unsigned char * s);

/* output flush (force buffered bytes to be transmitted now) */
void serial_flush (serial_t * self);

void serial_discard (serial_t * self, int max);

#endif
