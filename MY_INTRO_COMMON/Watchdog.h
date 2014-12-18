/**
 * \file
 * \brief Watchdog Module Interface
 * \author Erich Styger
 *
 * Module reset the system with a watchdog timer in case of deadlock.
 */

#ifndef WATCHDOG_H_
#define WATCHDOG_H_

#include "Platform.h"

/*! task ID's to monitor */
typedef enum {
#if PL_HAS_SHELL
  WDT_TASK_ID_SHELL,
#endif
#if PL_HAS_TRACE
  WDT_TASK_ID_TRACE,
#endif
#if PL_HAS_REMOTE && PL_APP_ACCEL_CONTROL_SENDER
  WDT_TASK_ID_REMOTE,
#endif
#if PL_HAS_DRIVE
  WDT_TASK_ID_DRIVE,
#endif
#if 0
#if PL_HAS_REFLECTANCE
  WDT_TASK_ID_REFLECTANCE,
#endif
#endif
  WDT_TASK_ID_STADIUM,
  WDT_TASK_ID_MAIN,
  WDT_TASK_ID_WDT,
  WDT_TASK_ID_LAST /* must be last! */
} WDT_TaskID;

/*!
 * \brief Each task calls this method with its period time
 * \param id Task identifier
 * \param msCntr Task period time
 */
void WDT_IncTaskCntr(WDT_TaskID id, uint16_t msCntr);

/*! \brief De-initialization of the module */
void WDT_Deinit(void);

/*! \brief Initialization of the module */
void WDT_Init(void);


#endif /* WATCHDOG_H_ */
