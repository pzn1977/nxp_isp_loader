#ifndef _NXP_ENCODE_H
#define _NXP_ENCODE_H

#include <stdint.h>

void nxp_uuencode_cks_reset (void);
int nxp_uuencode_cks_get (void);
int nxp_uuencode_block (int in_size, uint8_t * in, uint8_t * out);
int nxp_uudecode_block (uint8_t * in, uint8_t * out);

#endif
