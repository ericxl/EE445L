/*
 * main.c - Example project for UT.6.02x Embedded Systems - Shape the World
 * Jonathan Valvano and Ramesh Yerraballi
 * July 14, 2015
 * Hardware requirements 
     TM4C123 LaunchPad, optional Nokia5110
     CC3100 wifi booster and 
     an internet access point with OPEN, WPA, or WEP security
 
 * derived from TI's getweather example
 * Copyright (C) 2014 Texas Instruments Incorporated - http://www.ti.com/
 *
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
*/

/*
 * Application Name     -   Get weather
 * Application Overview -   This is a sample application demonstrating how to
                            connect to openweathermap.org server and request for
              weather details of a city.
 * Application Details  -   http://processors.wiki.ti.com/index.php/CC31xx_SLS_Get_Weather_Application
 *                          doc\examples\sls_get_weather.pdf
 */
 /* CC3100 booster pack connections (unused pins can be used by user application)
Pin  Signal        Direction      Pin   Signal     Direction
P1.1  3.3 VCC         IN          P2.1  Gnd   GND      IN
P1.2  PB5 UNUSED      NA          P2.2  PB2   IRQ      OUT
P1.3  PB0 UART1_TX    OUT         P2.3  PE0   SSI2_CS  IN
P1.4  PB1 UART1_RX    IN          P2.4  PF0   UNUSED   NA
P1.5  PE4 nHIB        IN          P2.5  Reset nRESET   IN
P1.6  PE5 UNUSED      NA          P2.6  PB7  SSI2_MOSI IN
P1.7  PB4 SSI2_CLK    IN          P2.7  PB6  SSI2_MISO OUT
P1.8  PA5 UNUSED      NA          P2.8  PA4   UNUSED   NA
P1.9  PA6 UNUSED      NA          P2.9  PA3   UNUSED   NA
P1.10 PA7 UNUSED      NA          P2.10 PA2   UNUSED   NA

Pin  Signal        Direction      Pin   Signal      Direction
P3.1  +5  +5 V       IN           P4.1  PF2 UNUSED      OUT
P3.2  Gnd GND        IN           P4.2  PF3 UNUSED      OUT
P3.3  PD0 UNUSED     NA           P4.3  PB3 UNUSED      NA
P3.4  PD1 UNUSED     NA           P4.4  PC4 UART1_CTS   IN
P3.5  PD2 UNUSED     NA           P4.5  PC5 UART1_RTS   OUT
P3.6  PD3 UNUSED     NA           P4.6  PC6 UNUSED      NA
P3.7  PE1 UNUSED     NA           P4.7  PC7 NWP_LOG_TX  OUT
P3.8  PE2 UNUSED     NA           P4.8  PD6 WLAN_LOG_TX OUT
P3.9  PE3 UNUSED     NA           P4.9  PD7 UNUSED      IN (see R74)
P3.10 PF1 UNUSED     NA           P4.10 PF4 UNUSED      OUT(see R75)

UART0 (PA1, PA0) sends data to the PC via the USB debug cable, 115200 baud rate
Port A, SSI0 (PA2, PA3, PA5, PA6, PA7) sends data to Nokia5110 LCD

*/
#include "..\cc3100\simplelink\include\simplelink.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/fpu.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
#include "utils/cmdline.h"
#include "application_commands.h"
#include "LED.h"
#include "Nokia5110.h"
#include "ST7735.h"
#include "ADCSWTrigger.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Timer1.h"
#include "..//inc//tm4c123gh6pm.h"
#include "SongPlayer.h"
#include "SwitchDriver.h"
#include "AlarmClock.h"
#include "LCDDriver.h"
//#include "PLL.h"
//#include "Timers.h"

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode



#define SSID_NAME  "EriciPhone7"        /* Access point name to connect to. */
#define SEC_TYPE   SL_SEC_TYPE_WPA
#define PASSKEY    "123456789"        /* Password in case of secure AP */
#define BAUD_RATE   115200
void UART_Init(void){
  SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
  GPIOPinConfigure(GPIO_PA0_U0RX);
  GPIOPinConfigure(GPIO_PA1_U0TX);
  GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
  UARTStdioConfig(0,BAUD_RATE,50000000);
}

#define MAX_RECV_BUFF_SIZE  2048
#define MAX_SEND_BUFF_SIZE  512
#define MAX_HOSTNAME_SIZE   40
#define MAX_PASSKEY_SIZE    32
#define MAX_SSID_SIZE       32


#define SUCCESS             0

#define CONNECTION_STATUS_BIT   0
#define IP_AQUIRED_STATUS_BIT   1

/* Application specific status/error codes */
typedef enum{
    DEVICE_NOT_IN_STATION_MODE = -0x7D0,/* Choosing this number to avoid overlap w/ host-driver's error codes */

    STATUS_CODE_MAX = -0xBB8
}e_AppStatusCodes;


/* Status bits - These are used to set/reset the corresponding bits in 'g_Status' */
typedef enum{
    STATUS_BIT_CONNECTION =  0, /* If this bit is:
                                 *      1 in 'g_Status', the device is connected to the AP
                                 *      0 in 'g_Status', the device is not connected to the AP
                                 */

    STATUS_BIT_IP_AQUIRED,       /* If this bit is:
                                 *      1 in 'g_Status', the device has acquired an IP
                                 *      0 in 'g_Status', the device has not acquired an IP
                                 */

}e_StatusBits;


#define SET_STATUS_BIT(status_variable, bit)    status_variable |= (1<<(bit))
#define CLR_STATUS_BIT(status_variable, bit)    status_variable &= ~(1<<(bit))
#define GET_STATUS_BIT(status_variable, bit)    (0 != (status_variable & (1<<(bit))))
#define IS_CONNECTED(status_variable)           GET_STATUS_BIT(status_variable, \
                                                               STATUS_BIT_CONNECTION)
#define IS_IP_AQUIRED(status_variable)          GET_STATUS_BIT(status_variable, \
                                                               STATUS_BIT_IP_AQUIRED)

typedef struct{
    UINT8 SSID[MAX_SSID_SIZE];
    INT32 encryption;
    UINT8 password[MAX_PASSKEY_SIZE];
}UserInfo;

/*
 * GLOBAL VARIABLES -- Start
 */

char Recvbuff[MAX_RECV_BUFF_SIZE];
char SendBuff[MAX_SEND_BUFF_SIZE];
char HostName[MAX_HOSTNAME_SIZE];
unsigned long DestinationIP;
int SockID;


typedef enum{
    CONNECTED = 0x01,
    IP_AQUIRED = 0x02,
    IP_LEASED = 0x04,
    PING_DONE = 0x08

}e_Status;
UINT32  g_Status = 0;
/*
 * GLOBAL VARIABLES -- End
 */


 /*
 * STATIC FUNCTION DEFINITIONS  -- Start
 */

static int32_t configureSimpleLinkToDefaultState(char *);


/*
 * STATIC FUNCTION DEFINITIONS -- End
 */



void Crash(uint32_t time){
  while(1){
    for(int i=time;i;i--){};
    LED_RedToggle();
  }
}


int32_t strequal(const char *str1, const char *str2, uint32_t length) {
	for(uint32_t i = 0; i < length; i += 1) {
		if(str1[i] != str2[i])
			return 0;
	}
	return 1;
}

#define MAX_TEMP_LENGTH 10
// null terminated buffer for temperature extraction
const char temperature_buffer[MAX_TEMP_LENGTH+1];
const char formatted_temperature_buffer[MAX_TEMP_LENGTH+1+9];

char* Get_Temperature(char *received_data) {
	const char *tempMatch = "\"temp\":";
	const int tempLength = 7;
	int32_t index = -1;
	// find index of temperature string
	for(uint32_t i = 0; i < MAX_RECV_BUFF_SIZE; i += 1) {
		if(strequal(tempMatch, &received_data[i], tempLength)) {
			index = i + tempLength;
			break;
		}
	}
	
	//copy temperature value part out
	char *temperatureString = &received_data[index];
	uint32_t j;
	for(j = 0; j < MAX_TEMP_LENGTH && received_data[j + index] != ','; j += 1) {
		temperatureString[j] = received_data[j + index];
	}
	temperatureString[j] = '\0';
	
	return temperatureString;
}

char* Get_Time(char *received_data) {
	
	const char *tempMatch = "\"GMT\":";
	const int tempLength = -9;
	int32_t index = -1;
	// find index of temperature string
	for(uint32_t i = 0; i < MAX_RECV_BUFF_SIZE; i += 1) {
		if(strequal(tempMatch, &received_data[i], tempLength)) {
			index = i + tempLength;
			break;
		}
	}
	
	//copy temperature value part out
	char *temperatureString = &received_data[index];
	uint32_t j;
	for(j = 0; j < 13 && received_data[j + index] != ','; j += 1) {
		temperatureString[j] = received_data[j + index];
	}
	temperatureString[j] = '\0';
	
	return temperatureString;
}
/*
void Wifi_Connect(char *pConfig, SlSecParams_t *secParams) {
	int32_t retVal;
	retVal = configureSimpleLinkToDefaultState(pConfig); // set policies
  if(retVal < 0) Crash(4000000);
  retVal = sl_Start(0, pConfig, 0);
  if((retVal < 0) || (ROLE_STA != retVal) ) Crash(8000000);
  secParams->Key = PASSKEY;
  secParams->KeyLen = strlen(PASSKEY);
  secParams->Type = SEC_TYPE; // OPEN, WPA, or WEP
  sl_WlanConnect(SSID_NAME, strlen(SSID_NAME), 0, secParams, 0);
  while((0 == (g_Status&CONNECTED)) || (0 == (g_Status&IP_AQUIRED))){
    _SlNonOsMainLoopTask();
  }
  UARTprintf("Connected\n");
}
*/


const char *REQ_1 = " HTTP/1.1\r\nUser-Agent: Keil\r\nHost:";
const char *REQ_2 = "\r\n\r\n";



char* HTTP_Request(const char *hostName, uint16_t port, const char *method, const char *request, char *requestData1, char *requestData2) {
	SlSockAddrIn_t Addr; int32_t retVal; uint32_t ASize = 0;
	memset(&Recvbuff,0,MAX_RECV_BUFF_SIZE);
  memset(&SendBuff,0,MAX_SEND_BUFF_SIZE);
  memset(&HostName,0,MAX_HOSTNAME_SIZE);
  DestinationIP = 0;;
  SockID = 0;
	strcpy(HostName, hostName);
	//UARTprintf("\r\n\r\nUsing host: %s\r\n", HostName);
	retVal = sl_NetAppDnsGetHostByName(HostName, strlen(HostName),&DestinationIP, SL_AF_INET);
	if(retVal == 0){
		Addr.sin_family = SL_AF_INET;
		Addr.sin_port = sl_Htons(port);
		Addr.sin_addr.s_addr = sl_Htonl(DestinationIP);// IP to big endian 
		ASize = sizeof(SlSockAddrIn_t);
		SockID = sl_Socket(SL_AF_INET,SL_SOCK_STREAM, 0);
		if( SockID >= 0 ){
			retVal = sl_Connect(SockID, ( SlSockAddr_t *)&Addr, ASize);
		}
		if((SockID >= 0)&&(retVal >= 0)){
			uint32_t copyIndex = 0;
			strcpy(&SendBuff[copyIndex], method);  copyIndex += strlen(method);
			strcpy(&SendBuff[copyIndex], " ");     copyIndex += 1;
			strcpy(&SendBuff[copyIndex], request); copyIndex += strlen(request);
			if(requestData1) {
				strcpy(&SendBuff[copyIndex], requestData1);
				copyIndex += strlen(requestData1);
			}
			if(requestData2) {
				strcpy(&SendBuff[copyIndex], requestData2);
				copyIndex += strlen(requestData2);
			}
			strcpy(&SendBuff[copyIndex], REQ_1);   copyIndex += strlen(REQ_1);
			strcpy(&SendBuff[copyIndex], hostName); copyIndex += strlen(hostName);
			strcpy(&SendBuff[copyIndex], REQ_2);   copyIndex += strlen(REQ_2);
			SendBuff[copyIndex] = '\0';
			
			sl_Send(SockID, SendBuff, strlen(SendBuff), 0);// Send the HTTP GET 
			sl_Recv(SockID, Recvbuff, MAX_RECV_BUFF_SIZE, 0);// Receive response 
			sl_Close(SockID);
			
			return Recvbuff;
		}
	}
	return NULL;
}

char voltagestring_buff[20];
char* VoltageToString(uint32_t sample) {
	uint32_t i;
	voltagestring_buff[19] = '\0';
	for(i = 18; sample != 0; i--) {
		voltagestring_buff[i] = (char)('0' + sample % 10);
		sample /= 10;
	}
	return &voltagestring_buff[i+1];
}

/*
 * Application's entry point
 */

bool set_alarm = 0;

#define CYCLE_WAIT 5
bool debounce[4];
void Buttons_ReadInput(void) {
	int32_t data = GetSwitch();
	if((data & 0x01) != 0)
		debounce[0] = (data & 0x01)*CYCLE_WAIT;
	if((data & 0x02) != 0)
		debounce[1] = ((data & 0x2) >> 1)*CYCLE_WAIT;
	if((data & 0x04) != 0)
		debounce[2] = ((data & 0x4) >> 2)*CYCLE_WAIT;
	if((data & 0x08) != 0)
		debounce[3] = ((data & 0x8) >> 3)*CYCLE_WAIT;
}

void Buttons_Pressed(uint32_t button) {
	if (button == 0){
		if(! set_alarm) {
			ClockAddHr();
			ClockSetActive(1);
		}
		else {
			ClockAlrmAddHr();
			ClockSetActive(1);
		}
	}
	else if (button == 1){
		if(! set_alarm) {
			ClockAddMin();
			
			ClockSetActive(1);
		}
		else {
			ClockAlrmAddMin();
			ClockSetActive(1);
		}
	}
	else if (button == 2){
		set_alarm = !set_alarm;
	}
	else if (button == 3){
		ClockSetActive(0);
	}
	else if (button == 4){
		AddTimeZone();
		
	}
}

bool PF4Pressed = 0;

void TickClockFast(void){
	Buttons_ReadInput();
	for(uint32_t i = 0; i < 4; i += 1) {
		if(debounce[i] > 0) {
			debounce[i] -= 1;
			if(debounce[i] == 0) {
				Buttons_Pressed(i);
			}
		}
	}
	
	
	if (PF4 != 0){
		PF4Pressed = 0;
	}
	else{
		if (!PF4Pressed){
			Buttons_Pressed(4);
			PF4Pressed = 1;
		}
	}
	
	TickSong();
}

int counter1 = 0;
int counter2 = 0;

#define REQUEST "GET /data/2.5/weather?q=Austin%20Texas&APPID=1bc54f645c5f1c75e681c102ed4bbca4&units=metric HTTP/1.1\r\nUser-Agent: Keil\r\nHost:api.openweathermap.org\r\nAccept: */*\r\n\r\n"

int main(void){
	
	
	SlSecParams_t secParams;
  char *pConfig = NULL;
  initClk();        // PLL 50 MHz
  UART_Init();      // Send data to PC, 115200 bps
  LED_Init();       // initialize LaunchPad I/O
	
	Timer0_Init(&TickClock);	
	Timer1_Init(&TickClockFast);
	
	// *** Lab 3 stuff
	Heartbeat_Init();
  SongPlayerInit();
	SwitchInit();
	// **************
	
	// ADC 
	ADC0_InitSWTriggerSeq3_Ch9(); 
	
	
	// LCD Display
	ST7735_InitR(INITR_REDTAB);
	ST7735_SetCursor(1,1);
	printf("Weather App");
	
	
	//Set up wifi connection
	int32_t retVal;
	retVal = configureSimpleLinkToDefaultState(pConfig); // set policies
  if(retVal < 0) Crash(4000000);
  retVal = sl_Start(0, pConfig, 0);
  if((retVal < 0) || (ROLE_STA != retVal) ) Crash(8000000);
  secParams.Key = PASSKEY;
  secParams.KeyLen = strlen(PASSKEY);
  secParams.Type = SEC_TYPE; // OPEN, WPA, or WEP
  sl_WlanConnect(SSID_NAME, strlen(SSID_NAME), 0, &secParams, 0);
  while((0 == (g_Status&CONNECTED)) || (0 == (g_Status&IP_AQUIRED))){
    _SlNonOsMainLoopTask();
  }
  ST7735_OutString("Connected\n");
	
	LED_GreenOn();


	
	//Get Austin temperature
	INT32 ASize = 0; 
	SlSockAddrIn_t  Addr;

	strcpy(HostName, "api.openweathermap.org");
	retVal = sl_NetAppDnsGetHostByName(HostName, strlen(HostName),&DestinationIP, SL_AF_INET);
	if(retVal == 0){
		Addr.sin_family = SL_AF_INET;
		Addr.sin_port = sl_Htons(80);
		Addr.sin_addr.s_addr = sl_Htonl(DestinationIP);// IP to big endian 
		ASize = sizeof(SlSockAddrIn_t);
		SockID = sl_Socket(SL_AF_INET,SL_SOCK_STREAM, 0);
		if( SockID >= 0 ){
			retVal = sl_Connect(SockID, ( SlSockAddr_t *)&Addr, ASize);
		}
		if((SockID >= 0)&&(retVal >= 0)){
			strcpy(SendBuff,REQUEST); 
        sl_Send(SockID, SendBuff, strlen(SendBuff), 0);// Send the HTTP GET 
        sl_Recv(SockID, Recvbuff, MAX_RECV_BUFF_SIZE, 0);// Receive response 
        sl_Close(SockID);
		}
	}

	printf("Temp = %s Celsius\n", Get_Temperature(Recvbuff));
	
	
	//Calculate time and set it in alarm
	char* epochtime = Get_Time(Recvbuff);
	int epoch = atoi(epochtime);
	int diff = epoch - 1506920400;
	int hour = (diff / 3600) % 12;
	int min = (diff / 60) % 60;
	
	printf("GMT = %d:%d \n", hour, min);
	
	
	// Get ACD value
	uint32_t voltage = ADC0_InSeq3();
	LED_GreenOn();
	
	
	// Send data to server
	memset(&Recvbuff,0,MAX_RECV_BUFF_SIZE);
  memset(&SendBuff,0,MAX_SEND_BUFF_SIZE);
  memset(&HostName,0,MAX_HOSTNAME_SIZE);
  DestinationIP = 0;;
  SockID = 0;
	strcpy(HostName, "ee445l-xl5432.appspot.com");
	retVal = sl_NetAppDnsGetHostByName(HostName, strlen(HostName),&DestinationIP, SL_AF_INET);
	if(retVal == 0){
		Addr.sin_family = SL_AF_INET;
		Addr.sin_port = sl_Htons(80);
		Addr.sin_addr.s_addr = sl_Htonl(DestinationIP);// IP to big endian 
		ASize = sizeof(SlSockAddrIn_t);
		SockID = sl_Socket(SL_AF_INET,SL_SOCK_STREAM, 0);
		if( SockID >= 0 ){
			retVal = sl_Connect(SockID, ( SlSockAddr_t *)&Addr, ASize);
		}
		if((SockID >= 0)&&(retVal >= 0)){
			uint32_t index = 0;
			char *part1 = "GET /query?city=Austin%20Texas&id=Eric%20Liang&edxcode=8086&greet=Voltage~";
			strcpy(&SendBuff[index], part1);  
			index += strlen(part1);
					
			char volString[12];
			sprintf(volString, "%d", voltage);
			
			strcpy(&SendBuff[index], volString);
			index += strlen(volString);
			
			char *part2 = "V HTTP/1.1\r\nUser-Agent: Keil\r\nHost:ee445l-xl5432.appspot.com\r\n\r\n";
			strcpy(&SendBuff[index], part2);
			index += strlen(part2);
			
			SendBuff[index] = '\0';
			
			sl_Send(SockID, SendBuff, strlen(SendBuff), 0);// Send the HTTP GET 
			sl_Recv(SockID, Recvbuff, MAX_RECV_BUFF_SIZE, 0);// Receive response 
			sl_Close(SockID);
			
		}
	}

	
	//Print voltage to screen
	LED_GreenOff();
	UARTprintf("\r\n\r\n");
	UARTprintf(Recvbuff);  UARTprintf("\r\n");
	printf("Voltage~%uV\n", voltage);
	
	
	//Set up clock using time
	SetHr(hour);
	SetMin(min);
	
	LCDDrawClockFace();
	while(1){
		counter1 ++;
		counter2 ++;
		
		LCDDisplayClock(GetHr(), GetMin(), GetSec(), GetAlrmHr(), GetAlrmMin(), GetZone(), ((GetSec() % 2)!=0 && !set_alarm) ? 1 : ((GetSec() % 2)!=0 && set_alarm) ? 2 : 0  );
		if ((GetSec() % 2)!=0){
			PF2 ^= 0x04;
		}
		
		if (GetAlrmHr() == GetHr() && GetAlrmMin() == GetMin() && GetAlarmActive()){
			PlaySong();
		} else {
			StopSong();
		}

	}
}

/*!
    \brief This function puts the device in its default state. It:
           - Set the mode to STATION
           - Configures connection policy to Auto and AutoSmartConfig
           - Deletes all the stored profiles
           - Enables DHCP
           - Disables Scan policy
           - Sets Tx power to maximum
           - Sets power policy to normal
           - Unregister mDNS services

    \param[in]      none

    \return         On success, zero is returned. On error, negative is returned
*/
static int32_t configureSimpleLinkToDefaultState(char *pConfig){
  SlVersionFull   ver = {0};
  UINT8           val = 1;
  UINT8           configOpt = 0;
  UINT8           configLen = 0;
  UINT8           power = 0;

  INT32           retVal = -1;
  INT32           mode = -1;

  mode = sl_Start(0, pConfig, 0);


    /* If the device is not in station-mode, try putting it in station-mode */
  if (ROLE_STA != mode){
    if (ROLE_AP == mode){
            /* If the device is in AP mode, we need to wait for this event before doing anything */
      while(!IS_IP_AQUIRED(g_Status));
    }

        /* Switch to STA role and restart */
    retVal = sl_WlanSetMode(ROLE_STA);

    retVal = sl_Stop(0xFF);

    retVal = sl_Start(0, pConfig, 0);

        /* Check if the device is in station again */
    if (ROLE_STA != retVal){
            /* We don't want to proceed if the device is not coming up in station-mode */
      return DEVICE_NOT_IN_STATION_MODE;
    }
  }
    /* Get the device's version-information */
  configOpt = SL_DEVICE_GENERAL_VERSION;
  configLen = sizeof(ver);
  retVal = sl_DevGet(SL_DEVICE_GENERAL_CONFIGURATION, &configOpt, &configLen, (unsigned char *)(&ver));

    /* Set connection policy to Auto + SmartConfig (Device's default connection policy) */
  retVal = sl_WlanPolicySet(SL_POLICY_CONNECTION, SL_CONNECTION_POLICY(1, 0, 0, 0, 1), NULL, 0);

    /* Remove all profiles */
  retVal = sl_WlanProfileDel(0xFF);

    /*
     * Device in station-mode. Disconnect previous connection if any
     * The function returns 0 if 'Disconnected done', negative number if already disconnected
     * Wait for 'disconnection' event if 0 is returned, Ignore other return-codes
     */
  retVal = sl_WlanDisconnect();
  if(0 == retVal){
        /* Wait */
     while(IS_CONNECTED(g_Status));
  }

    /* Enable DHCP client*/
  retVal = sl_NetCfgSet(SL_IPV4_STA_P2P_CL_DHCP_ENABLE,1,1,&val);

    /* Disable scan */
  configOpt = SL_SCAN_POLICY(0);
  retVal = sl_WlanPolicySet(SL_POLICY_SCAN , configOpt, NULL, 0);

    /* Set Tx power level for station mode
       Number between 0-15, as dB offset from max power - 0 will set maximum power */
  power = 0;
  retVal = sl_WlanSet(SL_WLAN_CFG_GENERAL_PARAM_ID, WLAN_GENERAL_PARAM_OPT_STA_TX_POWER, 1, (unsigned char *)&power);

    /* Set PM policy to normal */
  retVal = sl_WlanPolicySet(SL_POLICY_PM , SL_NORMAL_POLICY, NULL, 0);

    /* TBD - Unregister mDNS services */
  retVal = sl_NetAppMDNSUnRegisterService(0, 0);


  retVal = sl_Stop(0xFF);


  g_Status = 0;
  memset(&Recvbuff,0,MAX_RECV_BUFF_SIZE);
  memset(&SendBuff,0,MAX_SEND_BUFF_SIZE);
  memset(&HostName,0,MAX_HOSTNAME_SIZE);
  DestinationIP = 0;;
  SockID = 0;


  return retVal; /* Success */
}




/*
 * * ASYNCHRONOUS EVENT HANDLERS -- Start
 */

/*!
    \brief This function handles WLAN events

    \param[in]      pWlanEvent is the event passed to the handler

    \return         None

    \note

    \warning
*/
void SimpleLinkWlanEventHandler(SlWlanEvent_t *pWlanEvent){
  switch(pWlanEvent->Event){
    case SL_WLAN_CONNECT_EVENT:
    {
      SET_STATUS_BIT(g_Status, STATUS_BIT_CONNECTION);

            /*
             * Information about the connected AP (like name, MAC etc) will be
             * available in 'sl_protocol_wlanConnectAsyncResponse_t' - Applications
             * can use it if required
             *
             * sl_protocol_wlanConnectAsyncResponse_t *pEventData = NULL;
             * pEventData = &pWlanEvent->EventData.STAandP2PModeWlanConnected;
             *
             */
    }
    break;

    case SL_WLAN_DISCONNECT_EVENT:
    {
      sl_protocol_wlanConnectAsyncResponse_t*  pEventData = NULL;

      CLR_STATUS_BIT(g_Status, STATUS_BIT_CONNECTION);
      CLR_STATUS_BIT(g_Status, STATUS_BIT_IP_AQUIRED);

      pEventData = &pWlanEvent->EventData.STAandP2PModeDisconnected;

            /* If the user has initiated 'Disconnect' request, 'reason_code' is SL_USER_INITIATED_DISCONNECTION */
      if(SL_USER_INITIATED_DISCONNECTION == pEventData->reason_code){
        UARTprintf(" Device disconnected from the AP on application's request \r\n");
      }
      else{
        UARTprintf(" Device disconnected from the AP on an ERROR..!! \r\n");
      }
    }
    break;

    default:
    {
      UARTprintf(" [WLAN EVENT] Unexpected event \r\n");
    }
    break;
  }
}

/*!
    \brief This function handles events for IP address acquisition via DHCP
           indication

    \param[in]      pNetAppEvent is the event passed to the handler

    \return         None

    \note

    \warning
*/
void SimpleLinkNetAppEventHandler(SlNetAppEvent_t *pNetAppEvent){
  switch(pNetAppEvent->Event)
  {
    case SL_NETAPP_IPV4_ACQUIRED:
    {

      SET_STATUS_BIT(g_Status, STATUS_BIT_IP_AQUIRED);
        /*
             * Information about the connected AP's ip, gateway, DNS etc
             * will be available in 'SlIpV4AcquiredAsync_t' - Applications
             * can use it if required
             *
             * SlIpV4AcquiredAsync_t *pEventData = NULL;
             * pEventData = &pNetAppEvent->EventData.ipAcquiredV4;
             * <gateway_ip> = pEventData->gateway;
             *
             */

    }
    break;

    default:
    {
            UARTprintf(" [NETAPP EVENT] Unexpected event \r\n");
    }
    break;
  }
}

/*!
    \brief This function handles callback for the HTTP server events

    \param[in]      pServerEvent - Contains the relevant event information
    \param[in]      pServerResponse - Should be filled by the user with the
                    relevant response information

    \return         None

    \note

    \warning
*/
void SimpleLinkHttpServerCallback(SlHttpServerEvent_t *pHttpEvent,
                                  SlHttpServerResponse_t *pHttpResponse){
    /*
     * This application doesn't work with HTTP server - Hence these
     * events are not handled here
     */
  UARTprintf(" [HTTP EVENT] Unexpected event \r\n");
}

/*!
    \brief This function handles general error events indication

    \param[in]      pDevEvent is the event passed to the handler

    \return         None
*/
void SimpleLinkGeneralEventHandler(SlDeviceEvent_t *pDevEvent){
    /*
     * Most of the general errors are not FATAL are are to be handled
     * appropriately by the application
     */
  UARTprintf(" [GENERAL EVENT] \r\n");
}

/*!
    \brief This function handles socket events indication

    \param[in]      pSock is the event passed to the handler

    \return         None
*/
void SimpleLinkSockEventHandler(SlSockEvent_t *pSock){
  switch( pSock->Event )
  {
    case SL_NETAPP_SOCKET_TX_FAILED:
    {
            /*
            * TX Failed
            *
            * Information about the socket descriptor and status will be
            * available in 'SlSockEventData_t' - Applications can use it if
            * required
            *
            * SlSockEventData_t *pEventData = NULL;
            * pEventData = & pSock->EventData;
            */
      switch( pSock->EventData.status )
      {
        case SL_ECLOSE:
          UARTprintf(" [SOCK EVENT] Close socket operation failed to transmit all queued packets\r\n");
          break;


        default:
          UARTprintf(" [SOCK EVENT] Unexpected event \r\n");
          break;
      }
    }
    break;

    default:
      UARTprintf(" [SOCK EVENT] Unexpected event \r\n");
    break;
  }
}
/*
 * * ASYNCHRONOUS EVENT HANDLERS -- End
 */


