
#include "..//inc//tm4c123gh6pm.h"
#include "Switch.h"
#include <stdint.h>

void Increase_Speed(void);
void Decrease_Speed(void);



void Switch_Init() {
	volatile unsigned long delay;
	//enable portE0-1 for botton clicked
	
	SYSCTL_RCGCGPIO_R |= 0x10; 
	while((SYSCTL_PRGPIO_R&0x10) == 0) {
	} 
	
	GPIO_PORTE_DEN_R |= 0x03;
	GPIO_PORTE_DIR_R &= ~0x03;
	/*disable analog*/
	GPIO_PORTE_AMSEL_R &= ~0x03;
	GPIO_PORTE_AFSEL_R &= ~0x03;
	/*set to pull down interface*/
	GPIO_PORTE_PDR_R |= 0x03;
	
	/*PE0-1 are edge sensitive*/
	GPIO_PORTE_IS_R &= ~0x03;
	GPIO_PORTE_IBE_R &= ~0x03;
	GPIO_PORTE_IEV_R &= ~0x03;
	
	/*clear the interrupt*/
	GPIO_PORTE_ICR_R = 0x03;
	
	/*enable*/
	GPIO_PORTE_IM_R |= 0x03;
	
	
	NVIC_PRI0_R = (NVIC_PRI1_R&0xFFFFFF00)|0x00000040; //PoRT E = priority 2
	NVIC_EN0_R |= 0x00000010;// enable interrupt 3 in NVIC
}


void GPIOPortE_Handler(void) {
	

	if(GPIO_PORTE_RIS_R&0x01) {
		GPIO_PORTE_ICR_R = 0x01;
		Increase_Speed();
	}

	if(GPIO_PORTE_RIS_R&0x02) {
		GPIO_PORTE_ICR_R = 0x02;
		Decrease_Speed();
	}
}	
