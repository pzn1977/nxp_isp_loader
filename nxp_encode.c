#include <stdlib.h>
#include <string.h>
#include "nxp_encode.h"

/* NXP Application Note: AN11229 ISP UUencode */

static uint16_t cks = 0;

void nxp_uuencode_cks_reset (void) {
  cks = 0;
}

int nxp_uuencode_cks_get (void) {
  return cks;
}

static void enc3to4 (uint8_t * in, uint8_t * out) {
  cks += in[0];
  cks += in[1];
  cks += in[2];
  out[0] = (in[0] >> 2) + 32;
  out[1] = (((in[0] & 0x03) << 4) | (in[1] >> 4)) + 32;
  out[2] = (((in[1] & 0x0f) << 2) | (in[2] >> 6)) + 32;
  out[3] = (in[2] & 0x3f) + 32;
  if (out[0] == 0x20) out[0] = 0x60;
  if (out[1] == 0x20) out[1] = 0x60;
  if (out[2] == 0x20) out[2] = 0x60;
  if (out[3] == 0x20) out[3] = 0x60;
}

static void dec4to3 (uint8_t * in, uint8_t * out) {
  uint8_t a,b,c,d;
  a = in[0] - 32;
  b = in[1] - 32;
  c = in[2] - 32;
  d = in[3] - 32;
  out[0] = (a << 2) | (b >> 4);
  out[1] = (b << 4) | (c >> 2);
  out[2] = (c << 6) | d;
}

int nxp_uuencode_block (int in_size, uint8_t * in, uint8_t * out) {
  int i, j;

  if (in_size > 45) return 0;
  if (in_size < 1) return 0;

  out[0] = in_size + 32;

  for (i=0, j=1; i<in_size; i+=3, j+=4) {
    if (i+3 <= in_size) {
      enc3to4(in+i,out+j);
    } else {
      uint8_t b[3];
      b[0] = in[i];
      if (i+1 < in_size) b[1] = in[i+1]; else b[1] = 0;
      b[2] = 0;
      enc3to4(b,out+j);
    }
  }
  out[j++] = '\r';
  out[j] = 0;

  return 1;
}

int nxp_uudecode_block (uint8_t * in, uint8_t * out) {
  int i, v, j, len;

  if ((in[0] < '!') || (in[0] > 'M')) return 0; /* underflow or overflow */

  len = strlen((char*)in);
  for (i=0; i<len; i++) {
    if (in[i] > 0x60) return 0; /* invalid char */
    if ((in[i] < 0x21) &&
	(in[i] != 0x0d) &&
	(in[i] != 0x0a)) return 0; /* invalid char */
    if (in[i] == 0x60) in[i] = 0x20;
    if (in[i] == 0x0d) in[i] = 0; /* CR */
    if (in[i] == 0x0a) in[i] = 0; /* LF */
  }
  len = strlen((char*)in);

  v = (((len-1)/4) * 3) - (in[0] - 0x20);
  if ((v < 0) || (v > 2)) return 0; /* wrong encoding size */

  for (i=1, j=0; i<len; i+=4, j+=3) {
    dec4to3 (in+i, out+j);
  }

  return in[0] - 0x20;
}
