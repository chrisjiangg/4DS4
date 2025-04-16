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
#include "semphr.h"

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

int counter = 0;
void producer_sem(void* pvParameters)
{
	SemaphoreHandle_t* semaphores = (SemaphoreHandle_t*)pvParameters;
	SemaphoreHandle_t producer1_semaphore = semaphores[0];
	SemaphoreHandle_t producer2_semaphore = semaphores[1];
	SemaphoreHandle_t consumer_semaphore = semaphores[2];
	BaseType_t status1, status2;

	while(1)
	{
		status1 = xSemaphoreTake(consumer_semaphore, portMAX_DELAY);
		status2 = xSemaphoreTake(consumer_semaphore, portMAX_DELAY);
		if (status1 != pdPASS || status2 != pdPASS)
		{
			PRINTF("Failed to acquire consumer_semaphore\r\n");
			while (1);
		}

		counter++;
		xSemaphoreGive(producer1_semaphore);
		xSemaphoreGive(producer2_semaphore);

		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}

void consumer1_sem(void* pvParameters)
{
	SemaphoreHandle_t* semaphores = (SemaphoreHandle_t*)pvParameters;
	SemaphoreHandle_t producer1_semaphore = semaphores[0];
	SemaphoreHandle_t consumer_semaphore = semaphores[2];
	BaseType_t status;
	while(1)
	{
		xSemaphoreGive(consumer_semaphore);
		status = xSemaphoreTake(producer1_semaphore, portMAX_DELAY);
		if (status != pdPASS)
		{
			PRINTF("Failed to acquire producer1_semaphore\r\n");
			while (1);
		}
		PRINTF("Received Value = %d\r\n", counter);
	}
}

void consumer2_sem(void* pvParameters)
{
	SemaphoreHandle_t* semaphores = (SemaphoreHandle_t*)pvParameters;
	SemaphoreHandle_t producer2_semaphore = semaphores[1];
	SemaphoreHandle_t consumer_semaphore = semaphores[2];
	BaseType_t status;
	while(1)
	{
		xSemaphoreGive(consumer_semaphore);
		status = xSemaphoreTake(producer2_semaphore, portMAX_DELAY);
		if (status != pdPASS)
		{
			PRINTF("Failed to acquire producer2_semaphore\r\n");
			while (1);
		}
		PRINTF("Received Value = %d\r\n", counter);
	}
}

int main(void)
{
	BaseType_t status;
	/* Init board hardware. */
	BOARD_InitBootPins();
	BOARD_InitBootClocks();
	BOARD_InitDebugConsole();

	SemaphoreHandle_t* semaphores = (SemaphoreHandle_t*) malloc(3 * sizeof(
	SemaphoreHandle_t));
	semaphores[0] = xSemaphoreCreateBinary(); //Producer1_sem
	semaphores[1] = xSemaphoreCreateBinary(); //Producer2_sem
	semaphores[2] = xSemaphoreCreateCounting(2, 2); //consumer_sem

	status = xTaskCreate(producer_sem, "producer", 200, (void*)semaphores, 2, NULL);
	if (status != pdPASS)
	{
		PRINTF("Task creation failed!.\r\n");
		while (1);
	}

	status = xTaskCreate(consumer1_sem, "consumer", 200, (void*)semaphores, 2, NULL);
	if (status != pdPASS)
	{
		PRINTF("Task creation failed!.\r\n");
		while (1);
	}

	status = xTaskCreate(consumer2_sem, "consumer", 200, (void*)semaphores, 2, NULL);
	if (status != pdPASS)
	{
		PRINTF("Task creation failed!.\r\n");
		while (1);
	}

	vTaskStartScheduler();
	while (1)
	{}
}
