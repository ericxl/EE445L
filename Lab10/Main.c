#include "..//inc//tm4c123gh6pm.h"
#include <stdint.h>
#include <stdio.h>

#include "PLL.h"
#include "PWM.h"
#include "Tachometer.h"
#include "Switch.h"
#include "ST7735.h"




extern uint32_t Period;
extern uint32_t Target_Speed;
extern uint32_t Speed;

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode



int main(void) {
	DisableInterrupts();
  Tach_Init();
	PWM0B_Init(40000, 30000);
	Switch_Init();
	ST7735_InitR(INITR_REDTAB);


	EnableInterrupts();

	ST7735_SetCursor(0,0);
	uint32_t i = 0;
	
	while(1) {
		printf("Speed:\nDesired:");
	
		
		uint32_t speed = Tach_GetSpeed();
		uint32_t target = Target_Speed;
		
		
		
		printf("%5d.%01d rps", speed / 10, speed % 10);
		ST7735_SetCursor(8, 0);
		ST7735_SetCursor(8, 1);
		printf("%5d.%01d rps", Target_Speed / 10, Target_Speed % 10);
		
		if ((i % 700) == 0){
			ST7735_PlotPoints(Tach_GetSpeed(), Tach_GetSpeed());  
		}
    if((i%((ST7735_TFTWIDTH-1)*700))==0){          
      ST7735_PlotNextErase();
    }
    if((i%10000)==0){   
      ST7735_SetCursor(10,2); 
			
			ST7735_OutUDec(speed/10);
			ST7735_OutString(".");
			ST7735_OutUDec(speed%10);
			ST7735_OutString(" rps  "); 

			ST7735_SetCursor(5,1); 
			ST7735_OutUDec(target/10);
			ST7735_OutString(".");
			ST7735_OutUDec(target%10);		
			ST7735_OutString(" rps  "); 
    }
		i++;
	}
}
