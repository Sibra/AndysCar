/*
 * LED.c
 *
 *  Created on: 10.10.2014
 *      Author: Laptop
 */



#include "Platform.h"
#include "LED.h"



void LED_Deinit(void) {
  /* all LED's off by default */
  #if PL_NOF_LEDS>=1
  LED_BLUE_Off();
  #endif
  #if PL_NOF_LEDS>=2
  LED_GREEN_Off();
  #endif
  #if PL_NOF_LEDS>=3
  LED_RED_Off();
  #endif
  LED_BLUE_Deinit();
  LED_GREEN_Deinit();
  LED_RED_Deinit();
}

void LED_Init(void) {
  LED_BLUE_Init();
  LED_GREEN_Init();
  LED_RED_Init();
  /* all LED's off by default */
  #if PL_NOF_LEDS>=1
  LED_BLUE_Off();
  #endif
  #if PL_NOF_LEDS>=2
  LED_GREEN_Off();
  #endif
  #if PL_NOF_LEDS>=3
  LED_RED_Off();
  #endif
}
