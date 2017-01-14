#ifndef _FILEREAD_H
#define _FILEREAD_H

#include <stdint.h>

int fileread_open (char * fname);
uint8_t * fileread_getpage512 ();
int fileread_getpage512_r (uint8_t * data);
void fileread_rewind ();

#endif
