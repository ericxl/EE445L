/**
 * Set up timers.
 */

#ifndef __TIMERSINTS_H__ // do not include more than once
#define __TIMERSINTS_H__

#include <stdint.h>

void Timer0A_Init(void(*task)(void), uint32_t period);
void Timer1_Init(void(*task)(void), uint32_t period);

#endif // __TIMERSINTS_H__
