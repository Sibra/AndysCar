/*
 * Application.c
 *
 *  Created on: 10.10.2014
 *      Author: Laptop
 */

#ifndef APPLICATION_H_
#define APPLICATION_H_

#include "Stadium.h"

void APP_Start(void);
bool getState();
void APP_DebugPrint(unsigned char *str);

#define init_function ({    	LED_BLUE_On();WAIT1_Waitms(100);LED_BLUE_Off();WAIT1_Waitms(400);LED_GREEN_On();WAIT1_Waitms(100);LED_GREEN_Off();WAIT1_Waitms(400);LED_BLUE_On();LED_GREEN_On();WAIT1_Waitms(100);LED_BLUE_Off();LED_GREEN_Off();})
#endif /* APPLICATION_H_ */
