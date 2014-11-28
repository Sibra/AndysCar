/*
 * stadium.h
 *
 *  Created on: 18.11.2014
 *      Author: Laptop
 */

#ifndef STADIUM_H_
#define STADIUM_H_

#include "Platform.h"
#if PL_HAS_STADIUM
#include "CLS1.h"

uint8_t Stadium_ParseCommand(const unsigned char *cmd, bool *handled, const CLS1_StdIOType *io);
void fightMode(void);
void Stadium_INIT(void);
void usMode();
#endif
#endif /* STADIUM_H_ */
