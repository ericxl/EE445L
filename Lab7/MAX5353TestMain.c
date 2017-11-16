
// MAX5353TestMain.c
// Runs on LM4F120/TM4C123
// Test the functions provided in MAX5353.c by outputting
// a sine wave at a particular frequency.
// Daniel Valvano
// September 11, 2013

/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2014

 Copyright 2014 by Jonathan W. Valvano, valvano@mail.utexas.edu
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

// SSI0Clk (SCLK, pin 4) connected to PA2
// SSI0Fss (!CS, pin 2) connected to PA3
// SSI0Tx (DIN, pin 3) connected to PA5
// see Figure 7.19 for complete schematic

#include "../inc/tm4c123gh6pm.h"
#include <stdint.h>
#include "PLL.h"
#include "Dac.h"
#include "SysTick.h"
#include "Timer0A.h"
#include "Switch.h"
#include "ST7735.h"


#define C7 2093.00
#define B6 1975.53
#define Bb6 1864.66
#define A6 1760.00
#define Ab6 1661.22
#define G6 1567.98
#define Gb6 1479.98
#define F6 1396.91
#define E6 1318.51
#define Eb6 1244.51
#define D6 1174.66
#define Db6 1108.73
#define C6 1046.50
#define B5 987.77
#define Bb5 932.33
#define A5 880.00
#define Ab5 830.61
#define G5 783.99
#define Gb5 739.99
#define F5 698.46
#define E5 659.25
#define Eb5 622.25
#define D5 587.33
#define Db5 554.37
#define C5 523.25
#define B4 493.88
#define Bb4 466.16
#define A4 440.00
#define Ab4 415.30
#define G4 392.00
#define Gb4 369.99
#define F4 349.23
#define E4 329.63
#define Eb4 311.13
#define D4 293.66
#define Db4 277.18
#define C4 261.63 // Middle C
#define B3 246.94
#define Bb3 233.08
#define A3 220.00
#define Ab3 207.65
#define G3 196.00
#define Gb3 185.00
#define F3 174.61
#define E3 164.81
#define Eb3 155.56
#define D3 146.83
#define Db3 138.59
#define C3 130.81
#define B2 123.47
#define Bb2 116.54
#define A2 110.00
#define Ab2 103.83
#define G2 98.00
#define Gb2 92.50
#define F2 87.31
#define E2 82.41
#define Eb2 77.78
#define D2 73.42
#define Db2 69.30
#define C2 65.41

#define BK 1

struct Song {
  float Note; 
  unsigned long Time;
}; 
typedef const struct Song Sng;


#define throneLength 88
Sng throne[throneLength]={
 {G6,2},
 {C6,2},
 {Eb6,1},
 {F6,1},
 
 {G6,2},
 {C6,2},
 {Eb6,1},
 {F6,1},
 
 {G6,2},
 {C6,2},
 {Eb6,1},
 {F6,1},
 
 {G6,2},
 {C6,2},
 {E6,1},
 {F6,1},
 
 {G6,2},
 {C6,2},
 {E6,1},
 {F6,1},
 
 {G6,2},
 {C6,2},
 {E6,1},
 {F6,1},
 
 {G6,2},
 {C6,2},
 {E6,1},
 {F6,1},
 
 {G6,2},
 {C6,2},
 {E6,1},
 {F6,1},
 
 {G6,6},
 
 {C6,6},
 
 {Eb6,1},
 {F6,1},
 {G6,4},
 
 {C6,4},
 {Eb6,1},
 {F6,1},
 
 {D6,2},
 {G5,2},
 {Bb5,1},
 {C6,1},
 
 {D6,2},
 {G5,2},
 {Bb5,1},
 {C6,1},
 
 {D6,2},
 {G5,2},
 {Bb5,1},
 {C6,1},
 
 {D6,2},
 {G5,2},
 {Bb5,1},
 {D6,1},
 
 {F6,6},
 
 {Bb5,6},
 
 {Eb6,1},
 {D6,1},
 {F6,4},
 
 {Bb5,4},
 {Eb6,1},
 {D6,1},
 
 {C6,2},
 {F5,2},
 {Ab5,1},
 {Bb5,1},
 
 {C6,2},
 {F5,2},
 {Ab5,1},
 {Bb5,1},
 
 {C6,2},
 {F5,2},
 {Ab5,1},
 {Bb5,1},
 
 {C6,2},
 {F5,2},
 {Ab5,1},
 {Bb5,1},
 
 {C7,12},
 
 {Bb6,12},
 
 {C6,12},
 
 {G6,12},
 
 {Ab5,12},
 
 {Eb6,6},
	 
 {D6,6},
 
 {C6,24}
};

#define throne2Length 208
Sng throne2[throne2Length]={
 {C4,1},
 {BK,1},
 {C4,1},
 {BK,1},
 {C4,1},
 {BK,1},
 
 {C4,1},
 {BK,1},
 {C4,1},
 {BK,1},
 {C4,1},
 {BK,1},
 
 {C4,1},
 {BK,1},
 {C4,1},
 {BK,1},
 {C4,1},
 {BK,1},
 
 {C4,1},
 {BK,1},
 {C4,1},
 {BK,1},
 {C4,1},
 {BK,1},
 
 {C4,1},
 {BK,1},
 {C4,1},
 {BK,1},
 {C4,1},
 {BK,1},
 
 {C4,1},
 {BK,1},
 {C4,1},
 {BK,1},
 {C4,1},
 {BK,1},
 
 {C4,1},
 {BK,1},
 {C4,1},
 {BK,1},
 {C4,1},
 {BK,1},
 
 {C4,1},
 {BK,1},
 {C4,1},
 {BK,1},
 {C4,1},
 {BK,1},
 
 //first period
 
 {C4,1},
 {BK,1},
 {C4,1},
 {BK,1},
 {C4,1},
 {BK,1},
 
 {C4,1},
 {BK,1},
 {C4,1},
 {BK,1},
 {C4,1},
 {BK,1},
 
 {C4,1},
 {BK,1},
 {C4,1},
 {BK,1},
 {C4,1},
 {BK,1},
 
 {C4,1},
 {BK,1},
 {C4,1},
 {BK,1},
 {C4,1},
 {BK,1},
 
 {G3,1},
 {BK,1},
 {G3,1},
 {BK,1},
 {G3,1},
 {BK,1},
 
 {G3,1},
 {BK,1},
 {G3,1},
 {BK,1},
 {G3,1},
 {BK,1},
 
 {G3,1},
 {BK,1},
 {G3,1},
 {BK,1},
 {G3,1},
 {BK,1},
 
 {G3,1},
 {BK,1},
 {G3,1},
 {BK,1},
 {G3,1},
 {BK,1},
 
 //second period
 
 {Bb3,1},
 {BK,1},
 {Bb3,1},
 {BK,1},
 {Bb3,1},
 {BK,1},
 
 {Bb3,1},
 {BK,1},
 {Bb3,1},
 {BK,1},
 {Bb3,1},
 {BK,1},
 
 {Bb3,1},
 {BK,1},
 {Bb3,1},
 {BK,1},
 {Bb3,1},
 {BK,1},
 
 {Bb3,1},
 {BK,1},
 {Bb3,1},
 {BK,1},
 {Bb3,1},
 {BK,1},
 
 {F4,1},
 {BK,1},
 {F4,1},
 {BK,1},
 {F4,1},
 {BK,1},
 
 {F4,1},
 {BK,1},
 {F4,1},
 {BK,1},
 {F4,1},
 {BK,1},
 
 {F4,1},
 {BK,1},
 {F4,1},
 {BK,1},
 {F4,1},
 {BK,1},
 
 {F4,1},
 {BK,1},
 {F4,1},
 {BK,1},
 {F4,1},
 {BK,1},
 
 {C5,2},
 {Eb4,2},
 {Ab4,1},
 {Bb4,1},
 
 {C5,2},
 {Eb4,2},
 {Ab4,1},
 {C5,1},
 
 {Bb4,2},
 {Eb4,2},
 {G4,1},
 {Ab4,1},
 
 {Bb4,2},
 {Eb4,2},
 {G4,1},
 {Bb4,1},
 
 {Ab4,2},
 {C4,2},
 {F4,1},
 {G4,1},
 
 {Ab4,2},
 {C4,2},
 {F4,1},
 {Ab4,1},
 
 {G4,2},
 {C4,2},
 {Eb4,1},
 {F4,1},
 
 {G4,2},
 {C4,2},
 {Eb4,1},
 {G4,1},
 
 {Eb4,2},
 {Ab3,2},
 {C4,1},
 {D4,1},
 
 {Eb4,2},
 {Ab3,2},
 {C4,1},
 {D4,1},
 
 {Eb4,2},
 {G3,2},
 {C4,1},
 {Eb4,1},
 
 {D4,2},
 {G3,2},
 {B3,1},
 {D4,1},
 
 {C4,2},
 {G4,2},
 {Ab4,1},
 {Bb4,1},
 
 {C4,2},
 {G4,2},
 {Ab4,1},
 {Bb4,1},
 
 {C4,2},
 {G4,2},
 {Ab4,1},
 {Bb4,1},
 
 {C4,2},
 {G4,2},
 {Ab4,1},
 {Bb4,1}
};

#define testLength 1
Sng test[testLength]={
	{C5,1}
};

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode

#define PF1 (*((volatile uint32_t*)0x40025008))
#define PF2 (*((volatile uint32_t*)0x40025010))


#define F6HZ (50000000/6)
#define F20KHZ (80000000/20000)

// 12-bit 32-element sine wave
// multiply each value by 2 to shift into bits 12:1 of SSI packet
// three control bits in 15:13 are all zero for immediate DAC update
// book figure shows MAX5353 in unipolar rail-to-rail configuration
// that means when wave[n] = 0x0000 (LSB = 0), output = 0
//                 wave[n] = 0x1000 (LSB = 0), output = Vref
//                 wave[n] = 0x1FFE (LSB = 0), output = 2*Vref
const uint16_t wave[32] = {
  2048*2,2448*2,2832*2,3186*2,3496*2,3751*2,3940*2,4057*2,4095*2,4057*2,3940*2,
  3751*2,3496*2,3186*2,2832*2,2448*2,2048*2,1648*2,1264*2,910*2,600*2,345*2,
  156*2,39*2,0*2,39*2,156*2,345*2,600*2,910*2,1264*2,1648*2};

	
	
const unsigned short Bassoon64[64]={
1068, 1169, 1175, 1161, 1130, 1113, 1102, 1076, 1032, 985, 963, 987, 1082, 1343, 1737, 1863, 
1575, 1031, 538, 309, 330, 472, 626, 807, 1038, 1270, 1420, 1461, 1375, 1201, 1005, 819, 658, 
532, 496, 594, 804, 1055, 1248, 1323, 1233, 1049, 895, 826, 826, 850, 862, 861, 899, 961, 1006, 
1023, 1046, 1092, 1177, 1224, 1186, 1133, 1098, 1102, 1109, 1076, 1027, 1003};

const unsigned short Trumpet64[64]={
  987, 1049, 1090, 1110, 1134, 1160, 1139, 1092, 1070, 1042, 1035, 1029, 1008, 1066, 1150, 1170, 1087, 915, 679, 372, 151, 
  558, 1014, 1245, 1260, 1145, 1063, 984, 934, 960, 1027, 1077, 1081, 1074, 1064, 1042, 1010, 974, 968, 974, 994, 1039, 
  1094, 1129, 1125, 1092, 1056, 1056, 1082, 1059, 1046, 1058, 1061, 1045, 1034, 1050, 1094, 1112, 1092, 1063, 1053, 1065, 1052, 992};

const unsigned short Oboe64[64]={
1024, 1024, 1014, 1008, 1022, 1065, 1093, 1006, 858, 711, 612, 596, 672, 806, 952, 1074, 1154, 1191, 
1202, 1216, 1236, 1255, 1272, 1302, 1318, 1299, 1238, 1140, 1022, 910, 827, 779, 758, 757, 782, 856, 
972, 1088, 1177, 1226, 1232, 1203, 1157, 1110, 1067, 1028, 993, 958, 929, 905, 892, 900, 940, 1022, 
1125, 1157, 1087, 965, 836, 783, 816, 895, 971, 1017};
	
const float envolop32[16] = {
  2048*2,2448*2,2832*2,3186*2,3496*2,3751*2,3940*2,4057*2,4095*2,4057*2,3940*2,
  3751*2,3496*2,3186*2,2832*2,2448*2
};

volatile int shouldPlaySong;// bool that tells us if we should play the song
volatile int isPlaying = 0; // a bool checking if the song is playing
volatile int beatCount = 0; // the length
volatile int songIndex = 0; // the note index
volatile int beatCount2 = 0; // the length
volatile int songIndex2 = 0; // the note index
volatile int freqCount1 = 0;
volatile int freqCount2 = 0;
volatile float freq1, freq2;

volatile int curInst = 0;
volatile int totalInst = 3;
	
#define CYCLE_WAIT 5
int debounce[4];
void Buttons_ReadInput(void) {
	int32_t data = Switch_Get();
	if((data & 0x01) != 0)
		debounce[0] = (data & 0x01)*CYCLE_WAIT;
	if((data & 0x02) != 0)
		debounce[1] = ((data & 0x2) >> 1)*CYCLE_WAIT;
	if((data & 0x04) != 0)
		debounce[2] = ((data & 0x4) >> 2)*CYCLE_WAIT;
	if((data & 0x08) != 0)
		debounce[3] = ((data & 0x8) >> 3)*CYCLE_WAIT;
}
	
void Buttons_Pressed(uint32_t button) {
	if (button == 0){
		if (shouldPlaySong == 1){
			shouldPlaySong = 0;
			
			ST7735_SetCursor(0, 4);
			ST7735_OutString("Not Playing");

		}
		else {
			shouldPlaySong = 1;
			
			ST7735_SetCursor(0, 4);
			ST7735_OutString("Playing");
		}
	}
	else if (button == 1){
		isPlaying = 0;
	}
	else if (button == 2){
		curInst ++;
		curInst = curInst % totalInst;
	}
}

void CheckSwitch(void){
	Buttons_ReadInput();
	for(uint32_t i = 0; i < 4; i += 1) {
		if(debounce[i] > 0) {
			debounce[i] -= 1;
			if(debounce[i] == 0) {
				Buttons_Pressed(i);
			}
		}
	}
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
	
	PF1 &= ~0x02;
	PF2 &= ~0x04;
}

uint32_t getCycle (float freq){
	float cycleDouble = (float)781250 / freq;
	uint32_t waitCycle =(uint32_t) cycleDouble;
	return waitCycle;
}

void musicBeat(void){
	PF1 ^= 0x02;
	
	CheckSwitch();
	
	if(shouldPlaySong == 1 && isPlaying == 0) {
		isPlaying = 1;
		songIndex = 0;
		beatCount = throne[songIndex].Time;
		freqCount1 = 0;
		freq1 = throne[songIndex].Note;
		TIMER1_TAILR_R = getCycle(freq1)-1;
		
		songIndex2 = 0;
		beatCount2 = throne2[songIndex2].Time;
		freqCount2 = 0;
		freq2 = throne[songIndex2].Note;
		TIMER2_TAILR_R = getCycle(freq2)-1;
		
	} if(shouldPlaySong == 1 && isPlaying == 1) {
		beatCount --;
		beatCount2 --;

		if(beatCount == 0 && songIndex !=throneLength - 1){ // not the last note in song
			songIndex ++;
			freqCount1 = 0;
			beatCount = throne[songIndex].Time;
			freq1 = throne[songIndex].Note;
			TIMER1_TAILR_R = getCycle(freq1)-1;
		}
		else if(beatCount == 0 && songIndex == throneLength - 1) { // start over again
			songIndex = 0;
			freqCount1 = 0;
			beatCount = throne[songIndex].Time;		
			freq1 = throne[songIndex].Note;
			TIMER1_TAILR_R = getCycle(freq1)-1;
		}
		
		if(beatCount2 == 0 && songIndex2 !=throne2Length - 1){ // not the last note in song
			songIndex2 ++;
			freqCount2 = 0;
			beatCount2 = throne2[songIndex2].Time;
			freq2 = throne2[songIndex2].Note;
			TIMER2_TAILR_R = getCycle(freq2)-1;
		}
		else if(beatCount2 == 0 && songIndex2 == throne2Length - 1) { // start over again
			songIndex2 = 0;
			freqCount2 = 0;
			beatCount2 = throne2[songIndex2].Time;		
			freq2 = throne2[songIndex2].Note;
			TIMER2_TAILR_R = getCycle(freq2)-1;
		}
	}
	
	PF1 ^= 0x02;
}
volatile int wavecount1 = 0;
volatile int wavecount2 = 0;
volatile uint16_t wave1;
volatile uint16_t wave2;
//#define THRESHOLD 20
#define INC_RATE 5
#define DEC_RATE 2

uint16_t envelop1 (uint16_t wave){
	
	uint16_t minus = 0;
	int THRESHOLD = freq2 / 4;
	if(freqCount2 < THRESHOLD){
		minus = (THRESHOLD-freqCount2) * INC_RATE;
	}
	else {
		minus = wave2 - (freqCount2 - THRESHOLD) * DEC_RATE;
	}

	uint16_t newwave2 = wave;

	if (newwave2 <= (wave2 / 3)){
		newwave2 =  (wave2 / 3);
	}


	return newwave2;
}

uint16_t envelop2 (uint16_t wave){
	
	uint16_t minus = 0;


	uint16_t newwave2 = wave;

	if (newwave2 <= wave2){
		newwave2 = wave2;
	}


	return wave;
}

void handler(void){
	
	wavecount1++;
	if (wavecount1 >= 64){
		wavecount1 = 0;
		freqCount1 ++;
	}
	wave1 = curInst == 0? Bassoon64[wavecount1&0x3F] : curInst == 1 ? Trumpet64[wavecount1&0x3F] : Oboe64[wavecount1&0x3F];

	//wave1 = envelop2(wave1);
	
	uint16_t w = (wave1 + wave2) /2;
	DAC_OutRaw(shouldPlaySong == 1? w : 0);
}


void handler2(void){
	wavecount2++;
	if (wavecount2 >= 64){
		wavecount2 = 0;
		freqCount2 ++;
	}
	wave2 = (curInst == 0? Bassoon64[wavecount2&0x3F] : curInst == 1 ? Trumpet64[wavecount2&0x3F] : Oboe64[wavecount2&0x3F])/2;

	//wave2 = envelop2(wave2);
	uint16_t w = (wave1 + wave2) /2;
	DAC_OutRaw(shouldPlaySong == 1? w : 0);
}


int main(void){
	DisableInterrupts();
	
	PLL_Init(Bus50MHz);
	
	Timer0A_Init(&musicBeat, F6HZ);  // initialize timer0A (16 Hz)
	
	Switch_Init();
	
	ST7735_InitR(INITR_REDTAB);
	ST7735_FillScreen(ST7735_BLACK);
	ST7735_SetCursor(0, 0);
	ST7735_OutString("Final Proj Prototype");
	
  DAC_InitRaw();                  // initialize with command: Vout = Vref
	Timer1A_Init(&handler, 498);
	Timer2A_Init(&handler2, 498);
	Heartbeat_Init();
	
	
	EnableInterrupts();
	
	shouldPlaySong = 1;
	
	PF2 ^= 0x04;
	
  while(1){
		int x= 0;
  }
}