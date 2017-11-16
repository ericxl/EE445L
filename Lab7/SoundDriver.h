#ifndef __SOUNDDRIVER_H__
#define __SOUNDDRIVER_H__

#include <stdint.h>

void SoundInit(void);

void SoundSetFrequency(uint16_t freq);

void SoundOn(void);

void SoundOff(void);
#endif

