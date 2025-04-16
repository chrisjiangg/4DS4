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
#include "queue.h"

/* Freescale includes. */
#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"

void producer_queue(void* pvParameters)
{
    QueueHandle_t queue1 = (QueueHandle_t)pvParameters;
    BaseType_t status;
    char *str = NULL;

    while (1)
    {
        str = (char*) pvPortMalloc(100 * sizeof(char));  // Allocate memory for the string

        if (str == NULL)
        {
            PRINTF("Memory allocation failed!\r\n");
            vTaskDelete(NULL);
        }

        printf("Task1 Input: ");
        scanf("%s", str);

        // Send a copy of the string (pointer) to the queue
        status = xQueueSendToBack(queue1, &str, portMAX_DELAY);
        if (status != pdPASS)
        {
            PRINTF("Queue Send failed!.\r\n");
            vTaskDelete(NULL);
        }

        // Don't forget to free the memory after sending the data, if necessary
        // In this case, you may want to free it after the consumer task uses it
        vTaskDelete(NULL);
    }
}

void consumer_queue(void* pvParameters)
{
    QueueHandle_t queue1 = (QueueHandle_t)pvParameters;
    BaseType_t status;
    char* received_string = NULL;

    // Receive the string pointer from the queue
	status = xQueueReceive(queue1, (void *) &received_string, portMAX_DELAY);
	if (status != pdPASS)
	{
		PRINTF("Queue Receive failed!.\r\n");
	}

    while (1)
    {
        // Print the received string
        PRINTF("Received Value = %s\r\n", received_string);

        // Delay for next iteration
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

    QueueHandle_t queue1 = xQueueCreate(1, sizeof(char*));  // Queue will now hold pointers to char arrays

    if (queue1 == NULL)
    {
        PRINTF("Queue creation failed!.\r\n");
        while (1);
    }

    status = xTaskCreate(producer_queue, "producer", 200, (void*)queue1, 2, NULL);
    if (status != pdPASS)
    {
        PRINTF("Task creation failed!.\r\n");
        while (1);
    }

    status = xTaskCreate(consumer_queue, "consumer", 200, (void*)queue1, 3, NULL);
    if (status != pdPASS)
    {
        PRINTF("Task creation failed!.\r\n");
        while (1);
    }

    vTaskStartScheduler();
    while (1)
    {
    }
}
