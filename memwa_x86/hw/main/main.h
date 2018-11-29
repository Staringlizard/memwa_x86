#ifndef _MAIN_H_
#define _MAIN_H_

#include "hostif.h"

/* This must ALWAYS be uppdated for every release */
#define FW_REVISION             "V1.0.0"

#define CC_BROM_LOAD_ADDR		0x0000A000
#define CC_CROM_LOAD_ADDR		0x0000D000
#define CC_KROM_LOAD_ADDR		0x0000E000

#define DD_ALL_BASE_ADDR		0x00010000
#define DD_UTIL1_BASE_ADDR		0x00011000
#define DD_UTIL2_BASE_ADDR		0x00012000

#define DD_DOS_LOAD_ADDR       0x0000C000

void main_open_file();

#endif
