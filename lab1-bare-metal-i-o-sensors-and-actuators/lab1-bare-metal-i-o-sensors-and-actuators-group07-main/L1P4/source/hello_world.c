/*
 * Copyright (c) 2013 - 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"
#include "fsl_dspi.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/


/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 * @brief Main function
 */

void setupSPI()
{
	dspi_master_config_t masterConfig;
	/* Master config */
	masterConfig.whichCtar = kDSPI_Ctar0;
	masterConfig.ctarConfig.baudRate = 500000;
	masterConfig.ctarConfig.bitsPerFrame = 8U;
	masterConfig.ctarConfig.cpol = kDSPI_ClockPolarityActiveHigh;
	masterConfig.ctarConfig.cpha = kDSPI_ClockPhaseFirstEdge;
	masterConfig.ctarConfig.direction = kDSPI_MsbFirst;
	masterConfig.ctarConfig.pcsToSckDelayInNanoSec = 1000000000U / 500000;
	masterConfig.ctarConfig.lastSckToPcsDelayInNanoSec = 1000000000U / 500000;
	masterConfig.ctarConfig.betweenTransferDelayInNanoSec = 1000000000U / 500000;

	masterConfig.whichPcs = kDSPI_Pcs0;
	masterConfig.pcsActiveHighOrLow = kDSPI_PcsActiveLow;

	masterConfig.enableContinuousSCK = false;
	masterConfig.enableRxFifoOverWrite = false;
	masterConfig.enableModifiedTimingFormat = false;
	masterConfig.samplePoint = kDSPI_SckToSin0Clock;
	DSPI_MasterInit(SPI1, &masterConfig, BUS_CLK);
}

void voltageRegulatorEnable()
{
	gpio_pin_config_t pin_config = {
	.pinDirection = kGPIO_DigitalOutput,
	.outputLogic = 0U};
	GPIO_PinInit(GPIOB, 8, &pin_config);
	GPIO_PinWrite(GPIOB, 8, 1U);
}

void accelerometerEnable()
{
	gpio_pin_config_t pin_config = {
	.pinDirection = kGPIO_DigitalOutput,
	.outputLogic = 0U};
	GPIO_PinInit(GPIOA, 25, &pin_config);
	GPIO_PinWrite(GPIOA, 25, 0U);
}

status_t SPI_read(uint8_t regAddress, uint8_t *rxBuff, uint8_t rxBuffSize)
{
	dspi_transfer_t masterXfer;
	uint8_t *masterTxData = (uint8_t*)malloc(rxBuffSize + 2);
	uint8_t *masterRxData = (uint8_t*)malloc(rxBuffSize + 2);
	masterTxData[0] = regAddress & 0x7F; //Clear the most significant bit
	masterTxData[1] = regAddress & 0x80; //Clear the least significant 7 bits
	masterXfer.txData = masterTxData;
	masterXfer.rxData = masterRxData;
	masterXfer.dataSize = rxBuffSize + 2;
	masterXfer.configFlags = kDSPI_MasterCtar0 | kDSPI_MasterPcs0 |
	kDSPI_MasterPcsContinuous;
	status_t ret = DSPI_MasterTransferBlocking(SPI1, &masterXfer);
	memcpy(rxBuff, &masterRxData[2], rxBuffSize);
	free(masterTxData);
	free(masterRxData);
	return ret;
	}

status_t SPI_write(uint8_t regAddress, uint8_t value)
{
    dspi_transfer_t spiTransferConfig;
    uint8_t *spiTransferData = (uint8_t *)malloc(sizeof(typeof(value)) + 2);

    spiTransferData[0] = regAddress | 0x80; // Set the most significant bit
    spiTransferData[1] = regAddress & 0x80; // Clear the least significant 7 bits
    spiTransferData[2] = value;             // Add value to stream

    spiTransferConfig.txData = spiTransferData;
    spiTransferConfig.rxData = NULL;
    spiTransferConfig.dataSize = sizeof(typeof(value)) + 2;
    spiTransferConfig.configFlags = kDSPI_MasterCtar0 | kDSPI_MasterPcs0 | kDSPI_MasterPcsContinuous;
    status_t output = DSPI_MasterTransferBlocking(SPI1, &spiTransferConfig);

    free(spiTransferData);

    return output;
}

int main(void)
{
	uint8_t byte;

	/* Init board hardware. */
	BOARD_InitBootPins();
	BOARD_InitBootClocks();
	BOARD_InitDebugConsole();

	setupSPI();
	voltageRegulatorEnable();
	accelerometerEnable();

	/******* Delay *******/
	for (volatile int i = 0U; i < 1000000; i++)
		__asm("NOP");

	// Address 0x0D (WHO_AM_I) has constant value 0xC7
	SPI_read(0x0D, &byte, 1);
	printf("The expected value is 0xC7 and the read value 0x%X\n", byte);

	SPI_write(0x13, 0x66);
	SPI_read(0x13, &byte, 1);
	printf("The expected value is 0x66 and the read value 0x%X\n", byte);

	SPI_write(0x13, 0x88);
	SPI_read(0x13, &byte, 1);
	printf("The expected value is 0x88 and the read value 0x%X\n", byte);

	while (1)
	{
	}
}

