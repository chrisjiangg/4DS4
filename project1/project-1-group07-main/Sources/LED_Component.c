#include "LED_Component.h"

QueueHandle_t led_queue;

void setupLEDComponent()
{
	BaseType_t status;
	setupLEDPins();
	setupLEDs();

	//printf("Setup LED");
    /*************** LED Task ***************/
	//Create LED Queue
	led_queue = xQueueCreate(1,10);
	if (led_queue == NULL)
	{
		PRINTF("led_queue creation failed!.\r\n");
		while (1);
	}

	//Create LED Task
	status = xTaskCreate(ledTask, "ledTask", 200, NULL, 4, NULL);
	if (status != pdPASS)
	{
		PRINTF("Task creation failed!.\r\n");
		while (1);
	}
}

void setupLEDPins()
{
	/* Port D Clock Gate Control: Clock enabled */
	CLOCK_EnableClock(kCLOCK_PortD);
	/* Port C Clock Gate Control: Clock enabled */
	CLOCK_EnableClock(kCLOCK_PortC);
	/* PORTD13 is configured as PTD13 */
	PORT_SetPinMux(PORTD, 1U, kPORT_MuxAlt4);
	/* PORTD13 is configured as PTD13 */
	PORT_SetPinMux(PORTC, 8U, kPORT_MuxAlt3);
	PORT_SetPinMux(PORTC, 9U, kPORT_MuxAlt3);
}

void setupLEDs()
{
	ftm_config_t ftmInfo, ftmInfo2, ftmInfo3;
	ftm_chnl_pwm_signal_param_t ftmParam, ftmParam2, ftmParam3;

	ftmParam.chnlNumber = kFTM_Chnl_1;
	ftmParam.level = kFTM_HighTrue;
	ftmParam.dutyCyclePercent = 0;
	ftmParam.firstEdgeDelayPercent = 0U;

	ftmParam.enableComplementary = false;
	ftmParam.enableDeadtime = false;

	FTM_GetDefaultConfig(&ftmInfo);

	FTM_Init(FTM3, &ftmInfo);
	FTM_SetupPwm(FTM3, &ftmParam, 1U, kFTM_EdgeAlignedPwm, 5000U, CLOCK_GetFreq(
	kCLOCK_BusClk));
	FTM_StartTimer(FTM3, kFTM_SystemClock);


	ftmParam2.chnlNumber = kFTM_Chnl_5;
	ftmParam2.level = kFTM_HighTrue;
	ftmParam2.dutyCyclePercent = 0;
	ftmParam2.firstEdgeDelayPercent = 0U;

	ftmParam2.enableComplementary = false;
	ftmParam2.enableDeadtime = false;

	FTM_GetDefaultConfig(&ftmInfo2);

	FTM_Init(FTM3, &ftmInfo2);
	FTM_SetupPwm(FTM3, &ftmParam2, 8U, kFTM_EdgeAlignedPwm, 5000U, CLOCK_GetFreq(
	kCLOCK_BusClk));
	FTM_StartTimer(FTM3, kFTM_SystemClock);


	ftmParam3.chnlNumber = kFTM_Chnl_4;
	ftmParam3.level = kFTM_HighTrue;
	ftmParam3.dutyCyclePercent = 0;
	ftmParam3.firstEdgeDelayPercent = 0U;

	ftmParam3.enableComplementary = false;
	ftmParam3.enableDeadtime = false;

	FTM_GetDefaultConfig(&ftmInfo3);

	FTM_Init(FTM3, &ftmInfo3);
	FTM_SetupPwm(FTM3, &ftmParam3, 9U, kFTM_EdgeAlignedPwm, 5000U, CLOCK_GetFreq(kCLOCK_BusClk));
	FTM_StartTimer(FTM3, kFTM_SystemClock);
}

void ledTask(void* pvParameters)
{
	//printf("\nLED Task\n");
	BaseType_t status;
	int received_int = 0;

	while (1)
	{
		status = xQueueReceive(led_queue, (void*) &received_int, portMAX_DELAY);

		if (status != pdPASS)
		{
			PRINTF("Queue Receive failed!.\r\n");
		}
		// Print the received string
		//PRINTF("\nReceived LED = %d", received_int);

		int red = 0;
		int blue = 0;
		int green = 0;

		if(received_int == 1) {
			red = 0;
			green = 100;
			blue = 0;
		}else if(received_int == 2) {
			red = 100;
			green = 100;
			blue = 0;
		}else if(received_int == 3) {
			red = 100;
			green = 0;
			blue = 0;
		}

		//printf("Red %d, Green %d, Blue %d", red, green, blue);

		FTM_UpdatePwmDutycycle(FTM3, kFTM_Chnl_1, kFTM_EdgeAlignedPwm, red);
		FTM_SetSoftwareTrigger(FTM3, true);

		FTM_UpdatePwmDutycycle(FTM3, kFTM_Chnl_5, kFTM_EdgeAlignedPwm, green);
		FTM_SetSoftwareTrigger(FTM3, true);

		FTM_UpdatePwmDutycycle(FTM3, kFTM_Chnl_4, kFTM_EdgeAlignedPwm, blue);
		FTM_SetSoftwareTrigger(FTM3, true);
	}
}
