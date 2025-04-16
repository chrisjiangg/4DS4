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
#include "event_groups.h"

/* Freescale includes. */
#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"

#define CONSUMER1_EVENT_BIT  (1 << 0)  // Event bit for Consumer1
#define CONSUMER2_EVENT_BIT  (1 << 1)  // Event bit for Consumer2

int counter = 0;

/* Producer task using Event Group */
void producer_event(void* pvParameters)
{
    EventGroupHandle_t event_group = (EventGroupHandle_t) pvParameters;

    while(1)
    {
        // Producer increments the counter
        counter++;

        // Set event bits to trigger both consumers to process the counter value
        xEventGroupSetBits(event_group, CONSUMER1_EVENT_BIT | CONSUMER2_EVENT_BIT);

        // Simulate some delay before next increment
        vTaskDelay(1000 / portTICK_PERIOD_MS);  // Simulate work done by the producer
    }
}

/* Consumer 1 task using Event Group */
void consumer1_event(void* pvParameters)
{
    EventGroupHandle_t event_group = (EventGroupHandle_t) pvParameters;
    EventBits_t bits;

    while(1)
    {
        // Wait for Consumer1's event bit to be set
        bits = xEventGroupWaitBits(event_group, CONSUMER1_EVENT_BIT, pdTRUE, pdFALSE, portMAX_DELAY);

        if (bits & CONSUMER1_EVENT_BIT)
        {
            // Consumer1 processes the counter value
            PRINTF("Consumer1 received value = %d\r\n", counter);

            // Reset event bit after processing
            xEventGroupClearBits(event_group, CONSUMER1_EVENT_BIT);
        }

        // Give the other consumer a chance to process if it's ready
        vTaskDelay(10 / portTICK_PERIOD_MS);  // Small delay to allow context switching
    }
}

/* Consumer 2 task using Event Group */
void consumer2_event(void* pvParameters)
{
    EventGroupHandle_t event_group = (EventGroupHandle_t) pvParameters;
    EventBits_t bits;

    while(1)
    {
        // Wait for Consumer2's event bit to be set
        bits = xEventGroupWaitBits(event_group, CONSUMER2_EVENT_BIT, pdTRUE, pdFALSE, portMAX_DELAY);

        if (bits & CONSUMER2_EVENT_BIT)
        {
            // Consumer2 processes the counter value
            PRINTF("Consumer2 received value = %d\r\n", counter);

            // Reset event bit after processing
            xEventGroupClearBits(event_group, CONSUMER2_EVENT_BIT);
        }

        // Give the other consumer a chance to process if it's ready
        vTaskDelay(10 / portTICK_PERIOD_MS);  // Small delay to allow context switching
    }
}

int main(void)
{
    BaseType_t status;
    /* Init board hardware. */
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitDebugConsole();

    // Create an event group for managing events between producer and consumers
    EventGroupHandle_t event_group = xEventGroupCreate();

    if (event_group == NULL)
    {
        PRINTF("Failed to create event group!\r\n");
        while (1);
    }

    // Create producer and consumer tasks
    status = xTaskCreate(producer_event, "Producer", 200, (void*)event_group, 2, NULL);
    if (status != pdPASS)
    {
        PRINTF("Producer task creation failed!\r\n");
        while (1);
    }

    status = xTaskCreate(consumer1_event, "Consumer1", 200, (void*)event_group, 2, NULL);
    if (status != pdPASS)
    {
        PRINTF("Consumer1 task creation failed!\r\n");
        while (1);
    }

    status = xTaskCreate(consumer2_event, "Consumer2", 200, (void*)event_group, 2, NULL);
    if (status != pdPASS)
    {
        PRINTF("Consumer2 task creation failed!\r\n");
        while (1);
    }

    // Start the scheduler
    vTaskStartScheduler();
    while (1)
    {}
}
