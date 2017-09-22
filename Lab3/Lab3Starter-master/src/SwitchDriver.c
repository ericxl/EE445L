#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"

void SwitchInit(void){
	SYSCTL_RCGCGPIO_R |= 0x10;  // activate port B
  GPIO_PORTB_DIR_R &= ~0xFFFFFFF0;   // make in
  GPIO_PORTB_AFSEL_R &= ~0x04;// disable alt funct on PB
  GPIO_PORTB_DEN_R |= 0x04;   // enable digital I/O on PB
                              // configure PB as GPIO
  GPIO_PORTB_PCTL_R = (GPIO_PORTE_PCTL_R&0xFFFFF0FF)+0x00000000;
  GPIO_PORTB_AMSEL_R = 0;     // disable analog functionality on PB
}

uint8_t GetSwitch(void){
	return GPIO_PORTB_DATA_R & 0xF;
}
