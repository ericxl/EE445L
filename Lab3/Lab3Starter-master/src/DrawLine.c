#include <stdint.h>
#include "DrawLine.h"

#include "ST7735.h"

int32_t abs(int32_t v){
	if (v <0){
		return -v;
	}else {
		return v;
	}
}

void ST7735_Lines(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color){
	int32_t dx = abs(x2 - x1), sx = x1 < x2 ? 1 : -1;
  int32_t dy = abs(y2 - y1), sy = y1 < y2 ? 1 : -1; 
  int32_t err = (dx>dy ? dx : -dy)/2;        
  while (1) {
		ST7735_DrawPixel(x1, y1, color);
    if (x1 == x2 && y1 == y2) {
			break;
		}
    int32_t e2 = err;
    if (e2 > -dx) { 
			err -= dy; 
			x1 += sx; 
		}
		if (e2 < dy) { 
			err += dx; 
			y1 += sy; 
		}
  }
	
}
