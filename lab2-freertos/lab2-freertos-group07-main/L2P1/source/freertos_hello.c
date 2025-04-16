/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"

/* Freescale includes. */
#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"

char* str;

void hello_task(void *pvParameters)
{
	str = (char*) pvPortMalloc(100 * sizeof(char));
	if (str == NULL)
	{
		PRINTF("Memory allocation failed!\r\n");
		vTaskDelete(NULL);
	}

	while(1)
	{
		printf("Task1 Input: ");
		scanf("%s", str);
		vTaskDelete(NULL);
	}
}

void hello_task2(void *pvParameters)
{
	while(1)
	{
		PRINTF("%s\r\n", str);
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}

int main(void)
{
	BaseType_t status;
	/* Init board hardware. */
	BOARD_InitBootPins();
	BOARD_InitBootClocks();
	BOARD_InitDebugConsole();

	status = xTaskCreate(hello_task, "Hello_task", 200, NULL, 2, NULL);

	if (status != pdPASS)
	{
		PRINTF("Task creation failed!.\r\n");
		while (1);
	}

	status = xTaskCreate(hello_task2, "Hello_task2", 200, (void*)str, 3, NULL);
	if (status != pdPASS)
	{
		PRINTF("Task creation failed!.\r\n");
		while (1);
	}

	vTaskStartScheduler();
	for (;;);
}
