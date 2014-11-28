/*
 * Buzzer.c
 *
 *  Created on: Aug 29, 2013
 *      Author: Erich Styger
 */

#include "Platform.h"
#if PL_HAS_BUZZER
#include "Buzzer.h"
#include "BUZ1.h"
#include "Trigger.h"

typedef struct {
  uint16_t buzPeriodTicks; /* number of trigger ticks for a PWM period */
  uint16_t buzIterationCntr; /*  */
} BUZ_TrgInfo;

static volatile BUZ_TrgInfo trgInfo;

static void BUZ_Toggle(void *dataPtr) {
  BUZ_TrgInfo *trgInfo = (BUZ_TrgInfo *)dataPtr;
  
  if (trgInfo->buzIterationCntr==0) {
    BUZ1_ClrVal(); /* turn buzzer off */
  } else {
    trgInfo->buzIterationCntr--;
    BUZ1_NegVal();
    (void)TRG_SetTrigger(TRG_BUZ_BEEP, trgInfo->buzPeriodTicks, BUZ_Toggle, trgInfo);
  }
}

uint8_t BUZ_Beep(uint16_t freq, uint16_t durationMs) {
  if (trgInfo.buzIterationCntr==0) { /* only if buzzer is not running right now */
    BUZ1_SetVal(); /* turn buzzer on */
    trgInfo.buzPeriodTicks = (1000*TRG_TICKS_MS)/freq;
    trgInfo.buzIterationCntr = durationMs/TRG_TICKS_MS/trgInfo.buzPeriodTicks;
    return TRG_SetTrigger(TRG_BUZ_BEEP, trgInfo.buzPeriodTicks, BUZ_Toggle, (void*)&trgInfo);
  } else {
    return ERR_BUSY;
  }
}

void BUZ_Deinit(void) {
  /* nothing to do */
}

void BUZ_Init(void) {
  BUZ1_SetVal(); /* turn buzzer on */
  trgInfo.buzPeriodTicks = 0;
  trgInfo.buzIterationCntr = 0;
}

#endif /* PL_HAS_BUZZER */
