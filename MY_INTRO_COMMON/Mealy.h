/*
 * Mealy.h
 *
 *  Created on: 21.10.2014
 *      Author: Laptop
 */

#ifndef MEALY_H_
#define MEALY_H_

/*! \brief This implements our Mealy Sequential Machine. On each call the FSM will perform a check/test advances as necessary. */
void MEALY_Step(void);

/*! \brief Initialization routine of the state machine */
void MEALY_Init(void);

/*! \brief De-iinitialization routine of the state machine */
void MEALY_Deinit(void);



#endif /* MEALY_H_ */
