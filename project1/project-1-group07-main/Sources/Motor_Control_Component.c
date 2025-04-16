#include "Motor_Control_Component.h"

QueueHandle_t motor_queue;
QueueHandle_t angle_queue;

void setupMotorComponent()
{
	BaseType_t status;
	setupMotorPins();
	setupDCMotor();
	setupServo();

	int input10;
	int input20;
	float dutyCycle1;
	float dutyCycle2;

    /*************** Motor Task ***************/
	//Create Motor Queue
	motor_queue = xQueueCreate(1,sizeof(int));
	if (motor_queue == NULL)
	{
		PRINTF("motor_queue creation failed!.\r\n");
		while (1);
	}

	//Create Motor Task
	status = xTaskCreate(motorTask, "motorTask", 200, NULL, 2, NULL);
	if (status != pdPASS)
	{
		PRINTF("Task creation failed!.\r\n");
		while (1);
	}

    /*************** Position Task ***************/
	//Create Angle Queue
	angle_queue = xQueueCreate(1,sizeof(int));
	if (angle_queue == NULL)
	{
		PRINTF("angle_queue creation failed!.\r\n");
		while (1);
	}
	//Create Position Task
	status = xTaskCreate(positionTask, "positionTask", 200, NULL, 2, NULL);
	if (status != pdPASS)
	{
		PRINTF("Task creation failed!.\r\n");
		while (1);
	}
}

void setupMotorPins()
{
	/* Port C Clock Gate Control: Clock enabled */
	CLOCK_EnableClock(kCLOCK_PortC);
	/* PORTC1 is configured as PTC1 */
	PORT_SetPinMux(PORTC, 1U, kPORT_MuxAlt4);

	/* Port C Clock Gate Control: Clock enabled */
	CLOCK_EnableClock(kCLOCK_PortA);
	/* PORTC1 is configured as PTC1 */
	PORT_SetPinMux(PORTA, 6U, kPORT_MuxAlt3);
}

void setupDCMotor()
{
	ftm_config_t ftmInfo, ftmInfo2;
	ftm_chnl_pwm_signal_param_t ftmParam, ftmParam2;
	ftm_pwm_level_select_t pwmLevel = kFTM_HighTrue;

	ftmParam.chnlNumber = FTM_CHANNEL_DC_MOTOR;
	ftmParam.level = pwmLevel;
	ftmParam.dutyCyclePercent = 7;
	ftmParam.firstEdgeDelayPercent = 0U;
	ftmParam.enableComplementary = false;
	ftmParam.enableDeadtime = false;

	FTM_GetDefaultConfig(&ftmInfo);
	ftmInfo.prescale = kFTM_Prescale_Divide_128;

	FTM_Init(FTM_MOTORS, &ftmInfo);
	FTM_SetupPwm(FTM_MOTORS, &ftmParam, 1U, kFTM_EdgeAlignedPwm, 50U, CLOCK_GetFreq(
	kCLOCK_BusClk));
	FTM_StartTimer(FTM_MOTORS, kFTM_SystemClock);

	//////

	ftmParam2.chnlNumber = FTM_CHANNEL_SERVO;
	ftmParam2.level = pwmLevel;
	ftmParam2.dutyCyclePercent = 7;
	ftmParam2.firstEdgeDelayPercent = 0U;
	ftmParam2.enableComplementary = false;
	ftmParam2.enableDeadtime = false;

	FTM_SetupPwm(FTM_MOTORS, &ftmParam2, 1U, kFTM_EdgeAlignedPwm, 50U, CLOCK_GetFreq(
	kCLOCK_BusClk));
}

void setupServo()
{
}

void updatePWM_dutyCycle(ftm_chnl_t channel, float dutyCycle)
{
	uint32_t cnv, cnvFirstEdge = 0, mod;

	/* The CHANNEL_COUNT macro returns -1 if it cannot match the FTM instance */
	assert(-1 != FSL_FEATURE_FTM_CHANNEL_COUNTn(FTM_MOTORS));

	mod = FTM_MOTORS->MOD;
	if(dutyCycle == 0U)
	{
		/* Signal stays low */
		cnv = 0;
	}
	else
	{
		cnv = mod * dutyCycle;
		/* For 100% duty cycle */
		if (cnv >= mod)
		{
			cnv = mod + 1U;
		}
	}

	FTM_MOTORS->CONTROLS[channel].CnV = cnv;
}

void motorTask(void* pvParameters)
{
	BaseType_t status;
	int received_speed = 0;
	float dutyCycle1;

	while (1)
	{
		status = xQueueReceive(motor_queue, (void*) &received_speed, portMAX_DELAY);

		//PRINTF("\nReceived Speed = %d", received_speed);

		dutyCycle1 = received_speed * 0.025f/100.0f + 0.0615;

		int duty = dutyCycle1*1000;
		//printf("Duty Cycle 1: %d", duty);
		updatePWM_dutyCycle(FTM_CHANNEL_DC_MOTOR, dutyCycle1);
		FTM_SetSoftwareTrigger(FTM_MOTORS, true);
	}
}

void positionTask(void* pvParameters)
{
	BaseType_t status;
	int received_angle = 0;
	float dutyCycle2;

	while (1)
	{
		status = xQueueReceive(angle_queue, (void*) &received_angle, portMAX_DELAY);

		//PRINTF("\nReceived Angle = %d\n", received_angle);

		dutyCycle2 = received_angle * 0.025f/100.0f + 0.068;

		int duty2 = dutyCycle2*1000;
		//printf("\nDuty Cycle 2: %d", duty2);
		updatePWM_dutyCycle(FTM_CHANNEL_SERVO, dutyCycle2);
		FTM_SetSoftwareTrigger(FTM_MOTORS, true);
	}
}
