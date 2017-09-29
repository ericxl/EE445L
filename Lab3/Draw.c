#include <stdint.h>
#include "Draw.h"

#include "ST7735.h"

int32_t abs(int32_t v){
	if (v <0){
		return -v;
	}else {
		return v;
	}
}

void ST7735_Line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color, uint16_t interval, uint16_t drawWidth){
	int32_t dx = abs(x2 - x1), sx = x1 < x2 ? interval : -interval;
  int32_t dy = abs(y2 - y1), sy = y1 < y2 ? interval : -interval; 
  int32_t err = (dx>dy ? dx : -dy)/2;        
  while (1) {
		ST7735_DrawPixel(x1, y1, color);
		for (uint16_t i = 1; i < drawWidth; i++){
			ST7735_DrawPixel(x1-i, y1, color);
			ST7735_DrawPixel(x1+i, y1, color);
			ST7735_DrawPixel(x1, y1-i, color);
			ST7735_DrawPixel(x1, y1+i, color);
		}
		
		
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

void ST7735_Circle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color){
	int x = r-1;
  int y = 0;
  int dx = 1;
  int dy = 1;
  int err = dx - (r << 1);

	while (x >= y)
	{
			ST7735_DrawPixel(x0 + x, y0 + y, color);
			ST7735_DrawPixel(x0 + y, y0 + x, color);
			ST7735_DrawPixel(x0 - y, y0 + x, color);
			ST7735_DrawPixel(x0 - x, y0 + y, color);
			ST7735_DrawPixel(x0 - x, y0 - y, color);
			ST7735_DrawPixel(x0 - y, y0 - x, color);
			ST7735_DrawPixel(x0 + y, y0 - x, color);
			ST7735_DrawPixel(x0 + x, y0 - y, color);

			if (err <= 0)
			{
					y++;
					err += dy;
					dy += 2;
			}
			if (err > 0)
			{
					x--;
					dx += 2;
					err += (-r << 1) + dx;
			}
	}
}

