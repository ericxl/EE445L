// dac.c
// This software configures DAC output
// Runs on LM4F120 or TM4C123
// Program written by: put your names here
// Date Created: 8/25/2014 
// Last Modified: 3/6/2015 
// Section 1-2pm     TA: Wooseok Lee
// Lab number: 6
// Hardware connections
// TO STUDENTS "REMOVE THIS LINE AND SPECIFY YOUR HARDWARE********

#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
// Code files contain the actual implemenation for public functions
// this file also contains an private functions and private data

const char SineWave[64] = {32,35,38,41,44,47,49,52,54,56,58,59,61,62,62,63,63,63,62,62,61,59,58,56,54,52,49,47,44,41,38,35,32,29,26,23,20,17,15,12,10,8,6,5,3,2,2,1,1,1,2,2,3,5,6,8,10,12,15,17,20,23,26,29};
char Index = 0;
// **************DAC_Init*********************
// Initialize 4-bit DAC, called once 
// Input: none
// Output: none
void DAC_Init(void){
	
	int delay;

  SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOB; // activate port B

  delay = SYSCTL_RCGC2_R;    // allow time to finish activating

  GPIO_PORTB_AMSEL_R &= ~0x3F;      // no analog

  GPIO_PORTB_PCTL_R &= ~0x00000FFF; // regular GPIO function

  GPIO_PORTB_DIR_R |= 0x3F;      // make PB3-0 out

  GPIO_PORTB_AFSEL_R &= ~0x3F;   // disable alt funct on PB2-0

  GPIO_PORTB_DEN_R |= 0x3F;      // enable digital I/O on PB2-0
	
	
	
	
	SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOF; // activate port F

  delay = SYSCTL_RCGC2_R;    // allow time to finish activating

  GPIO_PORTF_DIR_R |= 0x04;     // make Pf2 out
	GPIO_PORTF_LOCK_R = 0x4C4F434B;
	GPIO_PORTF_DIR_R &= ~0x01;  //portf1 int
	GPIO_PORTF_CR_R = 0x05;
	GPIO_PORTF_AMSEL_R = 0x00;
	GPIO_PORTF_PCTL_R = 0x00000000;
	GPIO_PORTF_AFSEL_R = 0x00;

  GPIO_PORTF_DEN_R |= 0x05;   // enable digital I/O on Pf2-0
	
	GPIO_PORTF_PUR_R |= 0x01;

}

// **************DAC_Out*********************
// output to DAC
// Input: 4-bit data, 0 to 15 
// Output: none
void DAC_Out(int data){

  GPIO_PORTB_DATA_R = data;

}

// the sound frequency will be (interrupt frequency)/(size of the table)
void SysTick_Handler(void){
  //GPIO_PORTF_DATA_R |= 0x04;
	
	Index = (Index+1)&0x3F;      // 4,5,6,7,7,7,6,5,4,3,2,1,1,1,2,3,... 

  DAC_Out(SineWave[Index]);    // output one value each interrupt
	
	//GPIO_PORTF_DATA_R &= ~0x04;

}
