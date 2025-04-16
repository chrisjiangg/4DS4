#include "RC_Receiver_Component.h"

SemaphoreHandle_t rc_hold_semaphore;
TaskHandle_t rc_task_handle;

//QueueHandle_t led_queue;

typedef struct {
	uint16_t header;
	uint16_t ch1;
	uint16_t ch2;
	uint16_t ch3;
	uint16_t ch4;
	uint16_t ch5;
	uint16_t ch6;
	uint16_t ch7;
	uint16_t ch8;
} RC_Values;

void setupRCReceiverComponent()
{
	BaseType_t status;
	setupRCPins();
	setupUART_RC();

    /*************** RC Task ***************/
	//Create RC Semaphore
	//Create RC Task
	status = xTaskCreate(rcTask, "rcTask", 200, NULL, 2, NULL);
	if (status != pdPASS)
	{
		PRINTF("Task creation failed!.\r\n");
		while (1);
	}
}

void setupRCPins()
{
	CLOCK_EnableClock(kCLOCK_PortC);
	PORT_SetPinMux(PORTC, 3U, kPORT_MuxAlt3);
}

void setupUART_RC()
{
	uart_config_t config;
	UART_GetDefaultConfig(&config);
	config.baudRate_Bps = 115200;
	config.enableTx = false;
	config.enableRx = true;

	UART_Init(UART1, &config, CLOCK_GetFreq(kCLOCK_CoreSysClk));
}

void rcTask(void* pvParameters)
{
	RC_Values rc_values;
	uint8_t* ptr = (uint8_t*) &rc_values;
	BaseType_t status;
	int modeRC = 0;
	int direction = 1;
	int speed = 0;
	int angle = 0;

	while (1) {

		UART_ReadBlocking(UART1, ptr, 1);
		if(*ptr != 0x20)
			continue;

		UART_ReadBlocking(UART1, &ptr[1], sizeof(rc_values) - 1);
		if(rc_values.header == 0x4020)
		{
//			printf("Channel 1 = %d\t", rc_values.ch1);
//			printf("Channel 2 = %d\t", rc_values.ch2);
//			printf("Channel 3 = %d\t", rc_values.ch3);
//			printf("Channel 4 = %d\t", rc_values.ch4);
//			printf("Channel 5 = %d\t", rc_values.ch5);
//			printf("Channel 6 = %d\t", rc_values.ch6);
//			printf("Channel 7 = %d\t", rc_values.ch7);
//			printf("Channel 8 = %d\r\n", rc_values.ch8);
		}
		///////////////////////
		if(rc_values.ch7 == 2000) {
			modeRC = 1;
		}else if (rc_values.ch7 == 1500) {
			modeRC = 2;
		}else if (rc_values.ch7 == 1000) {
			modeRC = 3;
		}

		status = xQueueSendToBack(led_queue, &modeRC, portMAX_DELAY);
		//printf("\nSent LED = %d", modeRC);
		if (status != pdPASS)
		{
			PRINTF("Queue Send failed!.\r\n");
		}
		///////////////////////

		if(rc_values.ch8 == 1000) {
			direction = 1; //Forward
		}else {
			direction = -1; //Backward
		}
		speed = direction * (rc_values.ch2 - 1500) / 5;

		switch (modeRC)
		{
		case 1:
			speed *= 0.33;
			break;
		case 2:
			speed *= 0.66;
			break;
		case 3:
			speed *= 1;
			break;
		}
		if((direction == 1) && (speed < 0)) {
			speed = 0;
		}else if ((direction == -1) && (speed > 0)) {
			speed = 0;
		}

		angle = rc_values.ch4 * 0.2 - 300;

//		printf("\nSpeed: %d", speed);
//		printf("\nAngle: %d", angle);

		status = xQueueSendToBack(motor_queue, &speed, portMAX_DELAY);

		//printf("\nSent Speed = %d", speed);
		if (status != pdPASS)
		{
			PRINTF("Queue Send failed!.\r\n");
		}

		status = xQueueSendToBack(angle_queue, &angle, portMAX_DELAY);
		//printf("\nSent Angle = %d", angle);
		if (status != pdPASS)
		{
			PRINTF("Queue Send failed!.\r\n");
		}
	}
}
