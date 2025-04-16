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
#include "semphr.h"
#include "fsl_uart.h"

/* Freescale includes. */
#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"

/* Semaphore handles */
SemaphoreHandle_t left_sem, right_sem, up_sem, down_sem;

/* Producer task */
void producer_task(void* pvParameters)
{
    char c;
    while (1)
    {
        scanf("%c", &c);  // Get user input

        switch(c)
        {
        case 'a':  // Left
            xSemaphoreGive(left_sem);  // Signal the left semaphore
            break;
        case 's':  // Down
            xSemaphoreGive(down_sem);  // Signal the down semaphore
            break;
        case 'd':  // Right
            xSemaphoreGive(right_sem);  // Signal the right semaphore
            break;
        case 'w':  // Up
            xSemaphoreGive(up_sem);  // Signal the up semaphore
            break;
        }

        vTaskDelay(pdMS_TO_TICKS(100)); // Delay to allow the consumer to process the event
    }
}

/* Consumer task */
void consumer_task(void* pvParameters)
{
    while (1)
    {
        // Wait for and process any semaphore without blocking others
        if (xSemaphoreTake(left_sem, pdMS_TO_TICKS(10)) == pdTRUE)
        {
            PRINTF("Left\r\n");
        }
        if (xSemaphoreTake(right_sem, pdMS_TO_TICKS(10)) == pdTRUE)
        {
            PRINTF("Right\r\n");
        }
        if (xSemaphoreTake(up_sem, pdMS_TO_TICKS(10)) == pdTRUE)
        {
            PRINTF("Up\r\n");
        }
        if (xSemaphoreTake(down_sem, pdMS_TO_TICKS(10)) == pdTRUE)
        {
            PRINTF("Down\r\n");
        }

        vTaskDelay(pdMS_TO_TICKS(10)); // Small delay to allow other tasks to run
    }
}

int main(void)
{
    BaseType_t status;

    /* Init board hardware. */
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitDebugConsole();

    /* Semaphore creation (initially not available) */
    left_sem = xSemaphoreCreateBinary();
    right_sem = xSemaphoreCreateBinary();
    up_sem = xSemaphoreCreateBinary();
    down_sem = xSemaphoreCreateBinary();

    /* Check if semaphores are created successfully */
    if (left_sem == NULL || right_sem == NULL || up_sem == NULL || down_sem == NULL)
    {
        PRINTF("Semaphore creation failed!\r\n");
        while (1);  // Lock the system if semaphore creation fails
    }

    /* Producer task creation */
    status = xTaskCreate(producer_task, "Producer", 200, NULL, 2, NULL);
    if (status != pdPASS)
    {
        PRINTF("Producer task creation failed!\r\n");
        while (1);
    }

    /* Consumer task creation */
    status = xTaskCreate(consumer_task, "Consumer", 200, NULL, 3, NULL);
    if (status != pdPASS)
    {
        PRINTF("Consumer task creation failed!\r\n");
        while (1);
    }

    /* Start scheduler */
    vTaskStartScheduler();

    while (1)
    {
    }
}
