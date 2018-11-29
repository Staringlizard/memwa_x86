#ifndef _SND_H_
#define _SND_H_

#include "if.h"

#ifdef __cplusplus
extern "C" {
#endif

void snd_init();
void snd_write(uint8_t addr, uint8_t data);
uint8_t snd_read(uint8_t addr);
void snd_run(uint32_t cc);

#ifdef __cplusplus
}
#endif
#endif
