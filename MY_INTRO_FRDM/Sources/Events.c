/* ###################################################################
**     Filename    : Events.c
**     Project     : MY_INTRO_FRDM
**     Processor   : MKL25Z128VLK4
**     Component   : Events
**     Version     : Driver 01.00
**     Compiler    : GNU C Compiler
**     Date/Time   : 2014-10-30, 14:30, # CodeGen: 57
**     Abstract    :
**         This is user's event module.
**         Put your event handler code here.
**     Settings    :
**     Contents    :
**         FRTOS1_vApplicationStackOverflowHook - void FRTOS1_vApplicationStackOverflowHook(xTaskHandle pxTask, char *pcTaskName);
**         FRTOS1_vApplicationTickHook          - void FRTOS1_vApplicationTickHook(void);
**         FRTOS1_vApplicationIdleHook          - void FRTOS1_vApplicationIdleHook(void);
**         FRTOS1_vApplicationMallocFailedHook  - void FRTOS1_vApplicationMallocFailedHook(void);
**         SW7_OnInterrupt                      - void SW7_OnInterrupt(void);
**         SW4_OnInterrupt                      - void SW4_OnInterrupt(void);
**         SW3_OnInterrupt                      - void SW3_OnInterrupt(void);
**         SW2_OnInterrupt                      - void SW2_OnInterrupt(void);
**         SW1_OnInterrupt                      - void SW1_OnInterrupt(void);
**         Heartbeat_OnInterrupt                - void Heartbeat_OnInterrupt(void);
**         Cpu_OnNMIINT                         - void Cpu_OnNMIINT(void);
**
** ###################################################################*/
/*!
** @file Events.c
** @version 01.00
** @brief
**         This is user's event module.
**         Put your event handler code here.
*/         
/*!
**  @addtogroup Events_module Events module documentation
**  @{
*/         
/* MODULE Events */

#include "Cpu.h"
#include "Events.h"

#ifdef __cplusplus
extern "C" {
#endif 


/* User includes (#include below this line is not maintained by Processor Expert) */
#include "Platform.h"
#if PL_HAS_TIMER
  #include "Timer.h"
#endif
#if PL_HAS_EVENTS
  #include "Event.h"
#endif
#if PL_HAS_KEYS
  #include "Keys.h"
#endif
#if PL_HAS_TRIGGER
  #include "Trigger.h"
#endif
/*
** ===================================================================
**     Event       :  FRTOS1_vApplicationStackOverflowHook (module Events)
**
**     Component   :  FRTOS1 [FreeRTOS]
**     Description :
**         if enabled, this hook will be called in case of a stack
**         overflow.
**     Parameters  :
**         NAME            - DESCRIPTION
**         pxTask          - Task handle
**       * pcTaskName      - Pointer to task name
**     Returns     : Nothing
** ===================================================================
*/
void FRTOS1_vApplicationStackOverflowHook(xTaskHandle pxTask, char *pcTaskName)
{
  /* This will get called if a stack overflow is detected during the context
     switch.  Set configCHECK_FOR_STACK_OVERFLOWS to 2 to also check for stack
     problems within nested interrupts, but only do this for debug purposes as
     it will increase the context switch time. */
  (void)pxTask;
  (void)pcTaskName;
  taskDISABLE_INTERRUPTS();
  /* Write your code here ... */
  for(;;) {}
}

/*
** ===================================================================
**     Event       :  FRTOS1_vApplicationTickHook (module Events)
**
**     Component   :  FRTOS1 [FreeRTOS]
**     Description :
**         If enabled, this hook will be called by the RTOS for every
**         tick increment.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void FRTOS1_vApplicationTickHook(void)
{
  /* Called for every RTOS tick. */
#if PL_HAS_TRIGGER
	TRG_IncTick();
#endif
}

/*
** ===================================================================
**     Event       :  FRTOS1_vApplicationIdleHook (module Events)
**
**     Component   :  FRTOS1 [FreeRTOS]
**     Description :
**         If enabled, this hook will be called when the RTOS is idle.
**         This might be a good place to go into low power mode.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void FRTOS1_vApplicationIdleHook(void)
{
  /* Called whenever the RTOS is idle (from the IDLE task).
     Here would be a good place to put the CPU into low power mode. */
  /* Write your code here ... */
}

/*
** ===================================================================
**     Event       :  FRTOS1_vApplicationMallocFailedHook (module Events)
**
**     Component   :  FRTOS1 [FreeRTOS]
**     Description :
**         If enabled, the RTOS will call this hook in case memory
**         allocation failed.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void FRTOS1_vApplicationMallocFailedHook(void)
{
  /* Called if a call to pvPortMalloc() fails because there is insufficient
     free memory available in the FreeRTOS heap.  pvPortMalloc() is called
     internally by FreeRTOS API functions that create tasks, queues, software
     timers, and semaphores.  The size of the FreeRTOS heap is set by the
     configTOTAL_HEAP_SIZE configuration constant in FreeRTOSConfig.h. */
  taskDISABLE_INTERRUPTS();
  /* Write your code here ... */
  for(;;) {}
}

/*
** ===================================================================
**     Event       :  SW7_OnInterrupt (module Events)
**
**     Component   :  SW7 [ExtInt]
**     Description :
**         This event is called when an active signal edge/level has
**         occurred.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void SW7_OnInterrupt(void)
{
	  if (KEY7_Get()) {
	    KEY_OnInterrupt(KEY_BTN7);
	  }
}

/*
** ===================================================================
**     Event       :  SW4_OnInterrupt (module Events)
**
**     Component   :  SW4 [ExtInt]
**     Description :
**         This event is called when an active signal edge/level has
**         occurred.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void SW4_OnInterrupt(void)
{
#if PL_HAS_KBI
  if (KEY4_Get()) {
    KEY_OnInterrupt(KEY_BTN4);
  }
#endif
}

/*
** ===================================================================
**     Event       :  SW3_OnInterrupt (module Events)
**
**     Component   :  SW3 [ExtInt]
**     Description :
**         This event is called when an active signal edge/level has
**         occurred.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void SW3_OnInterrupt(void)
{
#if 0
#if PL_HAS_KBI
#if 1 /* Problem with Processor Expert and sharing PTA4/NMI interrupt: code below is missing in ExtIntLdd3_OnInterrupt() */
  /* Check the pin interrupt flag of the shared interrupt */
  if (PORT_PDD_GetPinInterruptFlag(PORTA_BASE_PTR, ExtIntLdd3_PIN_INDEX)) {
    /* Clear the interrupt flag */
    PORT_PDD_ClearPinInterruptFlag(PORTA_BASE_PTR, ExtIntLdd3_PIN_INDEX);
    /* call user event */
    KEY_OnInterrupt(KEY_BTN3);
  }
#else
  if (KEY3_Get()) {
    KEY_OnInterrupt(KEY_BTN3);
  }
#endif
#endif
#endif
}

/*
** ===================================================================
**     Event       :  SW2_OnInterrupt (module Events)
**
**     Component   :  SW2 [ExtInt]
**     Description :
**         This event is called when an active signal edge/level has
**         occurred.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void SW2_OnInterrupt(void)
{
#if PL_HAS_KBI
  if (KEY2_Get()) {
    KEY_OnInterrupt(KEY_BTN2);
  }
#endif
}

/*
** ===================================================================
**     Event       :  SW1_OnInterrupt (module Events)
**
**     Component   :  SW1 [ExtInt]
**     Description :
**         This event is called when an active signal edge/level has
**         occurred.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void SW1_OnInterrupt(void)
{
#if PL_HAS_KBI
  if (KEY1_Get()) {
    KEY_OnInterrupt(KEY_BTN1);
  }
#endif
}

/*
** ===================================================================
**     Event       :  Cpu_OnNMIINT (module Events)
**
**     Component   :  Cpu [MKL25Z128LK4]
*/
/*!
**     @brief
**         This event is called when the Non maskable interrupt had
**         occurred. This event is automatically enabled when the [NMI
**         interrupt] property is set to 'Enabled'.
*/
/* ===================================================================*/
void Cpu_OnNMIINT(void)
{
  /* Write your code here ... */
}

/*
** ===================================================================
**     Event       :  GI2C1_OnError (module Events)
**
**     Component   :  GI2C1 [GenericI2C]
**     Description :
**         Event called in case of error condition
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void GI2C1_OnError(void)
{
  /* Write your code here ... */
}

/* END Events */

#ifdef __cplusplus
}  /* extern "C" */
#endif 

/*!
** @}
*/
/*
** ###################################################################
**
**     This file was created by Processor Expert 10.4 [05.10]
**     for the Freescale Kinetis series of microcontrollers.
**
** ###################################################################
*/
