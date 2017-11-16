#ifndef __DRAW_H__
#define __DRAW_H__

void ST7735_Line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color, uint16_t interval, uint16_t drawWidth);

void ST7735_Circle(uint16_t x, uint16_t y, uint16_t r, uint16_t color);
#endif
