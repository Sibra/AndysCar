/*
 * Application.c
 *
 *  Created on: 10.10.2014
 *      Author: Laptop
 */

#include "Platform.h"
#include "Application.h"
#include "WAIT1.h"

#if PL_HAS_LED
#include "LED.h"
#endif
#if PL_HAS_EVENTS
#include "Event.h"
#endif
#if PL_HAS_KEYS
#include "Keys.h"
#endif
#if PL_HAS_MEALY
#include "Mealy.h"
#endif
#if PL_HAS_TRIGGER
#include "Trigger.h"
#endif
#if PL_HAS_SHELL
#include "CLS1.h"
#include "Shell.h"
#endif
#if PL_HAS_BUZZER
#include "Buzzer.h"
#endif
#if PL_HAS_RTOS
#include "RTOS.h"
#include "FRTOS1.h"
#endif
#include "CLS1.h"
#if PL_HAS_MOTOR
#include "Motor.h"
#endif
#if PL_HAS_REFLECTANCE
#include "Reflectance.h"
#endif
#if PL_HAS_CONFIG_NVM
#include "NVM_Config.h"
#endif
#if PL_HAS_ACCEL
  #include "Accel.h"
#endif
#if PL_HAS_RADIO
#include "RNET1.h"
#include "RNet_App.h"
#include "RNetConf.h"
#include "RApp.h"
#endif
#if RNET_CONFIG_REMOTE_STDIO
#include "RStdIO.h"
#endif
#include "Stadium.h"
#if PL_HAS_WATCHDOG
  #include "Watchdog.h"
#endif

#define APP_dstAddr 			(0xFF)
#define RPHY_PACKET_FLAGS_NONE 	(0)

bool state = TRUE;
static uint8_t lastKeyPressed;
bool getState()
{
	return state;
}
void APP_DebugPrint(unsigned char *str) {
#if PL_HAS_SHELL
  CLS1_SendStr(str, CLS1_GetStdio()->stdOut);
#endif
}

static void APP_EventHandler(EVNT_Handle event)/*!TODO why static*/
{
	static int cntr = 0;
	switch (event) {
	case EVNT_INIT:
		init_function;
#if PL_HAS_ACCEL
		ACCEL_LowLevelInit();
#endif
#if PL_IS_ROBO
		SHELL_SendString((unsigned char*) "Hello ROBO\r\n");
#endif

#if PL_IS_FRDM
		SHELL_SendString((unsigned char*)"Hello FRDM\r\n");
#endif
#if PL_HAS_BUZZER
		//BUZ_Beep(500,500);
		//FRTOS1_vTaskDelay(2000/TRG_TICKS_MS);
#endif
		break;
	case EVENT_LED_HEARTBEAT:
		LED_GREEN_Neg();
		break;
//---------------------------------------------------------------
#if PL_NOF_KEYS >= 1
	case EVNT_SW1_PRESSED:
		lastKeyPressed = 1;
		state = !state;
#if !PL_HAS_STADIUM
		state=FIGHT;
		//RAPP_SendPayloadDataBlock(&state, sizeof(state), RAPP_MSG_TYPE_REMOTE_STATE, APP_dstAddr, RPHY_PACKET_FLAGS_NONE);
#endif
#if PL_HAS_SHELL
		CLS1_SendStr("SW1 PRESSED!\r\n", CLS1_GetStdio()->stdOut);
		//SHELL_SendString((unsigned char*) "SW1 PRESSED!\r\n");
#endif
		break;

	case EVNT_SW1_LPRESSED:
#if PL_HAS_SHELL
		CLS1_SendStr("SW1 LONG PRESSED!\r\n", CLS1_GetStdio()->stdOut);
#endif
		break;
	case EVNT_SW1_RELEASED:
#if PL_HAS_SHELL
		CLS1_SendStr("SW1 RELEASED!\r\n", CLS1_GetStdio()->stdOut);
#endif
		break;
#endif
//---------------------------------------------------------------
#if PL_NOF_KEYS >= 2
		case EVNT_SW2_PRESSED:
			remoteMode = 1;
		lastKeyPressed = 2;

#if PL_HAS_SHELL
		CLS1_SendStr("SW2 PRESSED!\r\n", CLS1_GetStdio()->stdOut);
#endif
		break;
		case EVNT_SW2_LPRESSED:
		CLS1_SendStr("SW2 LONG PRESSED!\r\n", CLS1_GetStdio()->stdOut);
		break;
		case EVNT_SW2_RELEASED:
		CLS1_SendStr("SW2 RELEASED!\r\n", CLS1_GetStdio()->stdOut);
		break;
#endif
//---------------------------------------------------------------
#if PL_NOF_KEYS >= 3
		case EVNT_SW3_PRESSED:
			remoteMode=0;
		lastKeyPressed = 3;
#if PL_HAS_SHELL
		CLS1_SendStr("SW3 PRESSED!\r\n", CLS1_GetStdio()->stdOut);
#endif
		break;
		case EVNT_SW3_LPRESSED:
		CLS1_SendStr("SW3 LONG PRESSED!\r\n", CLS1_GetStdio()->stdOut);
		break;
		case EVNT_SW3_RELEASED:
		CLS1_SendStr("SW3 RELEASED!\r\n", CLS1_GetStdio()->stdOut);
		break;
#endif
//---------------------------------------------------------------
#if PL_NOF_KEYS >= 4
		case EVNT_SW4_PRESSED:
#if !PL_HAS_STADIUM
		state=STOP;

		RAPP_SendPayloadDataBlock(&state, sizeof(state), RAPP_MSG_TYPE_REMOTE_STATE, APP_dstAddr, RPHY_PACKET_FLAGS_NONE);
#endif
		lastKeyPressed = 4;
#if PL_HAS_SHELL
		CLS1_SendStr("SW4 PRESSED!\r\n", CLS1_GetStdio()->stdOut);
#endif
		break;
		case EVNT_SW4_LPRESSED:
		CLS1_SendStr("SW4 LONG PRESSED!\r\n", CLS1_GetStdio()->stdOut);
		break;
		case EVNT_SW4_RELEASED:
		CLS1_SendStr("SW4 RELEASED!\r\n", CLS1_GetStdio()->stdOut);
		break;
#endif
//---------------------------------------------------------------
#if PL_NOF_KEYS >= 5
		case EVNT_SW5_PRESSED:
		lastKeyPressed = 5;
#if !PL_HAS_STADIUM
		state=NEED_CALIB;
		RAPP_SendPayloadDataBlock(&state, sizeof(state), RAPP_MSG_TYPE_REMOTE_STATE, APP_dstAddr, RPHY_PACKET_FLAGS_NONE);
#endif
#if PL_HAS_SHELL
		CLS1_SendStr("SW5 PRESSED!\r\n", CLS1_GetStdio()->stdOut);
#endif
		break;
		case EVNT_SW5_LPRESSED:
		CLS1_SendStr("SW5 LONG PRESSED!\r\n", CLS1_GetStdio()->stdOut);
		break;
		case EVNT_SW5_RELEASED:
		CLS1_SendStr("SW5 RELEASED!\r\n", CLS1_GetStdio()->stdOut);
		break;
#endif
//---------------------------------------------------------------
#if PL_NOF_KEYS >= 6
		case EVNT_SW6_PRESSED:
		lastKeyPressed = 6;
		remoteMode = 0;
#if !PL_HAS_STADIUM
		state=REMOTE_CONTROL;
		RAPP_SendPayloadDataBlock(&state, sizeof(state), RAPP_MSG_TYPE_REMOTE_STATE, APP_dstAddr, RPHY_PACKET_FLAGS_NONE);
#endif
#if PL_HAS_SHELL
		CLS1_SendStr("SW6 PRESSED!\r\n", CLS1_GetStdio()->stdOut);
#endif
		break;
		case EVNT_SW6_LPRESSED:
		CLS1_SendStr("SW6 LONG PRESSED!\r\n", CLS1_GetStdio()->stdOut);
		break;
		case EVNT_SW6_RELEASED:
		CLS1_SendStr("SW6 RELEASED!\r\n", CLS1_GetStdio()->stdOut);
		break;
#endif
//---------------------------------------------------------------
#if PL_NOF_KEYS >= 7
		case EVNT_SW7_PRESSED:
		lastKeyPressed = 7;
#if !PL_HAS_STADIUM
		state=FIND;
		RAPP_SendPayloadDataBlock(&state, sizeof(state), RAPP_MSG_TYPE_REMOTE_STATE, APP_dstAddr, RPHY_PACKET_FLAGS_NONE);
#endif
#if PL_HAS_SHELL
		CLS1_SendStr("SW7 PRESSED!\r\n", CLS1_GetStdio()->stdOut);
#endif
		break;
		case EVNT_SW7_LPRESSED:
		CLS1_SendStr("SW7 LONG PRESSED!\r\n", CLS1_GetStdio()->stdOut);
		break;
		case EVNT_SW7_RELEASED:
		CLS1_SendStr("SW7 RELEASED!\r\n", CLS1_GetStdio()->stdOut);
		break;
#endif
	default:
		break;
	}
}

static int8_t ToSigned8Bit(uint16_t val) {
	int tmp;
	tmp = ((int) ((val >> 12) & 0x0F)-7);

	return (int8_t) tmp;
}


static portTASK_FUNCTION(AppLoop, pvParameters) {
	for (;;) {

		EVNT_HandleEvent(APP_EventHandler);
		KEY_Scan();
#if PL_HAS_WATCHDOG
		WDT_IncTaskCntr(WDT_TASK_ID_MAIN, 50);
#endif
		FRTOS1_vTaskDelay(30);
#if 0
		uint8_t res;
		uint16_t values[2];
		res = AD1_Measure(TRUE);
		if (res != ERR_OK) {
			for (;;)
				;
		}
		res = AD1_GetValue16(&values[0]);
		if (res != ERR_OK) {
			for (;;)
				;
		}
		int x = ToSigned8Bit(values[0]) - 1;
		int y = ToSigned8Bit(values[1]) - 1;
		int8_t speedL = y + x + 7;
		if (speedL > 15) {
			speedL = 15;
		}
		if (speedL < 0) {
			speedL = 0;
		}
		int8_t speedR = y - x + 7;
		if (speedR > 15) {
			speedR = 15;
		}
		if (speedR < 0) {
			speedR = 0;
		}

		int8_t val8 = ((speedL << 4) + speedR);
		RAPP_SendPayloadDataBlock(&val8, sizeof(val8), RAPP_MSG_TYPE_REMOTE_RUN,
				APP_dstAddr,
				RPHY_PACKET_FLAGS_NONE);
		FRTOS1_vTaskDelay(10/portTICK_RATE_MS);
#endif
	}
}

void APP_Start(void) {
	PL_Init();


	EVNT_SetEvent(EVNT_INIT);
#if PL_HAS_RTOS
	if (FRTOS1_xTaskCreate(AppLoop, (signed portCHAR *)"App Loop", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+1, NULL) != pdPASS) {
		for (;;) {
		} /* error */
	}
#endif
	RTOS_Run();
	for (;;)
		;
	PL_Deinit();
}


