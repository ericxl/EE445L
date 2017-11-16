#ifndef __ALARMCLOCK_H__
#define __ALARMCLOCK_H__


uint32_t GetSec(void);

uint32_t GetMin(void);

uint32_t GetHr(void);

void SetSec(uint32_t s);

void SetMin(uint32_t m);

void SetHr(uint32_t h);


uint32_t GetAlrmMin(void);

uint32_t GetAlrmHr(void);

uint32_t GetZone(void);

bool GetAlarmActive(void);

void TickClock(void);

void ClockSetActive(bool newActive);

void ClockAddHr(void);

void ClockAddMin(void);

void ClockAlrmAddHr(void);

void ClockAlrmAddMin(void);

void AddTimeZone(void);

#endif
