#include <stdint.h>
#include <stdio.h>
#include "../inc/tm4c123gh6pm.h"
#include "ST7735.h"
#include "Draw.h"

struct clock_hand_pos {
	uint32_t x;
	uint32_t y;
};
struct clock_hand_pos Hour_hand[12] = { {64,50},{79,54},{90,65},{94,80},{90,95},{79,106},{64,110},{49,106},{38,95},{34,80},{38,65},{49,54} };
struct clock_hand_pos Min_hand[60] = {
{64,40},
{68,40},
{72,41},
{76,42},
{80,43},
{84,45},
{88,48},
{91,50},
{94,53},
{96,56},
{99,60},
{101,64},
{102,68},
{103,72},
{104,76},
{104,80},
{104,84},
{103,88},
{102,92},
{101,96},
{99,100},
{96,104},
{94,107},
{91,110},
{88,112},
{84,115},
{80,117},
{76,118},
{72,119},
{68,120},
{64,120},
{60,120},
{56,119},
{52,118},
{48,117},
{44,115},
{40,112},
{37,110},
{34,107},
{32,104},
{29,100},
{27,96},
{26,92},
{25,88},
{24,84},
{24,80},
{24,76},
{25,72},
{26,68},
{27,64},
{29,60},
{32,56},
{34,53},
{37,50},
{40,48},
{44,45},
{48,43},
{52,42},
{56,41},
{60,40},
};

char* timezones[] = {
	"UTC-12:00 Baker     ", 
	"UTC-11:00 Pago      ",
	"UTC-10:00 Papeete   ",
	"UTC-9:00 Anchorage  ",
	"UTC-8:00 Los Angeles",
	"UTC-7:00 Phoenix    ",
	"UTC-6:00 Chicago    ",
	"UTC-5:00 New York   ",
	"UTC-4:00 Puerto Rico",
	"UTC-3:00 Sao Paulo  ",
	"UTC-2:00 Fernando   ",
	"UTC-1:00 Praia      ",
	"UTC 0:00 London     ",
	"UTC+1:00 Paris      ",
	"UTC+2:00 Anthens    ",
	"UTC+3:00 Moscow     ",
	"UTC+4:00 Dubai      ",
	"UTC+5:00 Tashkent   ",
	"UTC+6:00 Almaty     ",
	"UTC+7:00 Ho Chi Minh",
	"UTC+8:00 Beijing    ",
	"UTC+9:00 Tokyo      ",
	"UTC+10:00 Sydney    ",
	"UTC+11:00 Noumea    ",
	"UTC+12:00 Auckland  ",
};

uint32_t Hours_old, Mins_old, Secs_old;

void LCDInit(void){
	ST7735_InitR(INITR_REDTAB);
}

/**** Initialize the Clock face ****/
void LCDDrawClockFace(void){
	ST7735_FillScreen(ST7735_BLACK);

	ST7735_Circle(64, 80,42,ST7735_WHITE);
	ST7735_DrawChar(87, 37, '1', ST7735_WHITE, ST7735_BLACK, 1);
	ST7735_DrawChar(103, 54, '2', ST7735_WHITE, ST7735_BLACK, 1);
	ST7735_DrawChar(109, 78, '3', ST7735_WHITE, ST7735_BLACK, 1);
	ST7735_DrawChar(103, 100, '4', ST7735_WHITE, ST7735_BLACK, 1);
	ST7735_DrawChar(86, 119, '5', ST7735_WHITE, ST7735_BLACK, 1);
	ST7735_DrawChar(62, 125, '6', ST7735_WHITE, ST7735_BLACK, 1);
	ST7735_DrawChar(39, 119, '7', ST7735_WHITE, ST7735_BLACK, 1);
	ST7735_DrawChar(21, 101, '8', ST7735_WHITE, ST7735_BLACK, 1);
	ST7735_DrawChar(14, 77, '9', ST7735_WHITE, ST7735_BLACK, 1);
	ST7735_DrawChar(16, 53, '1', ST7735_WHITE, ST7735_BLACK, 1);
	ST7735_DrawChar(21, 53, '0', ST7735_WHITE, ST7735_BLACK, 1);
	ST7735_DrawChar(34, 35, '1', ST7735_WHITE, ST7735_BLACK, 1);
	ST7735_DrawChar(39, 35, '1', ST7735_WHITE, ST7735_BLACK, 1);
	ST7735_DrawChar(60, 29, '1', ST7735_WHITE, ST7735_BLACK, 1);
	ST7735_DrawChar(65, 29, '2', ST7735_WHITE, ST7735_BLACK, 1);
}

void LCDDisplayClock(uint32_t hour, uint32_t min, uint32_t sec, uint32_t alarmHr, uint32_t alarmMin, uint8_t zone, uint8_t flick){
	if (Hours_old != hour){
			ST7735_Line(64,80,Hour_hand[Hours_old].x,Hour_hand[Hours_old].y,ST7735_BLACK,1,2); 				// erase the old hour hand

		}
		if(Mins_old != min){																																						// if min hand rolled over,
			ST7735_Line(64,80,Min_hand[Mins_old].x,Min_hand[Mins_old].y,ST7735_BLACK,1,1);							// erase the old min hand
		}
		if(Secs_old != sec){
			
			ST7735_Line(64,80,Min_hand[Secs_old].x,Min_hand[Secs_old].y,ST7735_BLACK,1,1);							// erase old sec hand
			ST7735_Line(64,80,Hour_hand[hour].x,Hour_hand[hour].y,ST7735_BLUE,1,2);	// draw new hour hand
			ST7735_Line(64,80,Min_hand[min].x,Min_hand[min].y,ST7735_BLUE,1,1);			// draw new min hand
			ST7735_Line(64,80,Min_hand[sec].x,Min_hand[sec].y,ST7735_RED,1,1);				// draw new sec hand
				
			
			ST7735_SetCursor(0, 0);
			ST7735_OutString(timezones[zone]);
			
			ST7735_SetCursor(4, 2);		
			
			char alrHrStr[12];
			char alrMinStr[12];
			sprintf(alrHrStr, "%02d", alarmHr);
			sprintf(alrMinStr, "%02d", alarmMin);
			
				
			ST7735_OutString("Alarm");
			ST7735_OutChar(flick == 2 ? ' ' : ':');
			ST7735_OutString(alrHrStr);
			ST7735_OutChar(flick == 2 ? ' ' : ':');
			ST7735_OutString(alrMinStr);
				

			ST7735_SetCursor(8, 14);
			char hourStr[12];
			char minStr[12];
			char secStr[12];
			sprintf(hourStr, "%02d", hour);
			sprintf(minStr, "%02d", min);
			sprintf(secStr, "%02d", sec);
			
			ST7735_OutString(hourStr);
			ST7735_OutChar(flick == 1 ? ' ' : ':');
			ST7735_OutString(minStr);
			ST7735_OutChar(flick == 1 ? ' ' : ':');
			ST7735_OutString(secStr);
		}
		
		Hours_old = hour;
		Mins_old = min;
		Secs_old = sec;
}
