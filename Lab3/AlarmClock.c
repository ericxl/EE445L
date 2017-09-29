#include <stdint.h>
#include "Bool.h"

uint32_t Secs_current =55;
uint32_t Mins_current = 38;
uint32_t Hours_current = 3;
uint32_t alarmHour = 3;
uint32_t alarmMin = 39;
uint32_t zone = 7;
bool alarm_active = true;


uint32_t GetSec(void){
	return Secs_current;
}

uint32_t GetMin(void){
	return Mins_current;
}

uint32_t GetHr(void){
	return Hours_current;
}

uint32_t GetAlrmMin(void){
	return alarmMin;
}

uint32_t GetAlrmHr(void){
	return alarmHour;
}

uint32_t GetZone(void){
	return zone;
}

bool GetAlarmActive(void){
	return alarm_active;
}

void TickClock(void){
	Secs_current += 1;
	
	if(Secs_current >= 60) {
		Secs_current = 0;
		Mins_current += 1;
	}
	if(Mins_current >= 60) {
		Mins_current = 0;
		Hours_current += 1;
	}
	if(Hours_current >= 12) {
		Hours_current = 0;
	}
}

void ClockSetActive(bool newActive){
	alarm_active = newActive;
}

void ClockAddHr(void){
	Hours_current = (Hours_current + 1) % 12;
}

void ClockAddMin(void){
	if(Mins_current+1 >= 60) {
		Hours_current = (Hours_current + 1) % 12;
	}
	Mins_current = (Mins_current + 1) % 60;
}

void ClockAlrmAddHr(void){
	alarmHour = (alarmHour + 1) % 12;
}

void ClockAlrmAddMin(void){
	alarmMin = (alarmMin + 1) % 60;
}

void AddTimeZone(void){
	int oldzone = zone;
		
	zone ++;
	if (zone > 24){
		zone = 0;
	}
	int offset = zone - oldzone;
	alarmHour = alarmHour + offset;
		
	Hours_current += offset;	
	alarmHour = (alarmHour + 24) % 12;
	Hours_current = (Hours_current + 24) % 12;
}
