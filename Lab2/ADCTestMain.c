// ADCTestMain.c
// Runs on TM4C123
// This program periodically samples ADC channel 0 and stores the
// result to a global variable that can be accessed with the JTAG
// debugger and viewed with the variable watch feature.
// Daniel Valvano
// September 5, 2015
// Repurposed by: Paul Heath, Eric Liang 9/20/2017
// Last Updated: 9/22/2017

/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2015

 Copyright 2015 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */
 
#include <stdint.h>
#include <stdio.h>
#include "ADCSWTrigger.h"
#include "../inc/tm4c123gh6pm.h"
#include "PLL.h"
#include "Timer1.h"
#include "Timer2.h"
#include "ST7735.h"

#define PF2             (*((volatile uint32_t *)0x40025010))
#define PF1             (*((volatile uint32_t *)0x40025008))
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode

volatile uint32_t TimeDump[1000];
volatile uint32_t DataDump[1000];
volatile uint32_t TimeIdx = 0;
volatile uint32_t DataIdx = 0;
volatile uint32_t TimeDiffs[999];
volatile uint32_t NumofOccur[4096];

volatile uint32_t ADCvalue;
volatile uint32_t Jitter;
volatile uint32_t MaxY;



// This debug function initializes Timer0A to request interrupts
// at a 100 Hz frequency.  It is similar to FreqMeasure.c.
void Timer0A_Init100HzInt(void){
  volatile uint32_t delay;
  DisableInterrupts();
  // **** general initialization ****
  SYSCTL_RCGCTIMER_R |= 0x01;      // activate timer0
  delay = SYSCTL_RCGCTIMER_R;      // allow time to finish activating
  TIMER0_CTL_R &= ~TIMER_CTL_TAEN; // disable timer0A during setup
  TIMER0_CFG_R = 0;                // configure for 32-bit timer mode
  // **** timer0A initialization ****
                                   // configure for periodic mode
  TIMER0_TAMR_R = TIMER_TAMR_TAMR_PERIOD;
  TIMER0_TAILR_R = 799999;         // start value for 100 Hz interrupts
  TIMER0_IMR_R |= TIMER_IMR_TATOIM;// enable timeout (rollover) interrupt
  TIMER0_ICR_R = TIMER_ICR_TATOCINT;// clear timer0A timeout flag
  TIMER0_CTL_R |= TIMER_CTL_TAEN;  // enable timer0A 32-b, periodic, interrupts
  // **** interrupt initialization ****
                                   // Timer0A=priority 2
  NVIC_PRI4_R = (NVIC_PRI4_R&0x00FFFFFF)|0x40000000; // top 3 bits
  NVIC_EN0_R = 1<<19;              // enable interrupt 19 in NVIC
}



void Timer0A_Handler(void){
  TIMER0_ICR_R = TIMER_ICR_TATOCINT;    // acknowledge timer0A timeout
  PF2 ^= 0x04;                   // profile
  PF2 ^= 0x04;                   // profile
  ADCvalue = ADC0_InSeq3();
	// put time and data to dump
	if(DataIdx < 1000){
	  DataDump[DataIdx] = ADCvalue;
	  ++DataIdx;
	}
	if(TimeIdx < 1000){	
	  TimeDump[TimeIdx] = Timer1_Val();
	  ++TimeIdx;
	}
  PF2 ^= 0x04;                   // profile
}



// This function calculates the jitter of the ADC sampling.
// It also sets the bins for a histogram of the ADC data to the correct values.
void calculate(void){
	uint32_t min = TimeDump[0] - TimeDump[1];   // pick first delta time value as min and max
	uint32_t max = TimeDump[0] - TimeDump[1];
  for(uint32_t i = 0; i < 999; ++i){
		TimeDiffs[i] = TimeDump[i] - TimeDump[i + 1];
		if(i < 5){
			max = TimeDiffs[i];
			min = TimeDiffs[i];
		}
		if(TimeDiffs[i] > max){            // get max of delta time values
			max = TimeDiffs[i];
		}
		if(TimeDiffs[i] < min){            // get min of delta time values
			min = TimeDiffs[i];
		}
	}
	Jitter = max - min;             
	for(uint32_t i = 0; i < 1000; ++i){
		++NumofOccur[DataDump[i]];
	}
	MaxY = NumofOccur[0];
	for(uint32_t i = 0; i < 4096; ++i){
		if(NumofOccur[i] > MaxY){
			MaxY = NumofOccur[i];
		}
		if(NumofOccur[i] < 2){
			NumofOccur[i] = 0;
		}
	}
}


//Recursive Helper to Plot Jitter Digits in Correct Order
void putClockToNanoSHelper(uint32_t clockVal){
	if(clockVal == 0){
		return;
	}
	uint32_t digitToPrint = clockVal % 10;
	clockVal /= 10;
	putClockToNanoSHelper(clockVal);
	fputc(digitToPrint + 48, (FILE*) 3);
}


//Puts Jitter Clock Value to Screen in Nano Seconds
void putClockToNanoS(uint32_t clockVal){
	if(clockVal == 0){
		fputc(48, (FILE*) 3);
	}
	else{
	  clockVal *= (12500 / 1000);
    putClockToNanoSHelper(clockVal);
	}
	fputc(' ', (FILE*) 3);
	fputc('n', (FILE*) 3);
	fputc('s', (FILE*) 3);
}


//Plots Histogram of PMF data 
void plotHisto(void){
	uint32_t first = 0;
	uint32_t last = 0;
	uint32_t middle = 0;
	uint8_t firstFound = 0;
	// find middle of histogram
	for(uint32_t i = 0; i < 4096; ++i){
		if(!firstFound){
			if(NumofOccur[i] != 0){
				firstFound = 1;
				first = i;
				last = i;
			}
		}
		else{
			if(NumofOccur[i] != 0){
				last = i;
			}
		}
	}
	middle = first + (last - first) / 2 ;
	// assumes values land within 64 bins of the middle value
	for(uint32_t i = middle - 64; i < (middle + 64); ++i){
		ST7735_PlotBar(NumofOccur[i]);
		ST7735_PlotNext();
	}	
}



int main(void){
	char collectStr[] = {'C', 'o', 'l', 'l', 'e', 'c', 't', 'i', 'n', 'g', ' ', 'D', 'a', 't', 'a', '.', '.', '.', 0};
	char jitterStr[] = {'J', 'i', 't', 't', 'e', 'r', ':', ' ', 0};
  PLL_Init(Bus80MHz);                   // 80 MHz
  SYSCTL_RCGCGPIO_R |= 0x20;            // activate port F
  ADC0_InitSWTriggerSeq3_Ch9();         // allow time to finish activating
  Timer0A_Init100HzInt();               // set up Timer0A for 100 Hz interrupts
  GPIO_PORTF_DIR_R |= 0x06;             // make PF2, PF1 out (built-in LED)
  GPIO_PORTF_AFSEL_R &= ~0x06;          // disable alt funct on PF2, PF1
  GPIO_PORTF_DEN_R |= 0x06;             // enable digital I/O on PF2, PF1
                                        // configure PF2 as GPIO
  GPIO_PORTF_PCTL_R = (GPIO_PORTF_PCTL_R&0xFFFFF00F)+0x00000000;
  GPIO_PORTF_AMSEL_R = 0;               // disable analog functionality on PF
  PF2 = 0;                              // turn off LED
	Timer1_Init();
	Timer2_Init(791999);
	ST7735_InitR(INITR_REDTAB);
	ST7735_FillScreen(ST7735_BLACK);
	ST7735_OutString(collectStr);
  EnableInterrupts();
  while(1){
		PF1 ^= 0x02;
		if(DataIdx >= 1000 || TimeIdx >= 1000){        // stop sampling after 1000 samples
			DisableInterrupts();
			calculate();                                 // calculate jitter and histogram data
			ST7735_FillScreen(ST7735_BLACK);
			ST7735_SetCursor(0, 0);
			ST7735_OutString(jitterStr);
			putClockToNanoS(Jitter);
			ST7735_PlotClear(0, MaxY);
			plotHisto();
			while(1){};
		}
  }
}


