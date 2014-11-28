/**
 * \file
 * \brief Real Time Operating System (RTOS) main program.
 * \author Erich Styger, erich.styger@hslu.ch
 */

#include "Platform.h"
#if PL_HAS_RTOS
#include "RTOS.h"
#include "FRTOS1.h"
#include "LED.h"
#include "Application.h"
#include "WAIT1.h"


#if 0
static portTASK_FUNCTION(T1, pvParameters) {
  for(;;) {

LED_BLUE_NegVal();
  }
}

static portTASK_FUNCTION(T2, pvParameters) {
  for(;;) {
	    LED_GREEN_NegVal();
  }
}
#endif
/*!TODO  Add two tasks Task1() and Task2() with tskIDLE PRIORITY. Let
them run with 'full speed' (without any suspend/vTaskDelay()). What
do you observe? How to x it? both have a runtime of 50%*/

void RTOS_Run(void) {
  FRTOS1_vTaskStartScheduler();
  /* does usually not return here */
}

void RTOS_Init(void) {

}

void RTOS_Deinit(void) {
}

#endif /* PL_HAS_RTOS */
