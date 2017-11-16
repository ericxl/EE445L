#include "../inc/tm4c123gh6pm.h"
#include "PWM.h"

#include "SoundDriver.h"

uint8_t soun;

void SoundInit(void){
	PWM0A_Init(40000, 10000);         			// initialize PWM0, 1000 Hz, 25% duty
	PWM0_ENABLE_R &= ~0x01;
}

void SoundSetFrequency(uint16_t freq){
	PWM0_0_LOAD_R = freq - 1;           // 5) cycles needed to count down to 0
  PWM0_0_CMPA_R = (freq / 2) - 1;             // 6) count value when output rises
}

void SoundOn(void){
	PWM0_ENABLE_R |= 0x01;
}

void SoundOff(void){
	PWM0_ENABLE_R &= ~0x01;
}
