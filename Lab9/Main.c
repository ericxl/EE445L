// Main.c
// Runs on LM4F120/TM4C123
// Temperature Data Acquisition System
// April 11, 2016

/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2015
  Program 7.5, example 7.6

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

/**
 * Three ways to sample using an ADC conversion :
 * 1. Flash
 * 2. Sigma delta
 * 3. Successive approximation
 *
 * To initiate an ADC conversion process, we could:
 * 1. Rising/falling edges or timer values
 * 2. Signal period trigger - Time difference between two edges of both rising or both falling edge
 * 3. Signal pulse trigger - Time difference between two edges of opposite edges
 *
 * We check to see if we've begun sampling by looking at the
 * CaptureA Event Raw Interrupt Status (CAERIS) interrupt flag.
 * This is triggered at every interrupt for sampling.
 *
 * To see if we've finished ADC conversion we check the ADC0_RIS_R memory address.
 */
 
#include <stdio.h>
#include <stdint.h>

#include "ADCTimerTrigger.h"
#include "ST7735.h"
#include "UART.h"
#include "PLL.h"
#include "calib.h"

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode

int main(void){ 
	DisableInterrupts();
	PLL_Init(Bus80MHz);	// bus clock at 80 MHz
	UART_Init();
	ST7735_InitR(INITR_REDTAB);
	ADC0_InitTimerTriggerSeq3_Ch0();
		
	EnableInterrupts();
	
	while(1) {
		ST7735_SetCursor(0, 0);
		int32_t adc = ADC_FIFO_CurrentValue();
		int32_t currentTemperature = get_temp(adc);
		printf("Temperature %5d.%02d\n", currentTemperature / 100, currentTemperature % 100);
		
		printf("ADC %d\n", adc);
		if (adc > 2450){
			printf("Circuit is open");
		}
		else if (adc < 400){
			printf("Circuit is shorted");
		}
		
		ST7735_PlotClear(32, 159);

		
		for(int i = 0; i < FIFO_SIZE; i += 1) {
			int32_t point = 128 - get_temp(ADC_FIFO_Get()[i]) * 2 / 100;
			ST7735_DrawPixel(i, point, ST7735_RED);
		}
		WaitForInterrupt();
	}
}
