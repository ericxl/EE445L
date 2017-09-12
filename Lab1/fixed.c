 // fixed.c 
// C file for Lab 1 Spring 2017
// Layer on top of ST7735 driver
// See fixed.h for descriptions of subroutines
// Paul Heath, Eric Liang
// Written: 9/6/2017
// Last Updated: 9/12/2017
// TA: Josh Cristol

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "fixed.h"
#include "ST7735.h"


// ****************ST7735_sDecOut3***************
void ST7735_sDecOut3(int32_t n){
	int32_t ch;
	// error case handled
  if((n > 9999) || (n < -9999)){
	  for(uint8_t i = 0; i < 6; ++i){
		  if(i == 0){ 
				ch = ' ';
			}
			else if(i == 2){ 
				ch = '.';
			}
			else{ 
				ch = '*';
			}
		  fputc(ch, (FILE*) 3);
		}
	}
	// no error
  else{
		// check for sign
		if(n < 0){
			fputc('-', (FILE*) 3);
			n *= -1;
		}
		// put space if positive
		else{
			fputc(' ', (FILE*) 3);
	  }
		// put most significant digit
	  fputc(n / 1000 + 48, (FILE*) 3);
    n %= 1000;
    fputc('.', (FILE*) 3);
		// put rest of digits in order
    for(uint8_t i = 100; i >= 1; i /= 10){
      fputc(n / i + 48, (FILE*) 3);
      n %= i;
		}			
	}
}


//**************ST7735_uBinOut8***************
void ST7735_uBinOut8(uint32_t n){
	uint32_t ch;
	// handle error
	if(n >= 256000){
		for(uint8_t i = 0; i < 6; ++i){
			if(i == 3){ 
				ch = '.';
			}
			else{ 
				ch = '*';
			}
			fputc(ch, (FILE*) 3);
		}
	}
	// no error
	else{
		n = (n * 100) / 256;
		uint32_t placeValue = 10000;
		// ignore leading zeros
		while(placeValue > 100){
		  // break when you see nonzero leading value or the ones place 
			if(n / placeValue){
        break;
			}
			fputc(' ', (FILE*) 3);
			placeValue /= 10;
		}
		// put digits up to the decimal point from the left
		while(placeValue > 10){
		  fputc(n / placeValue + 48, (FILE*) 3);
		  n %= placeValue;
		  placeValue /= 10;
		}
		fputc('.', (FILE*) 3);
		// put the last two digits
		for(uint8_t i = 0; i < 2; ++i){
			fputc(n / placeValue + 48, (FILE*) 3);
			n %= placeValue;
		  placeValue /= 10;
		}
	}
}


//**************ST7735_XYplotInit***************
static int32_t MinX, MaxX, MinY, MaxY;
void ST7735_XYplotInit(char *title, int32_t minX, int32_t maxX, int32_t minY, int32_t maxY){
  MinX = minX;
  MaxX = maxX;
  MinY = minY;
  MaxY = maxY;
	ST7735_PlotClear(minY, maxY);
  ST7735_FillScreen(ST7735_BLACK);
	ST7735_SetCursor(0, 0);
  ST7735_OutString(title);
}


//**************ST7735_XYplot***************
void ST7735_XYplot(uint32_t count, int32_t bufX[], int32_t bufY[]){
  for (int i = 0; i < count; i++) {
		if (bufX[i] <= MaxX && bufX[i] >= MinX && bufY[i] <= MaxY && bufY[i] >= MinY){
			int32_t rangeX = MaxX - MinX;
			int32_t rangeY = MaxY - MinY;
			int32_t coordinateX = (127 * (bufX[i] - MinX)) / rangeX;
			int32_t coordinateY = 32 + (127 * (MaxY - bufY[i])) / rangeY;	
			ST7735_DrawPixel(coordinateX, coordinateY, ST7735_MAGENTA);
		}
  }
}
