/**
 * \file
 * \brief Watchdog Module
 * \author Erich Styger
 *
 * Module reset the system with a watchdog timer in case of deadlock.
 */

#include "Platform.h"
#if PL_HAS_WATCHDOG
#include "Watchdog.h"
#include "FRTOS1.h"
#include "WDog1.h"
#include "LED.h"
#include "Shell.h"

static uint16_t state; /* state of watchdog state machine */
static uint16_t taskCntrs[WDT_TASK_ID_LAST]; /* counters of each task, will be incremented by each task */

#define WDT_TASK_TIME            20 /* wait time of watchdog task. Watchdog will be reset at this frequency */
#define WDT_CHECK_TASK_TIME_MS  800 /* After this period, the task time will be checked. Must be between WDT_TASK_MS_LOW_CNT and WDT_TASK_MS_HIGH_CNT */
#define WDT_TASK_MS_LOW_CNT     700 /* task counter must be above this value */
#define WDT_TASK_MS_HIGH_CNT   1000 /* task counter must be below this value */

/*!
 * \brief Each task calls this method with its period time
 * \param id Task identifier
 * \param msCntr Task period time
 */
void WDT_IncTaskCntr(WDT_TaskID id, uint16_t msCntr) {
  if (id < 0 || id>=WDT_TASK_ID_LAST) {
    SHELL_SendString("WDT: Wrong ID!\r\n");
    for(;;) {} /* illegal ID, let it block so the watchdog triggers */
  }
  taskCntrs[id] += msCntr;
}

static void ResetTaskCntr(void) {
  int i;
  
  for(i=0;i<WDT_TASK_ID_LAST;i++) {
    taskCntrs[i] = 0;
  }
}

static void CheckTaskCntr(void) {
  int i;
  
  for(i=0;i<WDT_TASK_ID_LAST;i++) {
    if (taskCntrs[i]<WDT_TASK_MS_LOW_CNT || taskCntrs[i]>WDT_TASK_MS_HIGH_CNT) { /* must be about one second! */
      uint8_t buf[48];
      UTIL1_strcpy(buf, sizeof(buf), "WDT: Watchdog! task:");
      UTIL1_strcatNum16u(buf, sizeof(buf), i);
      UTIL1_strcat(buf, sizeof(buf), " cnt: ");
      UTIL1_strcatNum16u(buf, sizeof(buf), taskCntrs[i]);
      UTIL1_strcat(buf, sizeof(buf), "\r\n");
      SHELL_SendString(buf);
      for(;;) {}
    }
  }
}

static void wdt_a(void) {
  if (state!=0x5555) {
    SHELL_SendString("WDT: not 0x5555!\r\n");
    for(;;) {} /* error! */
  }
  state += 0x1111;
}

static void wdt_b(bool check) {
  if (state!=0x8888) {
    SHELL_SendString("WDT: not 0x8888!\r\n");
    for(;;) {} /* error! */
  }
  if (check) {
    CheckTaskCntr();
  }
  (void)WDog1_Clear(WDog1_DeviceData);
  if (state!=0x8888) {
    SHELL_SendString("WDT: not 0x8888!\r\n");
    for(;;) {} /* error! */
  }
  state = 0;
  if (check) {
    ResetTaskCntr();
  }
}

static portTASK_FUNCTION(WatchdogTask, pvParameters) {
  uint16_t cnt = 0;
  
  (void)pvParameters; /* parameter not used */
  LED_GREEN_On(); /* Turn off watchdog LED */
  for(;;) {
    state = 0x5555;
    wdt_a();
    WDT_IncTaskCntr(WDT_TASK_ID_WDT, WDT_TASK_TIME);
    FRTOS1_vTaskDelay(WDT_TASK_TIME/portTICK_RATE_MS);
    cnt += WDT_TASK_TIME;
    state += 0x2222;
    if (cnt>WDT_CHECK_TASK_TIME_MS) {
      wdt_b(TRUE);
      cnt = 0;
    } else {
      wdt_b(FALSE);
    }
  } /* for */
}

void WDT_Deinit(void) {
  /* nothing needed */
}

void WDT_Init(void) {
  ResetTaskCntr();
  LED_GREEN_On(); /* turn red LED on. In case of watchdog reset, we should see the RED LED */
  if (FRTOS1_xTaskCreate(WatchdogTask, (signed portCHAR *)"WDog", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+1, NULL) != pdPASS) {
    for(;;){} /* error */
  }
}
#endif /* PL_HAS_WATCHDOG */


