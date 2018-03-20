#include <stdint.h>
#include <stdbool.h>
#include "ST7735.h"
#include "ADCSWTrigger.h"

int randomPos[] = {0,2,1,2,0,1,1,0,2};

int lane1 = 4096/3;
int lane2 = 2*(4096/3);

void carDodge(void){
	bool alive = true;
	int carLanePos = 0;
	int oldCarLanePos = 0;
	int obstacleLanePos1 = 0;
	int oldObstacleLanePos1;
	//test
	//int obstacleYPos1 = -30;
	int obstacleYPos1 = -30;
	//end test
	int oldObstacleYPos1;
	int obstacleLanePos2 = 0;
	int oldObstacleLanePos2;
	//test
	//int obstacleYPos2 = -90;
	int obstacleYPos2 = -50;
	//end test
	int oldObstacleYPos2;
	int randomPosPtr1 = 0;
	int randomPosPtr2 = 2;
	int speed = 1000;
	int score = 0;
	
	int i;

ST7735_FillScreen(0);
ST7735_SetCursor(3,5);
ST7735_OutString("Dodge The Cars!");
for (i=0;i<30000000;i++){}
ST7735_FillScreen(0);
ST7735_SetCursor(5,5);
ST7735_OutString("Ready!");
for (i=0;i<8000000;i++){}
ST7735_FillScreen(0);
ST7735_SetCursor(5,5);
ST7735_OutString("Set!");
for (i=0;i<8000000;i++){}
ST7735_FillScreen(0);
ST7735_SetCursor(5,5);
ST7735_OutString("Go!!!");
for (i=0;i<8000000;i++){}
ST7735_FillScreen(0);

	
	ST7735_DrawFastVLine(43,20,140,ST7735_WHITE);																		// draws lanes
	ST7735_DrawFastVLine(85,20,140,ST7735_WHITE);
	
	ST7735_FillRect(7,130,30,30,ST7735_YELLOW);									// draws initial car
	
	while(alive == true){

		for (i=0;i<100000;i++){}
		
		oldCarLanePos = carLanePos;
		oldObstacleYPos1 = obstacleYPos1;
		oldObstacleLanePos1 = obstacleLanePos1;
		oldObstacleYPos2 = obstacleYPos2;
		oldObstacleLanePos2 = obstacleLanePos2;
		carLanePos = ADC0_InSeq3();
//			ST7735_SetCursor(0,1);										// code to debug adc
//			ST7735_OutUDec(carLanePos);
		
		if (obstacleYPos1 > 190){																										// only changes obstacle lane if off screen
			obstacleLanePos1 = randomPos[(carLanePos + randomPosPtr1) % 9];							// determines random lane by ADC and array
			randomPosPtr1++;
			obstacleYPos1 = -30;
			score++;
		}
		obstacleYPos1 = obstacleYPos1 + speed/950;											// determines new Y pos based on speed
		if (obstacleYPos2 > 190){
			obstacleLanePos2 = randomPos[(carLanePos + randomPosPtr2) % 9];
			randomPosPtr2 = (randomPosPtr2 + 3) % 10;
			obstacleYPos2 = -30;
			score++;
		}
		obstacleYPos2 = obstacleYPos2 + speed/1000;
		
		if (carLanePos <= lane1){carLanePos = 0;}
		else if ((carLanePos > lane1) && (carLanePos < lane2)){carLanePos = 1;}
		else {carLanePos = 2;}
		
		if (((carLanePos == obstacleLanePos1) && (obstacleYPos1 > 100 && obstacleYPos1 <130)) || ((carLanePos == obstacleLanePos2) && (obstacleYPos2 >= 100 && obstacleYPos2 <130))){			// determines if dead
			alive = false;
		}
		
		speed++;
		
		if (oldCarLanePos != carLanePos){																							// if car moved lanes, redraw car
			if (oldCarLanePos == 0){ST7735_FillRect(7,130,30,30,ST7735_BLACK);}					// removes old car image
			else if (oldCarLanePos == 1){ST7735_FillRect(49,130,30,30,ST7735_BLACK);}
			else if (oldCarLanePos == 2){ST7735_FillRect(91,130,30,30,ST7735_BLACK);}
			
			if (carLanePos == 0){ST7735_FillRect(7,130,30,30,ST7735_YELLOW);}						// draws new car image
			else if (carLanePos == 1){ST7735_FillRect(49,130,30,30,ST7735_YELLOW);}
			else if (carLanePos == 2){ST7735_FillRect(91,130,30,30,ST7735_YELLOW);}
		}
		
		if (oldObstacleLanePos1 == 0){ST7735_FillRect(7,oldObstacleYPos1,30,30,ST7735_BLACK);}				// remove old obstacle 1
		else if (oldObstacleLanePos1 == 1){ST7735_FillRect(49,oldObstacleYPos1,30,30,ST7735_BLACK);}
		else if (oldObstacleLanePos1 == 2){ST7735_FillRect(91,oldObstacleYPos1,30,30,ST7735_BLACK);}
		
		if (obstacleLanePos1 == 0){ST7735_FillRect(7,obstacleYPos1,30,30,ST7735_RED);}								// draw new obstacle 1
		else if (obstacleLanePos1 == 1){ST7735_FillRect(49,obstacleYPos1,30,30,ST7735_RED);}
		else if (obstacleLanePos1 == 2){ST7735_FillRect(91,obstacleYPos1,30,30,ST7735_RED);}
		
		if (oldObstacleLanePos2 == 0){ST7735_FillRect(7,oldObstacleYPos2,30,30,ST7735_BLACK);}				// obstacle 2
		else if (oldObstacleLanePos2 == 1){ST7735_FillRect(49,oldObstacleYPos2,30,30,ST7735_BLACK);}
		else if (oldObstacleLanePos2 == 2){ST7735_FillRect(91,oldObstacleYPos2,30,30,ST7735_BLACK);}
		
		if (obstacleLanePos2 == 0){ST7735_FillRect(7,obstacleYPos2,30,30,ST7735_RED);}								// ""
		else if (obstacleLanePos2 == 1){ST7735_FillRect(49,obstacleYPos2,30,30,ST7735_RED);}
		else if (obstacleLanePos2 == 2){ST7735_FillRect(91,obstacleYPos2,30,30,ST7735_RED);}
		
		ST7735_SetCursor(0,0);
		ST7735_OutString("Score: ");
		ST7735_SetCursor(7,0);
		ST7735_OutUDec(score);
	}
	
	
}