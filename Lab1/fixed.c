// fixed.c 
// C file for Lab 1 Spring 2017
// Layer on top of ST7735 driver
// Paul Heath, Eric Liang
// Written: 9/6/2017
// Last Updated: 9/7/2017
// TA: Josh Cristol

#include <stdio.h>
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
	if(n > 256000){
		for(uint8_t i = 0; i < 6; ++i){
			if(i == 2){ 
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
		n /= 256;
		// put first digits unless they are zeros
		for(uint16_t i = 10000; i >= 100; i /= 10){
			ch = n / i;
			n %= i;
			if((ch != 0) || i == 100){
				fputc(ch + 48, (FILE*) 3);
			}
			else{
				fputc(' ', (FILE*) 3);
			}
		}
		fputc('.', (FILE*) 3);
		// put remaining digits to screen
		for(uint8_t i = 10; i >= 1; i /= 10){
			ch = n/i;
			n %= i;
			fputc(ch + 48, (FILE*) 3);
		}
	}
}

// static globals for boundaries of scatter plot
static int32_t LowerX, LowerY, UpperX, UpperY, RangeX, RangeY;


//**************ST7735_XYplotInit***************
void ST7735_XYplotInit(char *title, int32_t minX, int32_t maxX, int32_t minY, int32_t maxY){
	ST7735_OutString(title);
  ST7735_PlotClear(minY, maxY);
	// save all max min values in private globals
	LowerX = minX;
	UpperX = maxX;
	LowerY = minY;
	UpperY = maxY;
	RangeX = UpperX - LowerX;
	RangeY = UpperY - LowerY;
}


//**************ST7735_XYplot***************
void ST7735_XYplot(uint32_t num, int32_t bufX[], int32_t bufY[]){
	uint8_t pixelX, pixelY;
	// calculate pixel locations and put to screen
	for(uint32_t i = 0; i < num; ++i){
		pixelX = (bufX[i] + LowerX) / RangeX;
		pixelY = ((bufY[i] + LowerY) / RangeY) + 32;
		ST7735_DrawPixel(pixelX, pixelY, ST7735_BLACK);
	}
}
