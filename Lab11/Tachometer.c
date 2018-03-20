#include <stdint.h>
#include "PWM.h"
#include <stdio.h>
#include "ST7735.h"
#include "..//inc//tm4c123gh6pm.h"

#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"


#define NVIC_EN0_INT19          0x00080000  // Interrupt 19 enable
#define TIMER_TAILR_TAILRL_M    0x0000FFFF  // GPTM TimerA Interval Load
                                            // Register Low
																						
#define PF2                     (*((volatile uint32_t *)0x40025010))
																						

uint32_t Period;              // (1/clock) units
uint32_t First;               // Timer0A first edge
int32_t Done;                 // set each rising
uint32_t Speed;
uint32_t Target_Speed;
void Tach_Init(void){
	SYSCTL_RCGCTIMER_R |= 0x01;// activate timer0    
  SYSCTL_RCGCGPIO_R |= 0x22;       // activate port B and port F
                                   // allow time to finish activating
  First = 0;                       // first will be wrong
  Done = 0;                        // set on subsequent
  GPIO_PORTB_DIR_R &= ~0x40;       // make PB6 in
  GPIO_PORTB_AFSEL_R |= 0x40;      // enable alt funct on PB6/T0CCP0
  GPIO_PORTB_DEN_R |= 0x40;        // enable digital I/O on PB6
                                   // configure PB6 as T0CCP0
  GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R&0xF0FFFFFF)+0x07000000;
  GPIO_PORTB_AMSEL_R &= ~0x40;     // disable analog functionality on PB6
  GPIO_PORTF_DIR_R |= 0x04;        // make PF2 out (PF2 built-in blue LED)
  GPIO_PORTF_AFSEL_R &= ~0x04;     // disable alt funct on PF2
  GPIO_PORTF_DEN_R |= 0x04;        // enable digital I/O on PF2
                                   // configure PF2 as GPIO
  GPIO_PORTF_PCTL_R = (GPIO_PORTF_PCTL_R&0xFFFFF0FF)+0x00000000;
  GPIO_PORTF_AMSEL_R = 0;          // disable analog functionality on PF
  TIMER0_CTL_R &= ~TIMER_CTL_TAEN; // disable timer0A during setup
  TIMER0_CFG_R = TIMER_CFG_16_BIT; // configure for 16-bit timer mode
                                   // configure for 24-bit capture mode
  TIMER0_TAMR_R = (TIMER_TAMR_TACMR|TIMER_TAMR_TAMR_CAP);
                                   // configure for rising edge event
  TIMER0_CTL_R &= ~(TIMER_CTL_TAEVENT_POS|0xC);
  TIMER0_TAILR_R = TIMER_TAILR_TAILRL_M;// start value
  TIMER0_TAPR_R = 0xFF;            // activate prescale, creating 24-bit
  TIMER0_IMR_R |= TIMER_IMR_CAEIM; // enable capture match interrupt
  TIMER0_ICR_R = TIMER_ICR_CAECINT;// clear timer0A capture match flag
  TIMER0_CTL_R |= TIMER_CTL_TAEN;  // enable timer0A 16-b, +edge timing, interrupts
                                   // Timer0A=priority 2
  NVIC_PRI4_R = (NVIC_PRI4_R&0x00FFFFFF)|0x40000000; // top 3 bits
  NVIC_EN0_R = NVIC_EN0_INT19;     // enable interrupt 19 in NVIC
}

void tach_update(){
	TIMER0_ICR_R = 0x00000004;
	
	
  Period = (First - TIMER0_TAR_R)&0x00FFFFFF;

  First = TIMER0_TAR_R;
	
	Speed = 200000000/Period;
	
		uint32_t E = Target_Speed-Speed;
		uint32_t U = (400*E)/64;
		PWM0B_Duty(U);

}

uint32_t Tach_GetSpeed() {
	return 80000000/Period;
}
