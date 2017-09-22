#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "SysTick.h"
#include "PLL.h"
#include "LCDDriver.h"
#include "SwitchDriver.h"
#include "SoundDriver.h"

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode

#define AUS 0;
#define NYC 1;
#define LA  2;

#define PF2   (*((volatile uint32_t *)0x40025010))

uint8_t hour = 12;
uint8_t min = 0;
uint8_t alarmHour = 6;
uint8_t alarmMin = 0;
uint8_t zone = AUS;
uint16_t heartBeatCount = 100;
uint8_t sound = 0;
uint32_t count;

int main(){
	
	SYSCTL_RCGCGPIO_R |= 0x20;        // 1) activate clock for Port F
  while((SYSCTL_PRGPIO_R&0x20)==0){}; // allow time for clock to start
                                    // 2) no need to unlock PF2, PF4
  GPIO_PORTF_PCTL_R &= ~0x000F0F00; // 3) regular GPIO
  GPIO_PORTF_AMSEL_R &= ~0x14;      // 4) disable analog function on PF2, PF4
  GPIO_PORTF_PUR_R |= 0x10;         // 5) pullup for PF4
  GPIO_PORTF_DIR_R |= 0x04;         // 5) set direction to output
  GPIO_PORTF_AFSEL_R &= ~0x14;      // 6) regular port function
  GPIO_PORTF_DEN_R |= 0x14;         // 7) enable digital port
	
	PLL_Init(Bus80MHz);         // set system clock to 50 MHz
  SoundInit();
	SwitchInit();
	LCDInit();
	SysTick_Init(80000);
	uint8_t input;
	while(1){
		// heartbeat
		if(heartBeatCount > 0){
			--heartBeatCount;
		}
		else{
			PF2 ^= 0x02;
			heartBeatCount = 100;
		}
		// logic
		input = GetSwitch();
		if((alarmHour == hour) && (alarmMin == min)){
			sound = 1;
			SoundOn();
		}
		if(input & 0x4){
			if(zone < 2){
				++zone;
			}
			else{
				zone = AUS;
			}
		}
		if(input & 0x2 && sound){
			SoundOff();
			sound = 0;
		}
		if(input & 0x1){
			SoundOff();
			sound = 0;
		  DisableInterrupts();
			if(input & 0x2)
			if(min == 59){
				min = 0; 
				if(hour == 12){
					hour = 0;
				}
				else{
					++hour;
				}
			}
			EnableInterrupts();
		}
		if(input & 0x4){
			SoundOff();
			sound = 0;
			DisableInterrupts();
			if(input & 0x2)
			if(min == 59){
				min = 0; 
				if(hour == 12){
					hour = 0;
				}
				else{
					++hour;
				}
			}
			EnableInterrupts();
		}
		if(count > 60000){
			if(min == 59){
				min = 0; 
				if(hour == 12){
					hour = 0;
				}
				else{
					++hour;
				}
			}
		}
		else ++ min;
		LCDDisplay(hour, min, zone);
	}
}
