/**
 * \file
 * \brief Accelerometer Module
 * \author Erich Styger, erich.styger@hslu.ch
 *
 * Module to calculate the speed based on the quadrature counter.
 */

#include "Platform.h" /* interface to the platform */
#include "Application.h"
#if PL_HAS_ACCEL
#include "Accel.h"    /* our own interface */
#include "MMA1.h"     /* interface to accelerometer */
#include "LED.h"
#include "Shell.h"
#if PL_HAS_RTOS
#include "RTOS.h"
#include "FRTOS1.h"
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
#include "AD1.h"

#define APP_dstAddr 			(255)
#define RPHY_PACKET_FLAGS_NONE 	(0)

static int8_t ToSigned8Bit(uint16_t val) {
	int tmp;
	tmp = ((int) ((val >> 12) & 0x0F)-7);

	return (int8_t) tmp;
}

void ACCEL_GetValues(int16_t *x, int16_t *y, int16_t *z) {
	int16_t xmg, ymg, zmg;

	xmg = MMA1_GetXmg();
	ymg = MMA1_GetYmg();
	zmg = MMA1_GetZmg();
	*x = xmg;
	*y = ymg;
	*z = zmg;
}

void ACCEL_Deinit(void) {
	(void) MMA1_Deinit();
}

uint8_t ACCEL_LowLevelInit(void) {
	uint8_t res;

	res = MMA1_Init(); /* this might communicate to the device using I2C, so it needs interrupts enabled */
	if (res != ERR_OK) {
		return res;
	}
	res = ACCEL_Enable();
	if (res != ERR_OK) {
		return res;
	}
	res = MMA1_SetFastMode(FALSE); /* disable fast mode of the sensor: otherwise we cannot read individual sensor values */
	if (res != ERR_OK) {
		return res;
	}
	return ERR_OK;
}

uint8_t ACCEL_Enable(void) {
	uint8_t res;

	res = MMA1_Enable();
	return res;
}

uint8_t ACCEL_isEnabled(bool *isEnabled) {
	uint8_t res;

	res = MMA1_isEnabled(isEnabled);
	return res;
}
static portTASK_FUNCTION(Accel, pvParameters) {
	static bool enable = FALSE;
	(void) pvParameters; /* not used */
#if PL_HAS_ACCEL
	if (enable == FALSE) {
		ACCEL_LowLevelInit();
		ACCEL_Enable();
		enable = TRUE;
	}
#endif
	int16_t accel_val[3];
	for (;;) {
		if (getState() == REMOTE_CONTROL) {
			if (getRemoteMode() == 0) {
				ACCEL_GetValues(&accel_val[0], &accel_val[1], &accel_val[2]);
				int8_t speedR = (accel_val[1] / 125) - (accel_val[0] / 125) + 7;
				if (speedR > 15) {
					speedR = 15;
				}
				if (speedR < 0) {
					speedR = 0;
				}
				int8_t speedL = (accel_val[1] / 125) + (accel_val[0] / 125) + 7;
				if (speedL > 15) {
					speedL = 15;
				}
				if (speedL < 0) {
					speedL = 0;
				}
				int8_t val8 = ((speedL << 4) + speedR);

				RAPP_SendPayloadDataBlock(&val8, sizeof(val8),
						RAPP_MSG_TYPE_REMOTE_RUN, APP_dstAddr,
						RPHY_PACKET_FLAGS_NONE);
			} else {
#if 1
				uint8_t res;
				uint16_t values[2];
				res = AD1_Measure(TRUE);
				if (res != ERR_OK) {
					for (;;);
				}
				res = AD1_GetValue16(&values[0]);
				if (res != ERR_OK) {
					for (;;);
				}
				int x =ToSigned8Bit(values[0])-1;
				int y =ToSigned8Bit(values[1])-1;
				int8_t speedL = y+x+7;
				if (speedL > 15) {
					speedL = 15;
				}
				if (speedL < 0) {
					speedL = 0;
				}
				int8_t speedR = y-x+7;
				if (speedR > 15) {
					speedR = 15;
				}
				if (speedR < 0) {
					speedR = 0;
				}

				int8_t val8 = ((speedL << 4) + speedR);
				RAPP_SendPayloadDataBlock(&val8, sizeof(val8),
										RAPP_MSG_TYPE_REMOTE_RUN, APP_dstAddr,
										RPHY_PACKET_FLAGS_NONE);
#endif
			}

		}
		FRTOS1_vTaskDelay(2);
	}

}




/*! \brief Initialises module */
void ACCEL_Init(void) {
#if !PL_HAS_RTOS /* with an RTOS, the interrupts are disabled here. Need to do this in a task */
	ACCEL_LowLevelInit();
#endif
#if PL_HAS_ACCEL
	if (FRTOS1_xTaskCreate(Accel, (signed portCHAR *)"AccelCheck",
			configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+2, NULL) != pdPASS) {
		for (;;) {
		} /* error */
	}
#endif
}
#endif /* PL_HAS_ACCEL */

