/**
 * \file
 * \brief Implementation of push button debouncing.
 * \author Erich Styger, erich.styger@hslu.ch
 *
 * This module implements the debouncing of keys, using a state machine.
 * While this is a rather complex way of debouncing, it allows us to
 * detect both short and long key presses, which is a plus.
 */

#include "Platform.h"
#if PL_HAS_DEBOUNCE
#include <stddef.h> /* for NULL */
#include "Debounce.h"
#include "Trigger.h"

/*! \brief Key scan routine which implements the state machine.
 \dot
 digraph example_api_graph {
 node [shape=box];
 DBNC_KEY_IDLE    [fillcolor=lightblue,style=filled,label="DBNC_KEY_IDLE" ];
 DBNC_KEY_PRESSED [fillcolor=lightblue,style=filled,label="DBNC_KEY_PRESSED" ];
 DBNC_KEY_RELEASE [fillcolor=lightblue,style=filled,label="DBNC_KEY_WAIT_RELEASE"];
 DBNC_KEY_IDLE -> DBNC_KEY_PRESSED -> DBNC_KEY_RELEASE ;
 DBNC_KEY_PRESSED -> DBNC_KEY_PRESSED ;
 DBNC_KEY_IDLE -> DBNC_KEY_IDLE ;
 }
 \enddot
 */

void DBNC_Process(DBNC_FSMData *data)
{
	DBNC_KeySet keys;

	for (;;)
	{ /* for loop, will return */
		switch (data->state)
		{
		case DBNC_KEY_IDLE: /* idle, and now getting a key */
			keys = data->getKeys();
			if (keys != 0)
			{
				data->state = DBNC_KEY_PRESSED; /* advance to next state */
				data->scanValue = data->getKeys();
				data->onDebounceEvent(DBNC_EVENT_PRESSED, data->scanValue);
				(void) TRG_SetTrigger(data->trigger, data->debounceTicks, (TRG_Callback) DBNC_Process, (void*) data);
			}
			return;

		case DBNC_KEY_PRESSED:
			keys = data->getKeys();
			if (keys == data->scanValue)
			{ /* still pressing the same keys */
				(void) TRG_SetTrigger(data->trigger, data->debounceTicks, (TRG_Callback) DBNC_Process, (void*) data); /* continue waiting */
				return;
			} else if (keys == 0)
			{ /* all keys are released */
				//if (data->longKeyCnt != 0) //only used if long pressed is enable

				data->onDebounceEvent(DBNC_EVENT_RELEASED, data->scanValue);
				data->scanValue = keys;

				data->state = DBNC_KEY_RELEASE;
				(void) TRG_SetTrigger(data->trigger, data->debounceTicks, (TRG_Callback) DBNC_Process, (void*) data);
			} else if (0 != ((data->scanValue ^ (keys)) & keys))
			{ /* we got another key pressed*/

				data->onDebounceEvent(DBNC_EVENT_PRESSED, (uint8_t) ((data->scanValue ^ (keys)) & keys)); //set pressed for new key
				data->scanValue = keys; //set new key state

				(void) TRG_SetTrigger(data->trigger, data->debounceTicks, (TRG_Callback) DBNC_Process, (void*) data);
			} else if (0 == ((data->scanValue ^ (keys)) & keys))
			{
				//one key is released

				data->onDebounceEvent(DBNC_EVENT_RELEASED, (uint8_t) ((data->scanValue ^ (keys)) & (~keys))); //set pressed for new key
				data->scanValue = keys; //set new key state
				(void) TRG_SetTrigger(data->trigger, data->debounceTicks, (TRG_Callback) DBNC_Process, (void*) data);
			}
			break;

		case DBNC_KEY_RELEASE: /* wait until keys are released */
			keys = data->getKeys();
			if (keys == 0)
			{ /* all keys released, go back to idle state. */
				//data->onDebounceEvent(DBNC_EVENT_RELEASED, data->scanValue);/* ------------------------- */
				data->scanValue = keys; //set new key state
				data->state = DBNC_KEY_IDLE; /* go back to idle */
				data->onDebounceEvent(DBNC_EVENT_END, data->scanValue); /* callback at the end of debouncing. *//* ------------------------- */
				return;
			} else
			{
				for(;;); //error ?
			}
			break;
		} /* switch */
	} /* for */
}

void DBNC_Deinit(void)
{
	/* nothing for now */
}

void DBNC_Init(void)
{
	/* nothing to init for now */
}
#endif /* PL_HAS_DEBOUNCE */
