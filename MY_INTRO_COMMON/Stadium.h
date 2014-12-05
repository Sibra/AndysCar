/*
 * stadium.h
 *
 *  Created on: 18.11.2014
 *      Author: Laptop
 */

#ifndef STADIUM_H_
#define STADIUM_H_

#include "Platform.h"
typedef enum {
	STOP, FIGHT, NEED_CALIB, FIND, PID, REMOTE_CONTROL,NONE
} StadiumMode;
#if PL_HAS_STADIUM
#include "CLS1.h"

void remoteControl(uint8_t speedType);
uint8_t Stadium_ParseCommand(const unsigned char *cmd, bool *handled, const CLS1_StdIOType *io);
void fightMode(void);
void Stadium_INIT(void);
void setState(StadiumMode state);
void usMode();
#endif
#endif /* STADIUM_H_ */
