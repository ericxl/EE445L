#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"

uint8_t soun;

void SoundInit(void){
	SYSCTL_RCGCGPIO_R |= 0x10;  // activate port E
  GPIO_PORTE_DIR_R |= 0x04;   // make PE2 out
  GPIO_PORTE_AFSEL_R &= ~0x04;// disable alt funct on PE2
  GPIO_PORTE_DEN_R |= 0x04;   // enable digital I/O on PE2
                              // configure PE2 as GPIO
  GPIO_PORTE_PCTL_R = (GPIO_PORTE_PCTL_R&0xFFFFF0FF)+0x00000000;
  GPIO_PORTE_AMSEL_R = 0;     // disable analog functionality on PE
}

void SoundOn(void){
	soun = 1;
}

void SoundOff(void){
	soun = 0;
}
