/**
 * \file
 * \brief Drive interface.
 * \author Erich Styger, erich.styger@hslu.ch
 *
 * This module implements a simple 'drive' task which is responsible to move the robot around.
 */
#include "Platform.h"
#if PL_HAS_DRIVE
#include "Drive.h"
#include "FRTOS1.h"
#include "Pid.h"
#include "Tacho.h"
#include "Motor.h"
#if PL_HAS_RTOS_TRACE
#include "RTOSTRC1.h"
#endif


static int32_t DRV_SpeedLeft, DRV_SpeedRight;



void DRV_SetSpeed(int32_t left, int32_t right) {
	FRTOS1_taskENTER_CRITICAL();
	DRV_SpeedLeft = left;
	DRV_SpeedRight = right;
	FRTOS1_taskEXIT_CRITICAL();
}


static portTASK_FUNCTION(DriveTask, pvParameters) {
	(void) pvParameters; /* parameter not used */

	for (;;) {
		/*! \todo extend this for your own needs and with a position PID */
		TACHO_CalcSpeed();
		{
			PID_Speed(TACHO_GetSpeed(TRUE), DRV_SpeedLeft, TRUE); /* left */
			PID_Speed(TACHO_GetSpeed(FALSE), DRV_SpeedRight, FALSE); /* right */
		}
		FRTOS1_vTaskDelay(2/portTICK_RATE_MS);
	} /* for */
}

void DRV_Deinit(void) {
	/* nothing needed */
}

void DRV_Init(void) {
	DRV_SpeedLeft = 0;
	DRV_SpeedRight = 0;
	if (FRTOS1_xTaskCreate(
			DriveTask, /* pointer to the task */
			"Drive", /* task name for kernel awareness debugging */
			configMINIMAL_STACK_SIZE, /* task stack size */
			(void*)NULL, /* optional task startup argument */
			tskIDLE_PRIORITY+2, /* initial priority */
			(xTaskHandle*)NULL /* optional task handle to create */
	) != pdPASS) {
		/*lint -e527 */
		for (;;) {
		} /* error! probably out of memory */
		/*lint +e527 */
	}

}

#endif /* PL_HAS_DRIVE */
