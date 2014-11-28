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
static int32_t param;
typedef enum {
	RUN_FORWARD, /*!< Motor forward direction */
	RUN_BACKWARD, /*!< Motor backward direction */
	TURN_LEFT, TURN_RIGHT, STOP, FIGHT_RUN, FIGHT_STOP, UNKNOWN, NEED_CALIB, WAIT, FIND, PID
} StadiumMode;
static StadiumMode StateMode = UNKNOWN;
void FindMode() {
	uint8_t buf[16];
	uint16_t cm, us;
	MOTS_Turn(MOT_TURN_LEFT, 20);
	do {

		us = US_Measure_us();
		//MOTS_SetSpeedPercent(0);
		cm = US_usToCentimeters(us, 22);
	} while ((cm < 2) || (cm > 8));
	MOTS_SetSpeedPercent(0);
	MOTS_SetSpeedPercent(20);
	MOTS_SetDirection((MOT_Direction) MOT_DIR_FORWARD);
	while (getRefSum() > 5000) {

	}
	StateMode = STOP;
}
static portTASK_FUNCTION(fight, pvParameters) {
	(void) pvParameters; /* not used */
	for (;;) {
		switch (StateMode) {
		case RUN_FORWARD:
			while (getRefSum() > 5000) {
				MOTS_SetSpeedPercent(param);
				MOTS_SetDirection((MOT_Direction) MOT_DIR_FORWARD);
			}
			StateMode = STOP;
			break;
		case RUN_BACKWARD:
			while (getRefSum() > 5000) {
				MOTS_SetSpeedPercent(param);
				MOTS_SetDirection((MOT_Direction) MOT_DIR_BACKWARD);
			}
			StateMode = STOP;
			break;
		case TURN_LEFT:
			MOTS_Turn(MOT_TURN_RIGHT, param);
			break;
		case TURN_RIGHT:
			MOTS_Turn(MOT_TURN_RIGHT, param);
			break;
		case STOP:
			MOTS_SetSpeedPercent(0);
			//FRTOS1_vTaskSuspend(fight);
			break;
		case FIGHT_RUN:
			fightMode();
			break;
		case FIGHT_STOP:
			MOTS_SetSpeedPercent(0);
			break;
		case UNKNOWN:
#if PL_HAS_AUTOCALIB
#if PL_HAS_LINE_SENSOR
			if (loadFromFlash() == FALSE) {
				saveToFlash();
				SHELL_SendString((unsigned char*) "INFO: No calibration data present.\r\n");
			}

#endif
#endif
			StateMode = WAIT;
			break;
		case NEED_CALIB:
			saveToFlash();
			StateMode = WAIT;
			break;
		case WAIT:
			//--------------------
			break;
		case FIND:
			FindMode();
			break;
		case PID:
			while(1)
			{
			PID_Speed(TACHO_GetSpeed(FALSE), param, FALSE);
			FRTOS1_vTaskDelay(10);
			}

		}
		FRTOS1_vTaskDelay(1);

	}
}

static void STADIUM_PrintHelp(const CLS1_StdIOType *io) {
	CLS1_SendHelpStr((unsigned char*) "stadium", (unsigned char*) "Group of motor commands\r\n", io->stdOut);
	CLS1_SendHelpStr((unsigned char*) "  help|status", (unsigned char*) "Shows motor help or status\r\n", io->stdOut);
	CLS1_SendHelpStr((unsigned char*) "  runF <duty>", (unsigned char*) "Change motor direction\r\n", io->stdOut);
	CLS1_SendHelpStr((unsigned char*) "  runB <duty>", (unsigned char*) "Change motor PWM (-100..+100)%\r\n", io->stdOut);
	CLS1_SendHelpStr((unsigned char*) "  turnL <time>", (unsigned char*) "Change motor PWM (-100..+100)%\r\n", io->stdOut);
	CLS1_SendHelpStr((unsigned char*) "  turnR <time>", (unsigned char*) "Change motor PWM (-100..+100)%\r\n", io->stdOut);
	CLS1_SendHelpStr((unsigned char*) "  fight <duty>", (unsigned char*) "Change motor PWM (-100..+100)%\r\n", io->stdOut);
	CLS1_SendHelpStr((unsigned char*) "  stopp", (unsigned char*) "Change motor PWM (-100..+100)%\r\n", io->stdOut);
	CLS1_SendHelpStr((unsigned char*) "  calib", (unsigned char*) "Change motor PWM (-100..+100)%\r\n", io->stdOut);
	CLS1_SendHelpStr((unsigned char*) "  find", (unsigned char*) "Change motor PWM (-100..+100)%\r\n", io->stdOut);
	CLS1_SendHelpStr((unsigned char*) "  pid", (unsigned char*) "Change motor PWM (-100..+100)%\r\n", io->stdOut);
}

static void STADIUM_PrintStatus(const CLS1_StdIOType *io) {
	unsigned char buf[32];

	CLS1_SendStatusStr((unsigned char*) "comming soon", (unsigned char*) "\r\n", io->stdOut);
}

uint8_t Stadium_ParseCommand(const unsigned char *cmd, bool *handled, const CLS1_StdIOType *io) {
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
	} else if (UTIL1_strncmp((char* ) cmd, (char* ) "stadium runF ", sizeof("stadium runF ") - 1)
			== 0) {
		p = cmd + sizeof("stadium runF");
		if (UTIL1_xatoi(&p, &val) == ERR_OK && val >= -100 && val <= 100) {
			param = val;
			StateMode = RUN_FORWARD;
			//FRTOS1_vTaskResume(fight);
			*handled = TRUE;
		} else {
			CLS1_SendStr((unsigned char*) "Wrong argument, must be in the range -100..100\r\n", io->stdErr);
			res = ERR_FAILED;
		}
	} else if (UTIL1_strncmp((char* ) cmd, (char* ) "stadium runB ", sizeof("stadium runB ") - 1)
			== 0) {
		p = cmd + sizeof("stadium runB");
		if (UTIL1_xatoi(&p, &val) == ERR_OK && val >= -100 && val <= 100) {
			param = val;
			StateMode = RUN_BACKWARD;
			//FRTOS1_vTaskResume(fight);
			*handled = TRUE;
		}
	} else if (UTIL1_strncmp((char* ) cmd, (char* ) "stadium turnL ", sizeof("stadium turnL ") - 1)
			== 0) {
		p = cmd + sizeof("stadium turnL");
		if (UTIL1_xatoi(&p, &val) == ERR_OK && val >= -100 && val <= 100) {
			param = val;
			StateMode = TURN_LEFT;
			//FRTOS1_vTaskResume(fight);
			*handled = TRUE;
		} else {
			CLS1_SendStr((unsigned char*) "Wrong argument, must be in the range -100..100\r\n", io->stdErr);
			res = ERR_FAILED;
		}
	} else if (UTIL1_strncmp((char* ) cmd, (char* ) "stadium turnR ", sizeof("stadium turnR ") - 1)
			== 0) {
		p = cmd + sizeof("stadium turnR");
		if (UTIL1_xatoi(&p, &val) == ERR_OK && val >= -100 && val <= 100) {
			param = val;
			StateMode = TURN_RIGHT;
			//FRTOS1_vTaskResume(fight);
			*handled = TRUE;
		} else {
			CLS1_SendStr((unsigned char*) "Wrong argument, must be in the range -100..100\r\n", io->stdErr);
			res = ERR_FAILED;
		}
	} else if (UTIL1_strncmp((char* ) cmd, (char* ) "stadium fight ", sizeof("stadium fight ")
			- 1) == 0) {
		p = cmd + sizeof("stadium fight");
		if (UTIL1_xatoi(&p, &val) == ERR_OK && val >= -100 && val <= 100) {
			param = val;
			//FRTOS1_vTaskResume(fight);
			StateMode = FIGHT_RUN;
			*handled = TRUE;
		} else {
			CLS1_SendStr((unsigned char*) "Wrong argument, must be in the range -100..100\r\n", io->stdErr);
			res = ERR_FAILED;
		}
	} else if (UTIL1_strcmp((char*)cmd, (char*)"stadium stopp") == 0) {
		StateMode = FIGHT_STOP;
		//FRTOS1_vTaskSuspend(fight);
		*handled = TRUE;
	} else if (UTIL1_strcmp((char*)cmd, (char*)"stadium calib") == 0) {
		StateMode = NEED_CALIB;
		//FRTOS1_vTaskSuspend(fight);
		*handled = TRUE;
	} else if (UTIL1_strcmp((char*)cmd, (char*)"stadium find") == 0) {
		StateMode = FIND;
		//FRTOS1_vTaskSuspend(fight);
		*handled = TRUE;
	} else if (UTIL1_strncmp((char* ) cmd, (char* ) "stadium pid ", sizeof("stadium pid ") - 1)
			== 0) {
		p = cmd + sizeof("stadium pid");
		if (UTIL1_xatoi(&p, &val) == ERR_OK && val >= -32000 && val <= 32000) {
			param = val;
			StateMode = PID;
			//FRTOS1_vTaskResume(fight);
			*handled = TRUE;
		}
	}
	return ERR_OK;
}
void fightMode() {
	MOTS_SetSpeedPercent(param);

	MOTS_SetDirection((MOT_Direction) MOT_DIR_FORWARD);
	do {
		FRTOS1_vTaskDelay(5);
	} while ((getRefSum() > 5000) && (StateMode == FIGHT_RUN));

	getWeightedValue() < 3500 ? MOTS_Turn(MOT_TURN_LEFT, param) : MOTS_Turn(MOT_TURN_RIGHT, param);

	do {
		FRTOS1_vTaskDelay(5);
	} while ((getRefSum() < 5000) && (StateMode == FIGHT_RUN));
	if (StateMode == FIGHT_RUN) {
		FRTOS1_vTaskDelay(250);
	}

}

void Stadium_INIT() {
	if (FRTOS1_xTaskCreate(fight, "Stadium", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+1, NULL) != pdPASS) {
		for (;;) {
		} /* error */
	}

}
#endif
