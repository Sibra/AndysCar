/**
 * \file
 * \brief Timer driver
 * \author Erich Styger, erich.styger@hslu.ch
 *
 * This module implements the driver for all our timers.
  */

#include "Platform.h"
#if PL_HAS_TIMER
#include "Timer.h"
#if PL_HAS_LED
  #include "LED.h"
#endif
#if PL_HAS_EVENTS
  #include "Event.h"
#endif
#if PL_HAS_TRIGGER
  #include "Trigger.h"
#endif
#if PL_HAS_MOTOR_TACHO
#include "Tacho.h"
#endif
#if PL_HAS_PID
#include "Pid.h"
#endif

void TMR_OnInterrupt(void) {
  /* this one gets called from an interrupt!!!! */
	#if PL_HAS_TRIGGER
	  TRG_IncTick();
	#endif
	  TACHO_Sample();
}
void TMR_Init(void) {
  /* nothing needed right now */
}

void TMR_Deinit(void) {
  /* nothing needed right now */
}

#endif /*PL_HAS_TIMER*/
