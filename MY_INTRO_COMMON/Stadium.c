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
#define TURNSPEEDFKT 2

int16_t speedR=5000;
int16_t speedL=5000;

static StadiumMode StateMode = NEED_CALIB;

//action movements-------------------------------------------------------------------
void DRV_SetSpeed(int32_t left, int32_t right) {
	PID_Speed(TACHO_GetSpeed(TRUE), left, TRUE); /* left */
	PID_Speed(TACHO_GetSpeed(FALSE), right, FALSE); /* right */

}
void setState(StadiumMode state) {
	StateMode = state;
}
void setStadiumSpeed(int16_t speed) {
	speedR = speed;
	speedL = speed;
}

void remoteControl(uint8_t speedType) {
	uint8_t counter = 0;
	if ((getRefSum() > 5000)) {
		if (((speedType & 0xF0) >> 4 == 0x07) && ((speedType & 0x0F) == 0x07)) {
			PID_Start();
		}
		TACHO_CalcSpeed();
		DRV_SetSpeed(((int16_t) ((speedType & 0xF0) >> 4) - 0x07) * 400,
				((int16_t) (speedType & 0x0F) - 0x07) * 400);
	}
	else  {
		PID_Start();
		counter=0;
		while(StateMode == REMOTE_CONTROL && counter < 200)
		{
			TACHO_CalcSpeed();
			PID_Speed(TACHO_GetSpeed(TRUE), -1000, TRUE); /* left */
			PID_Speed(TACHO_GetSpeed(FALSE), -1000, FALSE); /* right */
			counter++;
			FRTOS1_vTaskDelay(2);
		}
	}
#if 0
	do {
		TACHO_CalcSpeed();
		DRV_SetSpeed(-speedL, -speedR);
		FRTOS1_vTaskDelay(2);
		counter++;
	} while ((counter < 200 / (speedR / 100)) && (StateMode == FIGHT));
#endif

}
#if PL_IS_ROBO
static void turn(MOT_Turn_Direction dir) {
	if (dir == MOT_TURN_LEFT) {
		DRV_SetSpeed(-speedL, speedR);
	} else {
		DRV_SetSpeed(speedL, -speedR);
	}
}
void FindMode() {
	int counter = 0;
	uint8_t buf[16];
	uint16_t cm, us;
	speedR=2000;
	speedL=2000;
	int enableUS = 0;
	do {

		for(counter=0;counter<5;counter++)
		{
		turn(MOT_TURN_LEFT);
		FRTOS1_vTaskDelay(2);
		}

		us = US_Measure_us();
		cm = US_usToCentimeters(us, 22);


	} while ((cm < 1) || (cm > 30));
	counter=0;
	do {
		turn(MOT_TURN_LEFT);
		counter++;
		FRTOS1_vTaskDelay(2);
	} while (counter<30);
	speedR=4000;
	speedL=4000;
	while ((getRefSum() > 5000)  && (StateMode == FIND)) {
		TACHO_CalcSpeed();
		DRV_SetSpeed(speedL, speedR);
		FRTOS1_vTaskDelay(2);
	}
	counter = 0;
	do {
		TACHO_CalcSpeed();
		DRV_SetSpeed(-speedL, -speedR);
		FRTOS1_vTaskDelay(2);
		counter++;
	} while ((counter < 500) && (StateMode == FIND));
}

void fightMode() {
	while (StateMode == FIGHT) {
		int counter = 0;
		do {
			TACHO_CalcSpeed();
			DRV_SetSpeed(speedL, speedR);
			FRTOS1_vTaskDelay(2);
		} while ((getRefSum() > 5000) && (StateMode == FIGHT)); //fahren bis Rand
		for (counter = 0; counter < 200; counter++) {
			TACHO_CalcSpeed();
			DRV_SetSpeed(-speedL, -speedR);
			FRTOS1_vTaskDelay(2);
			if (StateMode != FIGHT) {
				return;
			}
		} // zurückfahren
		for (counter = 0; counter < 100; counter++) {
			TACHO_CalcSpeed();
			DRV_SetSpeed(-speedL, speedR);
			FRTOS1_vTaskDelay(2);
			if (StateMode != FIGHT) {
				return;
			}
		}
		counter = 0;
#if 1
		do {
			TACHO_CalcSpeed();
			DRV_SetSpeed(-speedL, speedR);
			FRTOS1_vTaskDelay(2);
			counter++;
		}while ((counter < 4000/(speedR/100)) && (StateMode == FIGHT));
#endif
	}
}

//RTOS-------------------------------------------------------------------------------

static portTASK_FUNCTION(fight, pvParameters) {
	(void) pvParameters; /* not used */
	for (;;) {
		switch (StateMode) {

		case STOP:
			SHELL_SendString((unsigned char*) "INFO: Stopp.\r\n");

			do {
				PID_Start();
				PID_Speed(0, 0, TRUE); /* left */
				PID_Speed(0, 0, FALSE); /* right */
				FRTOS1_vTaskDelay(2);
			} while (StateMode == STOP);
		case FIGHT: //sumo event
			fightMode();
			break;
		case NEED_CALIB:
			saveToFlash();
#if PL_HAS_AUTOCALIB
#if PL_HAS_LINE_SENSOR
			if (loadFromFlash() == FALSE) {
				saveToFlash();
				SHELL_SendString(
						(unsigned char*) "INFO: No calib in flash\r\n");
			}

#endif
#endif
			StateMode = STOP;
			break;
		case FIND: //testat
			FindMode();
			speedR=5000;
			speedL=5000;
			break;
		case PID: //test
#if 0
			while (StateMode == PID) {
				TACHO_CalcSpeed();
				DRV_SetSpeed(speedL,speedR);
				FRTOS1_vTaskDelay(2);
			}
#endif
		case REMOTE_CONTROL:
					while (StateMode == REMOTE_CONTROL) {
						FRTOS1_taskYIELD();
								}

					break;
		}

		FRTOS1_vTaskDelay(1);

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
		setState(FIGHT);
		*handled = TRUE;
	} else if (UTIL1_strcmp((char*)cmd, (char*)"stadium stop") == 0) {
		setState(STOP);
		*handled = TRUE;
	} else if (UTIL1_strcmp((char*)cmd, (char*)"stadium calib") == 0) {
		setState(NEED_CALIB);
		*handled = TRUE;
	} else if (UTIL1_strcmp((char*)cmd, (char*)"stadium find") == 0) {
		setState(FIND);
		*handled = TRUE;
	} else if (UTIL1_strcmp((char*)cmd, (char*)"stadium pid") == 0) {
		setState(PID);
		*handled = TRUE;
	} else if (UTIL1_strncmp(cmd, "Stadium speed ",
			sizeof("Stadium speed ") - 1) == 0) {
		p = cmd + sizeof("Shell val ") - 1;
		if (UTIL1_xatoi(&p, &val) == ERR_OK) {
			setStadiumSpeed(val);
			*handled = TRUE;
		}
		return ERR_OK;
	}
}

void Stadium_INIT() {
	if (FRTOS1_xTaskCreate(fight, "Stadium", configMINIMAL_STACK_SIZE, NULL,
			tskIDLE_PRIORITY+1, NULL) != pdPASS) {
		for (;;) {
		} /* error */
	}

}
#endif
#endif
