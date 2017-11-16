#include <stdint.h>
#include "Bool.h"

uint32_t secs =55;
uint32_t mins = 38;
uint32_t hours = 3;
uint32_t alarmHour = 3;
uint32_t alarmMin = 39;
uint32_t zone = 7;
bool alarm_active = true;


uint32_t GetSec(void){
	return secs;
}

uint32_t GetMin(void){
	return mins;
}

uint32_t GetHr(void){
	return hours;
}

void SetSec(uint32_t s){
	secs = s;
}

void SetMin(uint32_t m){
	mins = m;
}

void SetHr(uint32_t h){
	hours = h;
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
	secs += 1;
	
	if(secs >= 60) {
		secs = 0;
		mins += 1;
	}
	if(mins >= 60) {
		mins = 0;
		hours += 1;
	}
	if(hours >= 12) {
		hours = 0;
	}
}

void ClockSetActive(bool newActive){
	alarm_active = newActive;
}

void ClockAddHr(void){
	hours = (hours + 1) % 12;
}

void ClockAddMin(void){
	if(mins+1 >= 60) {
		hours = (hours + 1) % 12;
	}
	mins = (mins + 1) % 60;
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
		
	hours += offset;	
	alarmHour = (alarmHour + 24) % 12;
	hours = (hours + 24) % 12;
}
