/**
 * \file
 * \brief Semaphore usage
 * \author Erich Styger, erich.styger@hslu.ch
 *
 * Module demonstrating semaphores.
 */

#include "Platform.h" /* interface to the platform */
#if PL_HAS_SEMAPHORE
#include "FRTOS1.h"
#include "Sem.h"
#include "LED.h"
#if PL_HAS_SHELL
#include "Shell.h"
#endif
#if PL_HAS_RTOS_TRACE
#include "RTOSTRC1.h"
#endif

#if 0
static portTASK_FUNCTION(vSlaveTask, pvParameters)
{
	for (;;)
	{
		xSemaphoreHandle sem;

		sem = (xSemaphoreHandle) pvParameters;
		for (;;)
		{
			if (sem != NULL)
			{
				if (FRTOS1_xSemaphoreTake(sem, portMAX_DELAY) == pdTRUE)
				{
					LED_BLUE_NegVal();
					//CLS1_SendStr("got semaphore!\r\n", CLS1_GetStdio()->stdOut);
				}
			}
		}
	}
}

static portTASK_FUNCTION(vMasterTask, pvParameters)
{
	xSemaphoreHandle sem = NULL;
	static int i = 0;
	(void) pvParameters; /* parameter not used */
	FRTOS1_vSemaphoreCreateBinary(sem);
	if (sem == NULL)
	{ /* semaphore creation failed */
		for (;;)
		{
		} /* error */
	}
	FRTOS1_vQueueAddToRegistry(sem, "Sem"); //!*TODO what?
#if PL_HAS_RTOS_TRACE
			RTOSTRC1_vTraceSetQueueName(sem, "IPC_Sem");
#endif
	/* create slave task */
	if (FRTOS1_xTaskCreate(vSlaveTask, "Slave", configMINIMAL_STACK_SIZE, sem, tskIDLE_PRIORITY+1, NULL) != pdPASS)
	{
		for (;;)
		{
		} /* error */
	}
	for (;;)
	{
		if (sem != NULL)
		{ /* valid semaphore? */

			(void) xSemaphoreGive(sem); /* give control to other task */
			//CLS1_SendStr("gave semaphore!\r\n", CLS1_GetStdio()->stdOut);
			FRTOS1_vTaskDelay(1000/portTICK_RATE_MS);

		}
	}

}
#endif /* USE_SEMAPHORES */
#endif
void SEM_Deinit(void)
{
	/* nothing */
}

/*! \brief Initializes module */
void SEM_Init(void)
{
#if 0
	if (FRTOS1_xTaskCreate(vMasterTask, "Master", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+1, NULL) != pdPASS)
	{
		for (;;)
		{
		} /* error */
	}
#endif
}
