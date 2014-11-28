/*
 * LED.h
 *
 *  Created on: 10.10.2014
 *      Author: Laptop
 */

#ifndef LED_H_
#define LED_H_

#include "Platform.h"

#if PL_NOF_LEDS >= 1
 #include "LED_BLUE.h"
  #define LED_BLUE_On()     LED_BLUE_ClrVal();
    /*!< Turn LED 1 on */
  #define LED_BLUE_Off()    LED_BLUE_SetVal();
    /*!< Turn LED 1 off */
  #define LED_BLUE_Neg()    LED_BLUE_NegVal()
    /*!< Toggle LED 1 */
  #define LED_BLUE_Get()    LED_BLUE_GetVal()
    /*!< Return TRUE if LED is on, FALSE otherwise */
  #define LED_BLUE_Put(val) LED_BLUE_PutVal(val)
    /*!< Turn LED on or off */
  #define LED_BLUE_Init()   /* do nothing */
    /*!< Initialize LED */
  #define LED_BLUE_Deinit() /* do nothing */
    /*!< De-Initialize LED */
#else
  #define LED_BLUE_On()     /* do nothing */
    /*!< Turn LED 1 on */
  #define LED_BLUE_Off()    /* do nothing */
    /*!< Turn LED 1 off */
  #define LED_BLUE_Neg()    /* do nothing */
    /*!< Toggle LED 1 */
  #define LED_BLUE_Get()  0 /* do nothing */
    /*!< Return TRUE if LED is on, FALSE otherwise */
  #define LED_BLUE_Put(val) /* do nothing */
    /*!< Turn LED 1 on or off */
  #define LED_BLUE_Init()   /* do nothing */
    /*!< Initialize LED 1 */
  #define LED_BLUE_Deinit() /* do nothing */
    /*!< De-Initialize LED 1 */
#endif

#if PL_NOF_LEDS >= 2
  #include "LED_GREEN.h"
  #define LED_GREEN_On()     LED_GREEN_ClrVal();
    /*!< Turn LED 1 on */
  #define LED_GREEN_Off()    LED_GREEN_SetVal();
    /*!< Turn LED 1 off */
  #define LED_GREEN_Neg()    LED_GREEN_NegVal()
    /*!< Toggle LED 1 */
  #define LED_GREEN_Get()    LED_GREEN_GetVal()
    /*!< Return TRUE if LED is on, FALSE otherwise */
  #define LED_GREEN_Put(val) LED_GREEN_PutVal(val)
    /*!< Turn LED on or off */
  #define LED_GREEN_Init()   /* do nothing */
    /*!< Initialize LED */
  #define LED_GREEN_Deinit() /* do nothing */
    /*!< De-Initialize LED */
#else
  #define LED_GREEN_On()     /* do nothing */
    /*!< Turn LED 2 on */
  #define LED_GREEN_Off()    /* do nothing */
    /*!< Turn LED 2 off */
  #define LED_GREEN_Neg()    /* do nothing */
    /*!< Toggle LED 2 */
  #define LED_GREEN_Get()  0 /* do nothing */
    /*!< Return TRUE if LED is on, FALSE otherwise */
  #define LED_GREEN_Put(val) /* do nothing */
    /*!< Turn LED 2 on or off */
  #define LED_GREEN_Init()   /* do nothing */
    /*!< Initialize LED 2 */
  #define LED_GREEN_Deinit() /* do nothing */
    /*!< De-Initialize LED 2 */
#endif

#if PL_NOF_LEDS >= 3
  #include "LED_RED.h"
  #define LED_RED_On()     LED_RED_ClrVal();
    /*!< Turn LED 1 on */
  #define LED_RED_Off()    LED_RED_SetVal();
    /*!< Turn LED 1 off */
  #define LED_RED_Neg()    LED_RED_NegVal()
    /*!< Toggle LED 1 */
  #define LED_RED_Get()    LED_RED_GetVal()
    /*!< Return TRUE if LED is on, FALSE otherwise */
  #define LED_RED_Put(val) LED_RED_PutVal(val)
    /*!< Turn LED on or off */
  #define LED_RED_Init()   /* do nothing */
    /*!< Initialize LED */
  #define LED_RED_Deinit() /* do nothing */
    /*!< De-Initialize LED */
#else
  #define LED_RED_On()     /* do nothing */
    /*!< Turn LED 3 on */
  #define LED_RED_Off()    /* do nothing */
    /*!< Turn LED 3 off */
  #define LED_RED_Neg()    /* do nothing */
    /*!< Toggle LED 3 */
  #define LED_RED_Get()  0 /* do nothing */
    /*!< Return TRUE if LED is on, FALSE otherwise */
  #define LED_RED_Put(val) /* do nothing */
    /*!< Turn LED 3 on or off */
  #define LED_RED_Init()   /* do nothing */
    /*!< Initialize LED 3 */
  #define LED_RED_Deinit() /* do nothing */
    /*!< De-Initialize LED 3 */
#endif

/*!
 * \brief LED test routine.
 * This routine tests if:
 * - we can turn the LEDs properly on and off
 * - if we can negate them
 * - if we can set an LED value
 * - if we can get the LED value
 * If the test fails, the program will hanging in an endless loop
 */
void LED_Test(void);

/*!
 * \brief LED Driver Intialization.
 */
void LED_Init(void);

/*!
 * \brief LED Driver De-Initialization.
 */
void LED_Deinit(void);


#endif /* LED_H_ */
