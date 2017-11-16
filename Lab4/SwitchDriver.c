#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "SwitchDriver.h"

#define NVIC_EN0_INT4           0x00000010  // Interrupt 4 enable

void (*Button1Handler)(void);   // user function
void (*Button2Handler)(void);   // user function
void (*Button3Handler)(void);   // user function
void (*Button4Handler)(void);   // user function

void SwitchInit(void){
	SYSCTL_RCGCGPIO_R |= 0x02;        // 1) activate clock for Port E

                                    // 2) no need to unlock PE0-3
  GPIO_PORTB_PCTL_R &= ~0x000F0F00; // 3) regular GPIO
  GPIO_PORTB_AMSEL_R &= ~0x0F;      // 4) disable analog function on PE0-3
                                    // 5) no pullup for external switches
  GPIO_PORTB_DIR_R &= ~0x0F;        // 5) set direction to output
  GPIO_PORTB_AFSEL_R &= ~0x0F;      // 6) regular port function
  GPIO_PORTB_DEN_R |= 0x0F;         // 7) enable digital port
	
  SYSCTL_RCGCGPIO_R |= 0x20;        // 1) activate clock for Port F
  while((SYSCTL_PRGPIO_R&0x20)==0){}; // allow time for clock to start
                                    // 2) no need to unlock PF2, PF4
  GPIO_PORTF_PCTL_R &= ~0x000F0F0F; // 3) regular GPIO
  GPIO_PORTF_AMSEL_R &= ~0x11;      // 4) disable analog function on PF2, PF4
  GPIO_PORTF_PUR_R |= 0x11;         // 5) pullup for PF4 PF0
  GPIO_PORTF_DIR_R |= 0x0E;         // 5) set direction to output
  GPIO_PORTF_AFSEL_R &= ~0x11;      // 6) regular port function
  GPIO_PORTF_DEN_R |= 0x11;         // 7) enable digital port
}

void Heartbeat_Init(void) {
	volatile uint32_t delay;
	SYSCTL_RCGCGPIO_R |= 0x20;            // activate port F
  delay = SYSCTL_RCGCGPIO_R;						// delay for port activation
	GPIO_PORTF_DIR_R |= 0x06;             // make PF2, PF1 out (built-in LED)
  GPIO_PORTF_AFSEL_R &= ~0x06;          // disable alt funct on PF2, PF1
  GPIO_PORTF_DEN_R |= 0x06;             // enable digital I/O on PF2, PF1
                                        // configure PF2 as GPIO
  GPIO_PORTF_PCTL_R = (GPIO_PORTF_PCTL_R&0xFFFFF00F)+0x00000000;
  GPIO_PORTF_AMSEL_R = 0;               // disable analog functionality on PF
	
	PF2 &= ~0x04;
}

uint8_t GetSwitch(void){
	return GPIO_PORTB_DATA_R;
}

volatile uint8_t SW1, SW2, SW3, SW4;

void ButtonsInit(void(*task1)(void),void(*task2)(void),void(*task3)(void),void(*task4)(void)){
	SYSCTL_RCGCGPIO_R |= 0x10;        // 1) activate clock for Port E
	
	SW1 = 0;                    // clear semaphores
  SW2 = 0;
	SW3 = 0;                    // clear semaphores
  SW4 = 0;
	Button1Handler = task1;
	Button2Handler = task2;
	Button3Handler = task3;
	Button4Handler = task4;
  GPIO_PORTE_AMSEL_R &= ~0x3C;// disable analog function on PE5-4
  GPIO_PORTE_PCTL_R &= ~0x00FFFF00; // configure PE5-4 as GPIO 
  GPIO_PORTE_DIR_R &= ~0x0F;  // make PE5-4 in 
  GPIO_PORTE_AFSEL_R &= ~0x3C;// disable alt funct on PE5-4 
  GPIO_PORTE_DEN_R |= 0x3C;   // enable digital I/O on PE5-4
  GPIO_PORTE_IS_R &= ~0x3C;   // PE5-4 is edge-sensitive 
  GPIO_PORTE_IBE_R &= ~0x3C;  // PE5-4 is not both edges 
  GPIO_PORTE_IEV_R |= 0x3C;   // PE5-4 rising edge event
  GPIO_PORTE_ICR_R = 0x3C;    // clear flag5-4
  GPIO_PORTE_IM_R |= 0x3C;    // enable interrupt on PE5-4
                              // GPIO PortE=priority 2
  NVIC_PRI1_R = (NVIC_PRI1_R&0xFFFFFF00)|0x00000080; // bits 5-7
  NVIC_EN0_R = NVIC_EN0_INT4; // enable interrupt 4 in NVIC
}

void GPIOPortE_Handler(void){
  if(GPIO_PORTE_RIS_R&0x04){  // poll PE4
    GPIO_PORTE_ICR_R = 0x04;  // acknowledge flag4
    SW1 = 1;                  // signal SW1 occurred
		Button1Handler();
  }
  if(GPIO_PORTE_RIS_R&0x08){  // poll PE5
    GPIO_PORTE_ICR_R = 0x08;  // acknowledge flag5
    SW2 = 1;                  // signal SW2 occurred
		Button2Handler();
  }
	if(GPIO_PORTE_RIS_R&0x10){  // poll PE4
    GPIO_PORTE_ICR_R = 0x10;  // acknowledge flag4
    SW3 = 1;                  // signal SW3 occurred
		Button3Handler();
  }
  if(GPIO_PORTE_RIS_R&0x20){  // poll PE5
    GPIO_PORTE_ICR_R = 0x20;  // acknowledge flag5
    SW4 = 1;                  // signal SW4 occurred
		Button4Handler();
  }
}

