#include <stdint.h>
#include <stdio.h>
#include "Bool.h"
#include "../inc/tm4c123gh6pm.h"
#include "SysTick.h"
#include "Timers.h"
#include "PLL.h"
#include "LCDDriver.h"
#include "AlarmClock.h"
#include "SwitchDriver.h"
#include "SongPlayer.h"

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode

bool set_alarm = false;

#define CYCLE_WAIT 5
bool debounce[4];
void Buttons_ReadInput(void) {
	int32_t data = GetSwitch();
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
		if(! set_alarm) {
			ClockAddHr();
			ClockSetActive(true);
		}
		else {
			ClockAlrmAddHr();
			ClockSetActive(true);
		}
	}
	else if (button == 1){
		if(! set_alarm) {
			ClockAddMin();
			
			ClockSetActive(true);
		}
		else {
			ClockAlrmAddMin();
			ClockSetActive(true);
		}
	}
	else if (button == 2){
		set_alarm = !set_alarm;
	}
	else if (button == 3){
		ClockSetActive(false);
	}
	else if (button == 4){
		AddTimeZone();
		
	}
}

bool PF4Pressed = false;

void TickClockFast(void){
	Buttons_ReadInput();
	for(uint32_t i = 0; i < 4; i += 1) {
		if(debounce[i] > 0) {
			debounce[i] -= 1;
			if(debounce[i] == 0) {
				Buttons_Pressed(i);
			}
		}
	}
	
	
	if (PF4 != 0){
		PF4Pressed = false;
	}
	else{
		if (!PF4Pressed){
			Buttons_Pressed(4);
			PF4Pressed = true;
		}
	}
	
	TickSong();
}

void Button1(void){
}

void Button2(void){
}

void Button3(void){
}

void Button4(void){
}

void Button5(void){
}

int main(){
	DisableInterrupts();
	
	Heartbeat_Init();
	
	PLL_Init(Bus80MHz);         // set system clock to 80 MHz
  SongPlayerInit();
	SwitchInit();
	LCDInit();
	
	Timer0A_Init(&TickClock, 80000000);	
	Timer1_Init(&TickClockFast, 8000000);
	
	EnableInterrupts();
	
	LCDDrawClockFace();
	
	while(1){
		LCDDisplayClock(GetHr(), GetMin(), GetSec(), GetAlrmHr(), GetAlrmMin(), GetZone(), ((GetSec() % 2)!=0 && !set_alarm) ? 1 : ((GetSec() % 2)!=0 && set_alarm) ? 2 : 0  );
		if ((GetSec() % 2)!=0){
			PF2 ^= 0x04;
		}
		
		if (GetAlrmHr() == GetHr() && GetAlrmMin() == GetMin() && GetAlarmActive()){
			PlaySong();
		} else {
			StopSong();
		}

	}
	
}
