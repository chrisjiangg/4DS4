/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

//#include <gpio_led_output.h>
#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"
#include "fsl_debug_console.h"
//#include "fsl_gpio.h"
#include "gpio_led_output.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define BOARD_LED_GPIOC GPIOC
#define BOARD_LED_GPIOD GPIOD
#define BOARD_LED_GPIO_PIN8 8
#define BOARD_LED_GPIO_PIN9 9
#define BOARD_LED_GPIO_PIN1 1

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/*!
 * @brief delay a while.
 */
void delay(void);

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/
void delay(void)
{
    volatile uint32_t i = 0;
    for (i = 0; i < 800000; ++i)
    {
        __asm("NOP"); /* delay */
    }
}

void GPIOTogglePin(GPIO_Types *Port, uint32_t pin){
	Port->PTOR = pin;
}

void GPIOClearPin(GPIO_Types *Port){
	Port->PCOR = 0xFFFF;
}

void GPIOPinInit(GPIO_Types *Port, uint32_t pin, gpio_pin_config_t *config){
    assert(NULL != config);

    if (config->pinDirection == kGPIO_DigitalInput)
    {
    	Port->PDDR &= (~(1UL << pin));
    }
    else
    {
        Port->PDDR |= ((1UL << pin));
    }
}

/*!
 * @brief Main function
 */
int main(void)
{
    /* Define the init structure for the output LED pin*/
    gpio_pin_config_t led_config = {
        kGPIO_DigitalOutput,
        0,
    };

    /* Board pin, clock, debug console init */
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitDebugConsole();

//    BOARD_InitPins();

    /* Print a note to terminal. */
    PRINTF("\r\n GPIO Driver example\r\n");
    PRINTF("\r\n The LED is blinking.\r\n");

    BOARD_InitPins();

    /* Init output LED GPIO. */
    GPIOPinInit(BOARD_LED_GPIOC, BOARD_LED_GPIO_PIN8, &led_config);
    GPIOPinInit(BOARD_LED_GPIOC, BOARD_LED_GPIO_PIN9, &led_config);
    GPIOPinInit(BOARD_LED_GPIOD, BOARD_LED_GPIO_PIN1, &led_config);

    while (1)
    {
        delay();
        GPIOTogglePin(BOARD_LED_GPIOC, 1u << BOARD_LED_GPIO_PIN8);
        delay();
        GPIOClearPin(BOARD_LED_GPIOC);
        delay();
        GPIOTogglePin(BOARD_LED_GPIOC, 1u << BOARD_LED_GPIO_PIN9);
        delay();
        GPIOClearPin(BOARD_LED_GPIOC);
        delay();
        GPIOTogglePin(BOARD_LED_GPIOD, 1u << BOARD_LED_GPIO_PIN1);
        delay();
        GPIOClearPin(BOARD_LED_GPIOD);
    }
}
