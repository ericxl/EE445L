#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "ST7735.h"
#include "DrawLine.h"

#define AUS 0;
#define NYC 1;
#define LA  2;

char* aus = "Time Zone: AUS";
char* nyc = "Time Zone: NYC";
char* la = "Time Zone: LA";
uint8_t faceX[300];
uint8_t faceY[300];

void LCDInit(void){
	ST7735_InitR(INITR_REDTAB);
}

void LCDDisplay(uint8_t hour, uint8_t min, uint8_t zone){
	ST7735_FillScreen(ST7735_BLACK);
	ST7735_SetCursor(0, 0);
	if(zone == 0){
	  ST7735_OutString(aus);
	}
	else if(zone == 1){
		ST7735_OutString(nyc);
	}
	else if(zone == 2){
		ST7735_OutString(la);
	}
	ST7735_SetCursor(0, 10);
	ST7735_OutChar(hour + 48);
	ST7735_OutChar(':');
	ST7735_OutChar(min + 48);
	for(uint16_t i = 0; i < 300; ++i){
		ST7735_DrawPixel(faceX[i], faceY[i], ST7735_YELLOW);
	}
	ST7735_Lines(64, 80, hour, min, ST7735_YELLOW);
}
