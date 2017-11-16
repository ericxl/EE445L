#include "../inc/tm4c123gh6pm.h"
#include <stdint.h>

void Switch_Init(void){
	SYSCTL_RCGCGPIO_R |= 0x02;        // 1) activate clock for Port E

                                    // 2) no need to unlock PE0-3
  GPIO_PORTB_PCTL_R &= ~0x000F0F00; // 3) regular GPIO
  GPIO_PORTB_AMSEL_R &= ~0x0F;      // 4) disable analog function on PE0-3
                                    // 5) no pullup for external switches
  GPIO_PORTB_DIR_R &= ~0x0F;        // 5) set direction to output
  GPIO_PORTB_AFSEL_R &= ~0x0F;      // 6) regular port function
  GPIO_PORTB_DEN_R |= 0x0F;         // 7) enable digital port
	
}

int Switch_Get(void){
	return GPIO_PORTB_DATA_R & 0x07;
}
