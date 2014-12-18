/*
 * Plattform.h
 *
 *  Created on: 09.10.2014
 *      Author: Laptop
 */

#ifndef PLATTFORM_H_
#define PLATTFORM_H_

#include "PE_Types.h" /* for common Processor Expert types used throughout the project, e.g. 'bool' */
#include "PE_Error.h" /* global error constants */
#include <stddef.h>   /* for NULL */

/* List of supported platforms. The PL_BOARD_IS_xxx is defined in the compiler command line settings.  */
#define PL_IS_FRDM   (defined(PL_BOARD_IS_FRDM))
#define PL_IS_ROBO  (defined(PL_BOARD_IS_ROBO))


#define PL_HAS_STADIUM			(1 && PL_IS_ROBO)
// Freedom and Robo
#define PL_HAS_LED 				(1)
#define PL_HAS_EVENTS			(1)
#define PL_HAS_TIMER			(1)
#define PL_HAS_KEYS				(1)
#define PL_HAS_JOYSTICK       	(1 && PL_IS_FRDM && PL_HAS_KEYS)
#define PL_HAS_KBI            	(1)
#define PL_HAS_MEALY			(0)
#define PL_HAS_TRIGGER			(1)
#define PL_HAS_HEARTBEAT		(0)
#define PL_HAS_SHELL			(1)
#define PL_HAS_BUZZER			(1 && PL_IS_ROBO)
#define PL_HAS_DEBOUNCE			(1 && PL_HAS_KEYS)
#define PL_HAS_RTOS				(1)
#define PL_HAS_USB_CDC       	(0 && PL_IS_ROBO)
#define PL_HAS_BLUETOOTH      	(1 && PL_IS_ROBO)
#define PL_HAS_SHELL_QUEUE    	(1 && PL_HAS_SHELL)
#define PL_HAS_SEMAPHORE		(1)
#define PL_HAS_LINE_SENSOR		(1 && PL_IS_ROBO)
#define PL_HAS_REFLECTANCE		(1 && PL_IS_ROBO)
#define PL_HAS_MOTOR			(1 && PL_IS_ROBO)
#define PL_HAS_CONFIG_NVM		(1 && PL_IS_ROBO)
#define PL_HAS_QUAD_CALIBRATION (0 && PL_IS_ROBO) //disabled
#define PL_HAS_MCP4728			(0 && PL_IS_ROBO) //disabled
#define PL_HAS_MOTOR_TACHO		(1 && PL_IS_ROBO)
#define PL_HAS_MOTOR_QUAD		(1 && PL_IS_ROBO)
#define PL_HAS_ULTRASONIC		(1 && PL_IS_ROBO)
#define PL_HAS_RADIO          	(0)
#define PL_HAS_QUADRATURE		(1 && PL_IS_ROBO)
#define PL_HAS_DRIVE			(1 && PL_IS_ROBO)
#define PL_HAS_PID				(1 && PL_IS_ROBO)
#define PL_HAS_ACCEL			(0 && PL_IS_FRDM)
#define PL_HAS_WATCHDOG			(0 && PL_IS_ROBO)
// Freedomboard
#if (1 && PL_IS_FRDM)

	#define PL_NOF_LEDS       	(1)
	#define PL_NOF_KEYS			(7)

	#define PL_KEY_POLLED_KEY1  (0)
	#define PL_KEY_POLLED_KEY2  (0)
	#define PL_KEY_POLLED_KEY3  (1)
	#define PL_KEY_POLLED_KEY4  (0)
	#define PL_KEY_POLLED_KEY5  (1)
	#define PL_KEY_POLLED_KEY6  (1)
	#define PL_KEY_POLLED_KEY7  (0)
#endif
// Roboter
	#define PL_NOF_LEDS       	(1)
	#define PL_NOF_KEYS			(1)
	#define PL_KEY_POLLED_KEY1  (1)
//  #error "unknown configuration?"

void PL_Init(void);
void PL_Deinit(void);

#endif /* PLATTFORM_H_ */
