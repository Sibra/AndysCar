/**
 * \file
 * \brief This is the implementation of the PID Module
 * \author Erich Styger, erich.styger@hslu.ch
 */

#include "Platform.h"
#if PL_HAS_PID
#include "Pid.h"
#include "Motor.h"
#include "UTIL1.h"
#if PL_HAS_SHELL
  #include "CLS1.h"
#endif
#include "Reflectance.h"

typedef struct {
  int32_t pFactor100;
  int32_t iFactor100;
  int32_t dFactor100;
  int32_t iAntiWindup;
  int32_t lastError;
  int32_t integral;
} PID_Config;

#define PID_BACK_CALCULATING_ANTIWINDUP   0

/*! \todo Add your own additional configurations as needed, at least with a position config */
static PID_Config speedLeftConfig, speedRightConfig;
static PID_Config posLeftConfig, posRightConfig;

static int32_t PID(int32_t currVal, int32_t setVal, PID_Config *config) {
  int32_t error;
  int32_t pid;
  
  /* perform PID closed control loop calculation */
  error = setVal-currVal; /* calculate error */
  pid = (error*config->pFactor100)/100; /* P part */
  config->integral += error; /* integrate error */
#if !PID_BACK_CALCULATING_ANTIWINDUP
  if (config->integral>config->iAntiWindup) {
    config->integral = config->iAntiWindup;
  } else if (config->integral<-config->iAntiWindup) {
    config->integral = -config->iAntiWindup;
  }
#endif
  pid += (config->integral*config->iFactor100)/100; /* add I part */
  pid += ((error-config->lastError)*config->dFactor100)/100; /* add D part */
  config->lastError = error; /* remember for next iteration of D part */
  return pid;
}

void PID_SpeedCfg(int32_t currSpeed, int32_t setSpeed, bool isLeft, PID_Config *config) {
  int32_t speed;
  MOT_Direction direction=MOT_DIR_FORWARD;
  MOT_MotorDevice *motHandle;
  
  speed = PID(currSpeed, setSpeed, config);
#if PID_BACK_CALCULATING_ANTIWINDUP
  if (speed>0 && speed>0xFFFF) {
    config->integral -= ;
  }
#endif
  if (speed>=0) {
    direction = MOT_DIR_FORWARD;
  } else { /* negative, make it positive */
    speed = -speed; /* make positive */
    direction = MOT_DIR_BACKWARD;
  }
  /* speed shall be positive here, make sure it is within 16bit PWM boundary */
#if !PID_BACK_CALCULATING_ANTIWINDUP
  if (speed>0xFFFF) {
    speed = 0xFFFF;
  }
#endif
  /* send new speed values to motor */
  if (isLeft) {
    motHandle = MOT_GetMotorHandle(MOT_MOTOR_LEFT);
  } else {
    motHandle = MOT_GetMotorHandle(MOT_MOTOR_RIGHT);
  }
#if 1
  MOT_SetVal(motHandle, 0xFFFF-speed); /* PWM is low active */
  MOT_SetDirection(motHandle, direction);
  MOT_UpdatePercent(motHandle, direction);
#else
  MOT_SetSpeedPercent(motHandle, (MOT_SpeedPercent) percent);
#endif
}

void PID_Speed(int32_t currSpeed, int32_t setSpeed, bool isLeft) {
  if (isLeft) {
    PID_SpeedCfg(currSpeed, setSpeed, isLeft, &speedLeftConfig);
  } else {
    PID_SpeedCfg(currSpeed, setSpeed, isLeft, &speedRightConfig);
  }
}

void PID_PosCfg(int32_t currPos, int32_t setPos, bool isLeft, PID_Config *config) {
  int32_t pwm;
  MOT_Direction direction=MOT_DIR_FORWARD;
  MOT_MotorDevice *motHandle;

  pwm = PID(currPos, setPos, config);
  /* transform into motor pwm */
  pwm *= 1000; /* scale PID, otherwise we need high PID constants */
  if (pwm>=0) {
    direction = MOT_DIR_FORWARD;
  } else { /* negative, make it positive */
    pwm = -pwm; /* make positive */
    direction = MOT_DIR_BACKWARD;
  }
  /* pwm is now always positive, make sure it is within 16bit PWM boundary */
  if (pwm>0xFFFF) {
    pwm = 0xFFFF;
  }
  /* send new pwm values to motor */
  if (isLeft) {
    motHandle = MOT_GetMotorHandle(MOT_MOTOR_LEFT);
  } else {
    motHandle = MOT_GetMotorHandle(MOT_MOTOR_RIGHT);
  }
  MOT_SetVal(motHandle, 0xFFFF-pwm); /* PWM is low active */
  MOT_SetDirection(motHandle, direction);
  MOT_UpdatePercent(motHandle, direction);
}

void PID_Pos(int32_t currPos, int32_t setPos, bool isLeft) {
  if (isLeft) {
    PID_PosCfg(currPos, setPos, isLeft, &posLeftConfig);
  } else {
    PID_PosCfg(currPos, setPos, isLeft, &posRightConfig);
  }
}

#if PL_HAS_SHELL
static void PID_PrintHelp(const CLS1_StdIOType *io) {
  CLS1_SendHelpStr((unsigned char*)"pid", (unsigned char*)"Group of PID commands\r\n", io->stdOut);
  CLS1_SendHelpStr((unsigned char*)"  help|status", (unsigned char*)"Shows PID help or status\r\n", io->stdOut);
  CLS1_SendHelpStr((unsigned char*)"  speed (L|R) (p|d|i|w) <value>", (unsigned char*)"Sets P, D, I or anti-windup position value\r\n", io->stdOut);
  CLS1_SendHelpStr((unsigned char*)"  pos (L|R) (p|d|i|w) <value>", (unsigned char*)"Sets P, D, I or anti-windup position value\r\n", io->stdOut);
}

static void PrintPIDstatus(PID_Config *config, const unsigned char *kindStr, const CLS1_StdIOType *io) {
  unsigned char buf[48];
  unsigned char kindBuf[16];

  UTIL1_strcpy(kindBuf, sizeof(buf), (unsigned char*)"  ");
  UTIL1_strcat(kindBuf, sizeof(buf), kindStr);
  UTIL1_strcat(kindBuf, sizeof(buf), (unsigned char*)" PID");
  UTIL1_strcpy(buf, sizeof(buf), (unsigned char*)"p: ");
  UTIL1_strcatNum32s(buf, sizeof(buf), config->pFactor100);
  UTIL1_strcat(buf, sizeof(buf), (unsigned char*)" i: ");
  UTIL1_strcatNum32s(buf, sizeof(buf), config->iFactor100);
  UTIL1_strcat(buf, sizeof(buf), (unsigned char*)" d: ");
  UTIL1_strcatNum32s(buf, sizeof(buf), config->dFactor100);
  UTIL1_strcat(buf, sizeof(buf), (unsigned char*)"\r\n");
  CLS1_SendStatusStr(kindBuf, buf, io->stdOut);

  UTIL1_strcpy(kindBuf, sizeof(buf), (unsigned char*)"  ");
  UTIL1_strcat(kindBuf, sizeof(buf), kindStr);
  UTIL1_strcat(kindBuf, sizeof(buf), (unsigned char*)" windup");
  UTIL1_Num32sToStr(buf, sizeof(buf), config->iAntiWindup);
  UTIL1_strcat(buf, sizeof(buf), (unsigned char*)"\r\n");
  CLS1_SendStatusStr(kindBuf, buf, io->stdOut);

  UTIL1_strcpy(kindBuf, sizeof(buf), (unsigned char*)"  ");
  UTIL1_strcat(kindBuf, sizeof(buf), kindStr);
  UTIL1_strcat(kindBuf, sizeof(buf), (unsigned char*)" error");
  UTIL1_Num32sToStr(buf, sizeof(buf), config->lastError);
  UTIL1_strcat(buf, sizeof(buf), (unsigned char*)"\r\n");
  CLS1_SendStatusStr(kindBuf, buf, io->stdOut);

  UTIL1_strcpy(kindBuf, sizeof(buf), (unsigned char*)"  ");
  UTIL1_strcat(kindBuf, sizeof(buf), kindStr);
  UTIL1_strcat(kindBuf, sizeof(buf), (unsigned char*)" integral");
  UTIL1_Num32sToStr(buf, sizeof(buf), config->integral);
  UTIL1_strcat(buf, sizeof(buf), (unsigned char*)"\r\n");
  CLS1_SendStatusStr(kindBuf, buf, io->stdOut);
}

static void PID_PrintStatus(const CLS1_StdIOType *io) {
  CLS1_SendStatusStr((unsigned char*)"pid", (unsigned char*)"\r\n", io->stdOut);
  PrintPIDstatus(&speedLeftConfig, (unsigned char*)"speed L", io);
  PrintPIDstatus(&speedRightConfig, (unsigned char*)"speed R", io);
  PrintPIDstatus(&posLeftConfig, (unsigned char*)"pos L", io);
  PrintPIDstatus(&posRightConfig, (unsigned char*)"pos R", io);
}

static uint8_t ParsePidParameter(PID_Config *config, const unsigned char *cmd, bool *handled, const CLS1_StdIOType *io) {
  const unsigned char *p;
  uint32_t val32u;
  uint8_t res = ERR_OK;

  if (UTIL1_strncmp((char*)cmd, (char*)"p ", sizeof("p ")-1)==0) {
    p = cmd+sizeof("p");
    if (UTIL1_ScanDecimal32uNumber(&p, &val32u)==ERR_OK) {
      config->pFactor100 = val32u;
      *handled = TRUE;
    } else {
      CLS1_SendStr((unsigned char*)"Wrong argument\r\n", io->stdErr);
      res = ERR_FAILED;
    }
  } else if (UTIL1_strncmp((char*)cmd, (char*)"i ", sizeof("i ")-1)==0) {
    p = cmd+sizeof("i");
    if (UTIL1_ScanDecimal32uNumber(&p, &val32u)==ERR_OK) {
      config->iFactor100 = val32u;
      *handled = TRUE;
    } else {
      CLS1_SendStr((unsigned char*)"Wrong argument\r\n", io->stdErr);
      res = ERR_FAILED;
    }
  } else if (UTIL1_strncmp((char*)cmd, (char*)"d ", sizeof("d ")-1)==0) {
    p = cmd+sizeof("d");
    if (UTIL1_ScanDecimal32uNumber(&p, &val32u)==ERR_OK) {
      config->dFactor100 = val32u;
      *handled = TRUE;
    } else {
      CLS1_SendStr((unsigned char*)"Wrong argument\r\n", io->stdErr);
      res = ERR_FAILED;
    }
  } else if (UTIL1_strncmp((char*)cmd, (char*)"w ", sizeof("w ")-1)==0) {
    p = cmd+sizeof("w");
    if (UTIL1_ScanDecimal32uNumber(&p, &val32u)==ERR_OK) {
      config->iAntiWindup = val32u;
      *handled = TRUE;
    } else {
      CLS1_SendStr((unsigned char*)"Wrong argument\r\n", io->stdErr);
      res = ERR_FAILED;
    }
  }
  return res;
}

uint8_t PID_ParseCommand(const unsigned char *cmd, bool *handled, const CLS1_StdIOType *io) {
  uint8_t res = ERR_OK;

  if (UTIL1_strcmp((char*)cmd, (char*)CLS1_CMD_HELP)==0 || UTIL1_strcmp((char*)cmd, (char*)"pid help")==0) {
    PID_PrintHelp(io);
    *handled = TRUE;
  } else if (UTIL1_strcmp((char*)cmd, (char*)CLS1_CMD_STATUS)==0 || UTIL1_strcmp((char*)cmd, (char*)"pid status")==0) {
    PID_PrintStatus(io);
    *handled = TRUE;
  } else if (UTIL1_strncmp((char*)cmd, (char*)"pid speed L ", sizeof("pid speed L ")-1)==0) {
    res = ParsePidParameter(&speedLeftConfig, cmd+sizeof("pid speed L ")-1, handled, io);
  } else if (UTIL1_strncmp((char*)cmd, (char*)"pid speed R ", sizeof("pid speed R ")-1)==0) {
    res = ParsePidParameter(&speedRightConfig, cmd+sizeof("pid speed R ")-1, handled, io);
  } else if (UTIL1_strncmp((char*)cmd, (char*)"pid pos L ", sizeof("pid pos L ")-1)==0) {
    res = ParsePidParameter(&speedLeftConfig, cmd+sizeof("pid pos L ")-1, handled, io);
  } else if (UTIL1_strncmp((char*)cmd, (char*)"pid pos R ", sizeof("pid pos R ")-1)==0) {
    res = ParsePidParameter(&speedRightConfig, cmd+sizeof("pid pos R ")-1, handled, io);
  }
  return res;
}
#endif /* PL_HAS_SHELL */

void PID_Start(void) {
  /* reset the 'memory' values of the structure back to zero */
  speedLeftConfig.lastError = 0;
  speedLeftConfig.integral = 0;
  speedRightConfig.lastError = 0;
  speedRightConfig.integral = 0;
  posLeftConfig.lastError = 0;
  posLeftConfig.integral = 0;
  posRightConfig.lastError = 0;
  posRightConfig.integral = 0;
}

void PID_Deinit(void) {
  /* nothing needed */
}

void PID_Init(void) {
#if 0
  /*! \todo determine your PID values */
  speedLeftConfig.pFactor100 = 0;
  speedLeftConfig.iFactor100 = 0;
  speedLeftConfig.dFactor100 = 0;
  speedLeftConfig.iAntiWindup = 0;
  speedLeftConfig.lastError = 0;
  speedLeftConfig.integral = 0;

  speedRightConfig.pFactor100 = 0;
  speedRightConfig.iFactor100 = 0;
  speedRightConfig.dFactor100 = 0;
  speedRightConfig.iAntiWindup = 0;
  speedRightConfig.lastError = 0;
  speedRightConfig.integral = 0;
#else
  speedLeftConfig.pFactor100 = 2000;
  speedLeftConfig.iFactor100 = 0;
  speedLeftConfig.dFactor100 = 0;
  speedLeftConfig.iAntiWindup = 120000;
  speedLeftConfig.lastError = 0;
  speedLeftConfig.integral = 0;

  speedRightConfig.pFactor100 = speedLeftConfig.pFactor100;
  speedRightConfig.iFactor100 = speedLeftConfig.iFactor100;
  speedRightConfig.dFactor100 = speedLeftConfig.dFactor100;
  speedRightConfig.iAntiWindup = speedLeftConfig.iAntiWindup;
  speedRightConfig.lastError = 0;
  speedRightConfig.integral = 0;
#endif


}
#endif /* PL_HAS_PID */
