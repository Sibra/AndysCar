/**
 * \file
 * \brief Reflectance sensor driver implementation.
 * \author Erich Styger, erich.styger@hslu.ch
 *
 * This module implements the driver for the bot front sensor.
 */

#include "Platform.h"
#if PL_HAS_LINE_SENSOR
#include "Reflectance.h"
#include "LED_IR.h"
#include "WAIT1.h"
#include "RefCnt.h" /* timer counter to measure reflectance */
#include "IR1.h"
#include "IR2.h"
#include "IR3.h"
#include "IR4.h"
#include "IR5.h"
#include "IR6.h"
#include "UTIL1.h"
#include "FRTOS1.h"
#include "Application.h"
#include "Event.h"
#include "Shell.h"
#include "Trigger.h"
#if PL_HAS_BLUETOOTH
#include "BT1.h"
#endif
#if PL_HAS_MOTOR
#include "Motor.h"
#endif
#if PL_HAS_CONFIG_NVM
#include "NVM_Config.h"
#include "IFsh1.h"
#endif
#define REF_NOF_SENSORS 6 /* number of sensors */
/*!TODO critical section for values*/
#if PL_HAS_BLUETOOTH
/* Bluetooth stdio */
static CLS1_ConstStdIOType BT_stdio = { (CLS1_StdIO_In_FctType) BT1_StdIOReadChar, /* stdin */
(CLS1_StdIO_OutErr_FctType) BT1_StdIOSendChar, /* stdout */
(CLS1_StdIO_OutErr_FctType) BT1_StdIOSendChar, /* stderr */
BT1_StdIOKeyPressed /* if input is not empty */
};
#endif
//---------------------------------------------------------------------------

static UndergroundType stadium = ug_unknown;

typedef enum {
	REF_STATE_INIT,
	REF_STATE_NOT_CALIBRATED,
	REF_STATE_START_CALIBRATION,
	REF_STATE_CALIBRATING,
	REF_STATE_STOP_CALIBRATION,
	REF_STATE_READY
} RefStateType;
static volatile RefStateType refState = REF_STATE_INIT; /* state machine state */

static LDD_TDeviceData *timerHandle;

typedef struct SensorFctType_ {
	void (*SetOutput)(void);
	void (*SetInput)(void);
	void (*SetVal)(void);
	bool (*GetVal)(void);
} SensorFctType;


#define MAX_SENSOR_VALUE  ((SensorTimeType)-1)

/* calibration min/max values */
typedef struct SensorCalibT_ {
	SensorTimeType minVal[REF_NOF_SENSORS];
	SensorTimeType maxVal[REF_NOF_SENSORS];
} SensorCalibT;

static SensorCalibT SensorCalibMinMax; /* min/max calibration data in SRAM */
static SensorTimeType SensorRaw[REF_NOF_SENSORS]; /* raw sensor values */
static SensorTimeType SensorCalibrated[REF_NOF_SENSORS]; /* 0 means white/min value, 1000 means black/max value */
static SensorTimeType weightedValue;
static SensorTimeType RFSSum;

void saveToFlash()
{
	refState = REF_STATE_NOT_CALIBRATED;
	while(refState != REF_STATE_READY)
	{
	REF_StateMachine();
	}
	//int a = sizeof(SensorCalibMinMax);
	NVMC_SaveReflectanceData(&SensorCalibMinMax, sizeof(SensorCalibMinMax));
}
bool loadFromFlash()
{
	uint16_t* a = (uint16_t*)NVMC_GetReflectanceData();
	for(int i=0;i<REF_NOF_SENSORS;i++)
	{
		SensorCalibMinMax.minVal[i]=*(a+i);
		SensorCalibMinMax.maxVal[i]=*(a+REF_NOF_SENSORS+i);
	}
	refState = REF_STATE_READY;
	if (SensorCalibMinMax.minVal[1]>10 && SensorCalibMinMax.minVal[1]< 2000)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}

}
/* Functions as wrapper around macro. */
static void S1_SetOutput(void) {
	IR1_SetOutput();
}
static void S1_SetInput(void) {
	IR1_SetInput();
}
static void S1_SetVal(void) {
	IR1_SetVal();
}
static bool S1_GetVal(void) {
	return IR1_GetVal();
}

static void S2_SetOutput(void) {
	IR2_SetOutput();
}
static void S2_SetInput(void) {
	IR2_SetInput();
}
static void S2_SetVal(void) {
	IR2_SetVal();
}
static bool S2_GetVal(void) {
	return IR2_GetVal();
}

static void S3_SetOutput(void) {
	IR3_SetOutput();
}
static void S3_SetInput(void) {
	IR3_SetInput();
}
static void S3_SetVal(void) {
	IR3_SetVal();
}
static bool S3_GetVal(void) {
	return IR3_GetVal();
}

static void S4_SetOutput(void) {
	IR4_SetOutput();
}
static void S4_SetInput(void) {
	IR4_SetInput();
}
static void S4_SetVal(void) {
	IR4_SetVal();
}
static bool S4_GetVal(void) {
	return IR4_GetVal();
}

static void S5_SetOutput(void) {
	IR5_SetOutput();
}
static void S5_SetInput(void) {
	IR5_SetInput();
}
static void S5_SetVal(void) {
	IR5_SetVal();
}
static bool S5_GetVal(void) {
	return IR5_GetVal();
}

static void S6_SetOutput(void) {
	IR6_SetOutput();
}
static void S6_SetInput(void) {
	IR6_SetInput();
}
static void S6_SetVal(void) {
	IR6_SetVal();
}
static bool S6_GetVal(void) {
	return IR6_GetVal();
}

static const SensorFctType SensorFctArray[REF_NOF_SENSORS] = { { S1_SetOutput, S1_SetInput,
		S1_SetVal, S1_GetVal }, { S2_SetOutput, S2_SetInput, S2_SetVal, S2_GetVal }, { S3_SetOutput,
		S3_SetInput, S3_SetVal, S3_GetVal }, { S4_SetOutput, S4_SetInput, S4_SetVal, S4_GetVal }, {
		S5_SetOutput, S5_SetInput, S5_SetVal, S5_GetVal }, { S6_SetOutput, S6_SetInput, S6_SetVal,
		S6_GetVal }, };

static void REF_MeasureRaw(SensorTimeType raw[REF_NOF_SENSORS]) {
	uint8_t cnt; /* number of sensor */
	uint8_t i;
	static int black = 0;

	LED_IR_On(); /* IR LED's on */
	WAIT1_Waitus(200); /*! \todo adjust time as needed */ //warten bis IR brennen

	for (i = 0; i < REF_NOF_SENSORS; i++) {
		SensorFctArray[i].SetOutput(); /* turn I/O line as output */
		SensorFctArray[i].SetVal(); /* put high */
		raw[i] = MAX_SENSOR_VALUE;
	}
	WAIT1_Waitus(50); /* give some time to charge the capacitor */
	(void) RefCnt_ResetCounter(timerHandle); /* reset timer counter */
	for (i = 0; i < REF_NOF_SENSORS; i++) {
		SensorFctArray[i].SetInput(); /* turn I/O line as input */
	}
	do {
		/*! \todo Be aware that this might block for a long time, if discharging takes long. Consider using a timeout. */
		cnt = 0;
		for (i = 0; i < REF_NOF_SENSORS; i++) {
			if (raw[i] == MAX_SENSOR_VALUE) { /* not measured yet? */
				if (SensorFctArray[i].GetVal() == 0) {
					raw[i] = RefCnt_GetCounterValue(timerHandle);
				} else if (RefCnt_GetCounterValue(timerHandle) > 0x5000) {
					black++;
					cnt++;
				}
			} else { /* have value */
				cnt++;
			}
		}
		//SHELL_SendString((unsigned char*) raw[0]);
	} while (cnt != REF_NOF_SENSORS);
	LED_IR_Off();
}

static void REF_CalibrateMinMax(SensorTimeType min[REF_NOF_SENSORS], SensorTimeType max[REF_NOF_SENSORS], SensorTimeType raw[REF_NOF_SENSORS]) {
	int i;

	REF_MeasureRaw(raw);
	for (i = 0; i < REF_NOF_SENSORS; i++) {
		if (raw[i] < min[i]) {
			min[i] = raw[i];
		}
		if (raw[i] > max[i]) {
			max[i] = raw[i];
		}
	}
}

static void ReadCalibrated(SensorTimeType calib[REF_NOF_SENSORS], SensorTimeType raw[REF_NOF_SENSORS]) {
	int i;
	int32_t x, denominator;
	int32_t avg_calc = 0;  //wird benötigt um avg zu berechnen, uint16 zu klein
	int sum = 0;

	REF_MeasureRaw(raw);
	RFSSum = 0;
	for (i = 0; i < REF_NOF_SENSORS; i++) {
		x = 0;
		denominator = SensorCalibMinMax.maxVal[i] - SensorCalibMinMax.minVal[i];
		if (denominator != 0) {
			x = (((int32_t) raw[i] - SensorCalibMinMax.minVal[i]) * 1000) / denominator;
		}
		if (x < 0) {
			x = 0;
		} else if (x > 1000) {
			x = 1000;
		}
		calib[i] = x;
	}
	avg_calc = 0;
	for (i = 0; i < REF_NOF_SENSORS; i++) {
		avg_calc += (calib[i] * ((i + 1) * 1000));
		RFSSum += calib[i];
	}
	weightedValue = avg_calc / RFSSum;
}

static void REF_Measure(void) {
	ReadCalibrated(SensorCalibrated, SensorRaw);
}

static uint8_t PrintHelp(const CLS1_StdIOType *io) {
	CLS1_SendHelpStr((unsigned char*) "ref", (unsigned char*) "Group of Reflectance commands\r\n", io->stdOut);
	CLS1_SendHelpStr((unsigned char*) "  help|status", (unsigned char*) "Print help or status information\r\n", io->stdOut);
	return ERR_OK;
}

static unsigned char*REF_GetStateString(void) {
	switch (refState) {
	case REF_STATE_INIT:
		return (unsigned char*) "INIT";
	case REF_STATE_NOT_CALIBRATED:
		return (unsigned char*) "NOT CALIBRATED";
	case REF_STATE_START_CALIBRATION:
		return (unsigned char*) "START CALIBRATION";
	case REF_STATE_CALIBRATING:
		return (unsigned char*) "CALIBRATING";
	case REF_STATE_STOP_CALIBRATION:
		return (unsigned char*) "STOP CALIBRATION";
	case REF_STATE_READY:
		return (unsigned char*) "READY";
	default:
		break;
	} /* switch */
	return (unsigned char*) "UNKNOWN";
}

static uint8_t PrintStatus(const CLS1_StdIOType *io) {
	unsigned char buf[24];
	int i;

	CLS1_SendStatusStr((unsigned char*) "reflectance", (unsigned char*) "\r\n", io->stdOut);

	CLS1_SendStatusStr((unsigned char*) "  state", REF_GetStateString(), io->stdOut);
	CLS1_SendStr((unsigned char*) "\r\n", io->stdOut);

	CLS1_SendStatusStr((unsigned char*) "  raw val", (unsigned char*) "", io->stdOut);
	for (i = 0; i < REF_NOF_SENSORS; i++) {
		if (i == 0) {
			CLS1_SendStr((unsigned char*) "0x", io->stdOut);
		} else {
			CLS1_SendStr((unsigned char*) " 0x", io->stdOut);
		}
		buf[0] = '\0';
		UTIL1_strcatNum16Hex(buf, sizeof(buf), SensorRaw[i]);
		CLS1_SendStr(buf, io->stdOut);
	}
	CLS1_SendStr((unsigned char*) "\r\n", io->stdOut);

	CLS1_SendStatusStr((unsigned char*) "  min val", (unsigned char*) "", io->stdOut);
	for (i = 0; i < REF_NOF_SENSORS; i++) {
		if (i == 0) {
			CLS1_SendStr((unsigned char*) "0x", io->stdOut);
		} else {
			CLS1_SendStr((unsigned char*) " 0x", io->stdOut);
		}
		buf[0] = '\0';
		UTIL1_strcatNum16Hex(buf, sizeof(buf), SensorCalibMinMax.minVal[i]);
		CLS1_SendStr(buf, io->stdOut);
	}
	CLS1_SendStr((unsigned char*) "\r\n", io->stdOut);
	CLS1_SendStatusStr((unsigned char*) "  max val", (unsigned char*) "", io->stdOut);
	for (i = 0; i < REF_NOF_SENSORS; i++) {
		if (i == 0) {
			CLS1_SendStr((unsigned char*) "0x", io->stdOut);
		} else {
			CLS1_SendStr((unsigned char*) " 0x", io->stdOut);
		}
		buf[0] = '\0';
		UTIL1_strcatNum16Hex(buf, sizeof(buf), SensorCalibMinMax.maxVal[i]);
		CLS1_SendStr(buf, io->stdOut);
	}
	CLS1_SendStr((unsigned char*) "\r\n", io->stdOut);

	CLS1_SendStatusStr((unsigned char*) "  calib val", (unsigned char*) "", io->stdOut);
	for (i = 0; i < REF_NOF_SENSORS; i++) {
		if (i == 0) {
			CLS1_SendStr((unsigned char*) "0x", io->stdOut);
		} else {
			CLS1_SendStr((unsigned char*) " 0x", io->stdOut);
		}
		buf[0] = '\0';
		UTIL1_strcatNum16Hex(buf, sizeof(buf), SensorCalibrated[i]);
		CLS1_SendStr(buf, io->stdOut);
	}
	CLS1_SendStr((unsigned char*) "\r\n", io->stdOut);
	return ERR_OK;
}

byte REF_ParseCommand(const unsigned char *cmd, bool *handled, const CLS1_StdIOType *io) {
	if (UTIL1_strcmp((char*)cmd, CLS1_CMD_HELP) == 0 || UTIL1_strcmp((char*)cmd, "ref help") == 0) {
		*handled = TRUE;
		return PrintHelp(io);
	} else if ((UTIL1_strcmp((char*)cmd, CLS1_CMD_STATUS) == 0)
			|| (UTIL1_strcmp((char*)cmd, "ref status") == 0)) {
		*handled = TRUE;
		return PrintStatus(io);
	}
	return ERR_OK;
}

void REF_StateMachine(void) {
	int i;

	switch (refState) {
	case REF_STATE_INIT:
		SHELL_SendString((unsigned char*) "INFO: No calibration data present.\r\n");
		refState = REF_STATE_NOT_CALIBRATED;
	//	break;

	case REF_STATE_NOT_CALIBRATED:
		REF_MeasureRaw(SensorRaw);
//		if (EVNT_EventIsSet(EVNT_REF_START_STOP_CALIBRATION)) {
//			EVNT_ClearEvent(EVNT_REF_START_STOP_CALIBRATION);
//			refState = REF_STATE_START_CALIBRATION;
	//		break;
	//	}
//		break;

	case REF_STATE_START_CALIBRATION:
		SHELL_SendString((unsigned char*) "start calibration...\r\n");
		for (i = 0; i < REF_NOF_SENSORS; i++) {
			SensorCalibMinMax.minVal[i] = MAX_SENSOR_VALUE;
			SensorCalibMinMax.maxVal[i] = 0;
			SensorCalibrated[i] = 0;
		}
		refState = REF_STATE_CALIBRATING;
//		break;

	case REF_STATE_CALIBRATING:
		REF_CalibrateMinMax(SensorCalibMinMax.minVal, SensorCalibMinMax.maxVal, SensorRaw);
		MOTS_SetSpeedPercent((MOT_SpeedPercent) 10);
		MOTS_SetDirection((MOT_Direction) MOT_DIR_BACKWARD);
		FRTOS1_vTaskDelay(1000);
		MOTS_SetSpeedPercent((MOT_SpeedPercent) 0);
		REF_CalibrateMinMax(SensorCalibMinMax.minVal, SensorCalibMinMax.maxVal, SensorRaw);
		refState = REF_STATE_STOP_CALIBRATION;
//		break;

	case REF_STATE_STOP_CALIBRATION:
		SHELL_SendString((unsigned char*) "...stopping calibration.\r\n");
		refState = REF_STATE_READY;
	//	break;

	case REF_STATE_READY:
		REF_Measure();
		if (EVNT_EventIsSet(EVNT_REF_START_STOP_CALIBRATION)) {
			EVNT_ClearEvent(EVNT_REF_START_STOP_CALIBRATION);
			refState = REF_STATE_START_CALIBRATION;
		}
		break;
	} /* switch */
}

//static portTASK_FUNCTION(ReflTask, pvParameters) {
//	(void) pvParameters; /* not used */
//	for (;;) {
//		REF_StateMachine();
//		FRTOS1_vTaskDelay(10/portTICK_RATE_MS);
//	}
//}
SensorTimeType getRefSum() {
	ReadCalibrated(SensorCalibrated, SensorRaw);
	return RFSSum;
}
SensorTimeType getWeightedValue() {
	ReadCalibrated(SensorCalibrated, SensorRaw);
	return weightedValue;
}
void REF_Deinit(void) {
}

void REF_Init(void) {
	refState = REF_STATE_INIT;
	timerHandle = RefCnt_Init(NULL);
	/*! \todo You might need to adjust priority or other task settings */
//	if (FRTOS1_xTaskCreate(ReflTask, "Refl", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+1, NULL) != pdPASS) {
//		for (;;) {
//		} /* error */
//	}
}
#endif /* PL_HAS_LINE_SENSOR */
