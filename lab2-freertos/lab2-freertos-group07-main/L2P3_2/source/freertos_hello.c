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
#include "semphr.h"
#include "timers.h"

/* Freescale includes. */
#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"

char *str = NULL;
char *strCAP = NULL;
SemaphoreHandle_t input_semaphore;
SemaphoreHandle_t print_semaphore;
SemaphoreHandle_t capitalization_semaphore;

void hello_task(void *pvParameters)
{
    str = (char*) pvPortMalloc(100 * sizeof(char));  // Allocate memory for the string
    strCAP = (char*) pvPortMalloc(100 * sizeof(char));  // Allocate memory for the string
    if (str == NULL)
    {
        PRINTF("Memory allocation failed!\r\n");
        vTaskDelete(NULL);
    }

    if (strCAP == NULL)
	{
		PRINTF("Memory allocation failed!\r\n");
		vTaskDelete(NULL);
	}

    while(1)
    {
        printf("Task1 Input: ");
        scanf("%s", str);
        strcpy(strCAP, str);

        // Notify the other tasks that input has been taken
        xSemaphoreGive(input_semaphore);

        // Wait until the other tasks are ready to proceed
        xSemaphoreTake(print_semaphore, portMAX_DELAY);

        // Delete this task after receiving the input and printing
        vTaskDelete(NULL);
    }
}

void hello_task2(void *pvParameters)
{
	  // Wait for input to be taken before printing
	xSemaphoreTake(input_semaphore, portMAX_DELAY);

    while(1)
    {
        // Print the received string
        PRINTF("Received Value = %s\r\n", str);

        // Notify the task that capitalizes the string
        xSemaphoreGive(capitalization_semaphore);

        // Wait for the capitalized version to be printed
        xSemaphoreTake(print_semaphore, portMAX_DELAY);

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void hello_task3(void *pvParameters)
{
    while(1)
    {
        // Wait for the string to be ready for capitalization
        xSemaphoreTake(capitalization_semaphore, portMAX_DELAY);

        // Convert the string to uppercase
        for (int i = 0; strCAP[i] != '\0'; i++)
        {
            if (strCAP[i] >= 'a' && strCAP[i] <= 'z')
            {
                strCAP[i] = strCAP[i] - 'a' + 'A';
            }
        }

        // Print the capitalized string
        PRINTF("Capitalized String = %s\r\n", strCAP);

        // Notify hello_task2 to print again
        xSemaphoreGive(print_semaphore);

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

    // Create semaphores
    input_semaphore = xSemaphoreCreateBinary();   // Synchronize input
    print_semaphore = xSemaphoreCreateBinary();   // Synchronize printing
    capitalization_semaphore = xSemaphoreCreateBinary(); // Synchronize capitalization

    if (input_semaphore == NULL || print_semaphore == NULL || capitalization_semaphore == NULL)
    {
        PRINTF("Semaphore creation failed!\r\n");
        while (1);
    }

    // Create tasks
    status = xTaskCreate(hello_task, "Hello_task", 200, NULL, 2, NULL);
    if (status != pdPASS)
    {
        PRINTF("Task creation failed!.\r\n");
        while (1);
    }

    status = xTaskCreate(hello_task2, "Hello_task2", 200, NULL, 3, NULL);
    if (status != pdPASS)
    {
        PRINTF("Task creation failed!.\r\n");
        while (1);
    }

    status = xTaskCreate(hello_task3, "Hello_task3", 200, NULL, 3, NULL);
    if (status != pdPASS)
    {
        PRINTF("Task creation failed!.\r\n");
        while (1);
    }

    // Start the scheduler
    vTaskStartScheduler();
    for (;;);
}
