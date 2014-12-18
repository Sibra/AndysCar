/*
 * stadium.c
 *
 *  Created on: 18.11.2014
 *      Author: Laptop
 */

#include "Platform.h"
#if PL_HAS_STADIUM

#include "Application.h"
#include "Stadium.h"
#include "Motor.h"
#include "Reflectance.h"
#include "UTIL1.h"
#include "CLS1.h"
#include "Shell.h"
#include "CLS1.h"
#if PL_HAS_RTOS
#include "RTOS.h"
#include "FRTOS1.h"
#endif
#if PL_HAS_ULTRASONIC
#include "Ultrasonic.h"
#endif
#if PL_HAS_PID
#include "Pid.h"
#endif
#if PL_HAS_MOTOR_TACHO
#include "Tacho.h"
#endif
#include "Drive.h"
#include "Motor.h"
#include "Watchdog.h"
#include "Buzzer.h"
#define TURNSPEEDFKT 2

int16_t speedR=6000;
int16_t speedL=6000;
int16_t turnSpeed=2800;
uint16_t ref=0;



//RTOS-------------------------------------------------------------------------------

static portTASK_FUNCTION(fight, pvParameters) {
	(void) pvParameters; /* not used */
	for (;;) {
		saveToFlash();
		//FRTOS1_vTaskResume(getTaskRefHandle());
		for (;;) {


//Warten auf Tastendruck--------------------------------------
			while (getState()) {
#if PL_HAS_WATCHDOG
				WDT_IncTaskCntr(WDT_TASK_ID_STADIUM, WDT_);
#endif
				DRV_SetSpeed(0, 0);
				FRTOS1_vTaskDelay(10);
			}
//Countdowm---------------------------------------------------
#if 0
			for(int i =0;i<2500;i++)
			{
				if(i%500==0)
				{
					if(i%2500==0)
					{
						BUZ_Beep(800,500);
					}
					else
					{
						BUZ_Beep(500,500);
					}

				}

#if PL_HAS_WATCHDOG
				WDT_IncTaskCntr(WDT_TASK_ID_STADIUM, 10);
#endif
				FRTOS1_vTaskDelay(2);
			}
			BUZ_Beep(900,500);
#endif
//Kampfmodus---------------------------------------------------
			while (!getState()) {
				bool turnDir = FALSE;
				bool fastSpeed = TRUE;
				bool slowSpeed = FALSE;
				int counter = 0;
//fahren bis Rand----------------------------------------------
				DRV_SetSpeed(speedL, speedR);
				int sum=getRefSum();
				while (sum > 5000) {
					if (getState() == TRUE) {
						break;
					}
					int us = US_Measure_us();
					int cm = US_usToCentimeters(us, 22);
					sum = getRefSum();
					if(sum<5000)
					{
						MOT_SetVal(MOT_GetMotorHandle(MOT_MOTOR_LEFT), 0xFFFF);
						MOT_SetVal(MOT_GetMotorHandle(MOT_MOTOR_RIGHT), 0xFFFF);
						break;
					}
					if((cm > 1) && (cm < 5) && fastSpeed)
					{
						DRV_SetSpeed(50000, 50000);
						fastSpeed = FALSE;
						slowSpeed = TRUE;
					}
					else if(slowSpeed)
					{
						DRV_SetSpeed(speedL, speedR);
						fastSpeed = TRUE;
						slowSpeed = FALSE;
					}
					sum = getRefSum();
				}
//zurückfahren------------------------------------------------
				MOT_SetVal(MOT_GetMotorHandle(MOT_MOTOR_LEFT), 0xFFFF);
				MOT_SetVal(MOT_GetMotorHandle(MOT_MOTOR_RIGHT), 0xFFFF);
				DRV_SetSpeed(-speedL, -speedR);
				ref = getWeightedValue();
				if(ref>4700)
				{

					DRV_SetSpeed(-speedL, -turnSpeed);
				}
				if(ref<2300)
				{
					DRV_SetSpeed(-turnSpeed, -speedR);
				}
				turnDir = (ref > 3500) ? FALSE : TRUE;
				for (counter = 0; counter < 320; counter++) {
					if (getState() == TRUE) {
						break;
					}
#if PL_HAS_WATCHDOG
					WDT_IncTaskCntr(WDT_TASK_ID_STADIUM, 10);
#endif
					FRTOS1_vTaskDelay(2);
				}
//drehen-------------------------------------------------------
				if (turnDir == FALSE) {
					if(ref)
					DRV_SetSpeed(-turnSpeed, turnSpeed);
				} else {
					DRV_SetSpeed(turnSpeed, -turnSpeed);
				}
				for (counter = 0; counter < 100; counter++) {
					int us = US_Measure_us();
					int cm = US_usToCentimeters(us, 22);
					if(((cm > 1) && (cm < 30)))
					{
						break;
					}
					if (getState() == TRUE) {
						break;
					}
#if PL_HAS_WATCHDOG
					WDT_IncTaskCntr(WDT_TASK_ID_STADIUM, 10);
#endif
					FRTOS1_vTaskDelay(2);
				}
				DRV_SetSpeed(speedL, speedR);
			}
		}
	}
}



//Shell-------------------------------------------------------------------------------
static void STADIUM_PrintHelp(const CLS1_StdIOType *io) {
	CLS1_SendHelpStr((unsigned char*) "stadium",
			(unsigned char*) "Group of motor commands\r\n", io->stdOut);
	CLS1_SendHelpStr((unsigned char*) "  help|status",
			(unsigned char*) "Shows motor help or status\r\n", io->stdOut);
	CLS1_SendHelpStr((unsigned char*) "  runF <duty>",
			(unsigned char*) "Change motor direction\r\n", io->stdOut);
	CLS1_SendHelpStr((unsigned char*) "  runB <duty>",
			(unsigned char*) "Change motor PWM (-100..+100)%\r\n", io->stdOut);
	CLS1_SendHelpStr((unsigned char*) "  turnL <time>",
			(unsigned char*) "Change motor PWM (-100..+100)%\r\n", io->stdOut);
	CLS1_SendHelpStr((unsigned char*) "  turnR <time>",
			(unsigned char*) "Change motor PWM (-100..+100)%\r\n", io->stdOut);
	CLS1_SendHelpStr((unsigned char*) "  fight <duty>",
			(unsigned char*) "Change motor PWM (-100..+100)%\r\n", io->stdOut);
	CLS1_SendHelpStr((unsigned char*) "  stopp",
			(unsigned char*) "Change motor PWM (-100..+100)%\r\n", io->stdOut);
	CLS1_SendHelpStr((unsigned char*) "  calib",
			(unsigned char*) "Change motor PWM (-100..+100)%\r\n", io->stdOut);
	CLS1_SendHelpStr((unsigned char*) "  find",
			(unsigned char*) "Change motor PWM (-100..+100)%\r\n", io->stdOut);
	CLS1_SendHelpStr((unsigned char*) "  pid",
			(unsigned char*) "Change motor PWM (-100..+100)%\r\n", io->stdOut);
}

static void STADIUM_PrintStatus(const CLS1_StdIOType *io) {
	unsigned char buf[32];
	StadiumMode statusstate = getState();
	CLS1_SendStatusStr((unsigned char*) "comming soon", (unsigned char*) "\r\n",
			io->stdOut);
	CLS1_SendNum8u(getState(),io->stdOut);
}

uint8_t Stadium_ParseCommand(const unsigned char *cmd, bool *handled,
		const CLS1_StdIOType *io) {
	uint8_t res = ERR_OK;
	int32_t val;
	const unsigned char *p;

	if (UTIL1_strcmp((char*) cmd, (char*) CLS1_CMD_HELP) == 0
			|| UTIL1_strcmp((char*) cmd, (char*) "stadium help") == 0) {
		STADIUM_PrintHelp(io);
		*handled = TRUE;
	} else if (UTIL1_strcmp((char*) cmd, (char*) CLS1_CMD_STATUS) == 0
			|| UTIL1_strcmp((char*) cmd, (char*) "stadium status") == 0) {
		STADIUM_PrintStatus(io);
		*handled = TRUE;
	} else if (UTIL1_strcmp((char*)cmd, (char*)"stadium fight") == 0) {

		*handled = TRUE;
	} else if (UTIL1_strcmp((char*)cmd, (char*)"stadium stop") == 0) {

		*handled = TRUE;
	} else if (UTIL1_strcmp((char*)cmd, (char*)"stadium calib") == 0) {

		*handled = TRUE;
	} else if (UTIL1_strcmp((char*)cmd, (char*)"stadium find") == 0) {

		*handled = TRUE;
	} else if (UTIL1_strcmp((char*)cmd, (char*)"stadium pid") == 0) {

		*handled = TRUE;
	} else if (UTIL1_strncmp(cmd, "Stadium speed ",
			sizeof("Stadium speed ") - 1) == 0) {
		p = cmd + sizeof("Shell val ") - 1;
		if (UTIL1_xatoi(&p, &val) == ERR_OK) {

			*handled = TRUE;
		}
		return ERR_OK;
	}
}

void Stadium_INIT() {
	if (FRTOS1_xTaskCreate(fight, "Stadium", configMINIMAL_STACK_SIZE, NULL,
			tskIDLE_PRIORITY+3, NULL) != pdPASS) {
		for (;;) {
		} /* error */
	}

}
#endif

#if 0
void FindMode() {
	while(StateMode == FIND)
	{
	int counter = 0;
	uint8_t buf[16];
	uint16_t cm, us;
	speedR=1500;
	speedL=1500;
	int enableUS = 0;
	do {
		PID_Start();
		for(counter=0;counter<5;counter++)
		{
		TACHO_CalcSpeed();
		DRV_SetSpeed(1500,-1500);
		FRTOS1_vTaskDelay(2);
		}

		us = US_Measure_us();
		cm = US_usToCentimeters(us, 22);


	} while ((cm < 1) || (cm > 50));
	counter=0;
	PID_Start();
	do {
		TACHO_CalcSpeed();
		DRV_SetSpeed(1500,-1500);
		counter++;
		FRTOS1_vTaskDelay(2);
	} while (counter<100);
	speedR=4000;
	speedL=4000;
	PID_Start();
	while ((getRefSum() > 5000)  && (StateMode == FIND)) {
		TACHO_CalcSpeed();
		DRV_SetSpeed(speedL, speedR);
		FRTOS1_vTaskDelay(2);
	}
	counter = 0;
	PID_Start();
	do {
		TACHO_CalcSpeed();
		DRV_SetSpeed(-1000, -1000);
		FRTOS1_vTaskDelay(2);
		counter++;
	} while ((counter < 1000) && (StateMode == FIND));
	}
}

#endif
