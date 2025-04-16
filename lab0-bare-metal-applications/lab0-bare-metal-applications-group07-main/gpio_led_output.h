/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2021 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef GPIO_LED_OUTPUT_H_
#define GPIO_LED_OUTPUT_H_
#include "core_cm4.h"                  /* Core Peripheral Access Layer */

typedef struct {
	__IO uint32_t PDOR;                              /**< Port Data Output Register, offset: 0x0 */
	__O  uint32_t PSOR;                              /**< Port Set Output Register, offset: 0x4 */
	__O  uint32_t PCOR;                              /**< Port Clear Output Register, offset: 0x8 */
	__O  uint32_t PTOR;                              /**< Port Toggle Output Register, offset: 0xC */
	__I  uint32_t PDIR;                              /**< Port Data Input Register, offset: 0x10 */
	__IO uint32_t PDDR;                              /**< Port Data Direction Register, offset: 0x14 */
} GPIO_Types;

void GPIOTogglePin(GPIO_Types *Port, uint32_t pin);

void GPIOClearPin(GPIO_Types *Port);

void GPIOPinInit(GPIO_Types *Port, uint32_t pin, gpio_pin_config_t *config);

#endif /* GPIO_LED_OUTPUT_H_ */
