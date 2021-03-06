/*
 * Keys.c
 *
 *  Created on: 16.10.2014
 *      Author: Laptop
 */

#include "Platform.h"
#if PL_HAS_KEYS
  #include "Keys.h"
#if PL_HAS_EVENTS
  #include "Event.h"
#endif
#if PL_HAS_DEBOUNCE
  #include "KeyDebounce.h"
#endif

void KEY_Scan(void) {
#if PL_HAS_DEBOUNCE
  KEYDBNC_Process();
#else
#if PL_NOF_KEYS >= 1 && PL_KEY_POLLED_KEY1
  if (KEY1_Get()) { /* key pressed */
    EVNT_SetEvent(EVNT_SW1_PRESSED);
  }
#endif
#if PL_NOF_KEYS >= 2 && PL_KEY_POLLED_KEY2
  if (KEY2_Get()) { /* key pressed */
    EVNT_SetEvent(EVNT_SW2_PRESSED);
  }
#endif
#if PL_NOF_KEYS >= 3 && PL_KEY_POLLED_KEY3
  if (KEY3_Get()) { /* key pressed */
    EVNT_SetEvent(EVNT_SW3_PRESSED);
  }
#endif
#if PL_NOF_KEYS >= 4 && PL_KEY_POLLED_KEY4
  if (KEY4_Get()) { /* key pressed */
    EVNT_SetEvent(EVNT_SW4_PRESSED);
  }
#endif
#if PL_NOF_KEYS >= 5 && PL_KEY_POLLED_KEY5
  if (KEY5_Get()) { /* key pressed */
    EVNT_SetEvent(EVNT_SW5_PRESSED);
  }
#endif
#if PL_NOF_KEYS >= 6 && PL_KEY_POLLED_KEY6
  if (KEY6_Get()) { /* key pressed */
    EVNT_SetEvent(EVNT_SW6_PRESSED);
  }
#endif
#if PL_NOF_KEYS >= 7 && PL_KEY_POLLED_KEY7
  if (KEY7_Get()) { /* key pressed */
    EVNT_SetEvent(EVNT_SW7_PRESSED);
  }
#endif
#endif /* PL_HAS_DEBOUNCE */
}

#if PL_HAS_KBI
void KEY_OnInterrupt(KEY_Buttons button) {
#if PL_HAS_DEBOUNCE
  KEYDBNC_Process();
#else
  switch(button) {
#if PL_NOF_KEYS >= 1
    case KEY_BTN1: EVNT_SetEvent(EVNT_SW1_PRESSED); break;
#endif
#if PL_NOF_KEYS >= 2
    case KEY_BTN2: EVNT_SetEvent(EVNT_SW2_PRESSED); break;
#endif
#if PL_NOF_KEYS >= 3
    case KEY_BTN3: EVNT_SetEvent(EVNT_SW3_PRESSED); break;
#endif
#if PL_NOF_KEYS >= 4
    case KEY_BTN4: EVNT_SetEvent(EVNT_SW4_PRESSED); break;
#endif
#if PL_NOF_KEYS >= 5
    case KEY_BTN5: EVNT_SetEvent(EVNT_SW5_PRESSED); break;
#endif
#if PL_NOF_KEYS >= 6
    case KEY_BTN6: EVNT_SetEvent(EVNT_SW6_PRESSED); break;
#endif
#if PL_NOF_KEYS >= 7
    case KEY_BTN7: EVNT_SetEvent(EVNT_SW7_PRESSED); break;
#endif
    default:
      /* unknown? */
      break;
  } /* switch */
#endif
}

void KEY_EnableInterrupts(void) {
#if PL_NOF_KEYS >= 1 && !PL_KEY_POLLED_KEY1
  SW1_Enable();
#endif
#if PL_NOF_KEYS >= 2 && !PL_KEY_POLLED_KEY2
  SW2_Enable();
#endif
#if PL_NOF_KEYS >= 3 && !PL_KEY_POLLED_KEY3
  SW3_Enable();
#endif
#if PL_NOF_KEYS >= 4 && !PL_KEY_POLLED_KEY4
  SW4_Enable();
#endif
#if PL_NOF_KEYS >= 5 && !PL_KEY_POLLED_KEY5
  SW5_Enable();
#endif
#if PL_NOF_KEYS >= 6 && !PL_KEY_POLLED_KEY6
  SW6_Enable();
#endif
#if PL_NOF_KEYS >= 7 && !PL_KEY_POLLED_KEY7
  SW7_Enable();
#endif
}

void KEY_DisableInterrupts(void) {
#if PL_NOF_KEYS >= 1 && !PL_KEY_POLLED_KEY1
  SW1_Disable();
#endif
#if PL_NOF_KEYS >= 2 && !PL_KEY_POLLED_KEY2
  SW2_Disable();
#endif
#if PL_NOF_KEYS >= 3 && !PL_KEY_POLLED_KEY3
  SW3_Disable();
#endif
#if PL_NOF_KEYS >= 4 && !PL_KEY_POLLED_KEY4
  SW4_Disable();
#endif
#if PL_NOF_KEYS >= 5 && !PL_KEY_POLLED_KEY5
  SW5_Disable();
#endif
#if PL_NOF_KEYS >= 6 && !PL_KEY_POLLED_KEY6
  SW6_Disable();
#endif
#if PL_NOF_KEYS >= 7 && !PL_KEY_POLLED_KEY7
  SW7_Disable();
#endif
}


#endif

/*! \brief Key driver initialization */
void KEY_Init(void) {
  /* nothing needed for now */
}

/*! \brief Key driver de-initialization */
void KEY_Deinit(void) {
  /* nothing needed for now */
}
#endif /* PL_HAS_KEYS */

void PORTA_OnInterrupt(void) {
  void Cpu_ivINT_PORTA(void); /* prototype of ISR in Cpu.c */

  Cpu_ivINT_PORTA(); /* call interrupt handler created by the ExtInt components */
}
