/*
 * Copyright (c) 2013 - 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_ftm.h"
#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"

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

void pwm_setup() {
	ftm_config_t ftmInfo, ftmInfo2, ftmInfo3;
	ftm_chnl_pwm_signal_param_t ftmParam, ftmParam2, ftmParam3;

	ftmParam.chnlNumber = kFTM_Chnl_1;
	ftmParam.level = kFTM_HighTrue;
	ftmParam.dutyCyclePercent = 0;
	ftmParam.firstEdgeDelayPercent = 0U;

	ftmParam.enableComplementary = false;
	ftmParam.enableDeadtime = false;

	FTM_GetDefaultConfig(&ftmInfo);

	FTM_Init(FTM3, &ftmInfo);
	FTM_SetupPwm(FTM3, &ftmParam, 1U, kFTM_EdgeAlignedPwm, 5000U, CLOCK_GetFreq(
	kCLOCK_BusClk));
	FTM_StartTimer(FTM3, kFTM_SystemClock);


	ftmParam2.chnlNumber = kFTM_Chnl_5;
	ftmParam2.level = kFTM_HighTrue;
	ftmParam2.dutyCyclePercent = 0;
	ftmParam2.firstEdgeDelayPercent = 0U;

	ftmParam2.enableComplementary = false;
	ftmParam2.enableDeadtime = false;

	FTM_GetDefaultConfig(&ftmInfo2);

	FTM_Init(FTM3, &ftmInfo2);
	FTM_SetupPwm(FTM3, &ftmParam2, 8U, kFTM_EdgeAlignedPwm, 5000U, CLOCK_GetFreq(
	kCLOCK_BusClk));
	FTM_StartTimer(FTM3, kFTM_SystemClock);


	ftmParam3.chnlNumber = kFTM_Chnl_4;
	ftmParam3.level = kFTM_HighTrue;
	ftmParam3.dutyCyclePercent = 0;
	ftmParam3.firstEdgeDelayPercent = 0U;

	ftmParam3.enableComplementary = false;
	ftmParam3.enableDeadtime = false;

	FTM_GetDefaultConfig(&ftmInfo3);

	FTM_Init(FTM3, &ftmInfo3);
	FTM_SetupPwm(FTM3, &ftmParam3, 9U, kFTM_EdgeAlignedPwm, 5000U, CLOCK_GetFreq(kCLOCK_BusClk));
	FTM_StartTimer(FTM3, kFTM_SystemClock);
}

int main(void)
{
    char input[10];
    int input1 = 0;
    int input2 = 0;
    int input3 = 0;

    /* Init board hardware. */
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitDebugConsole();

    pwm_setup();

    /* Read RGB hex-code from the console */
	printf("Enter RGB hex-code (e.g., FFFF00):\r\n");
	scanf("%2x%2x%2x", &input1, &input2, &input3);

	input1 = (input1*100)/255;
	input2 = (input2*100)/255;
	input3 = (input3*100)/255;

	printf("Red %d, Green %d, Blue %d", input1, input2, input3);

	FTM_UpdatePwmDutycycle(FTM3, kFTM_Chnl_1, kFTM_EdgeAlignedPwm, input1);
	FTM_SetSoftwareTrigger(FTM3, true);

	FTM_UpdatePwmDutycycle(FTM3, kFTM_Chnl_5, kFTM_EdgeAlignedPwm, input2);
	FTM_SetSoftwareTrigger(FTM3, true);

    FTM_UpdatePwmDutycycle(FTM3, kFTM_Chnl_4, kFTM_EdgeAlignedPwm, input3);
    FTM_SetSoftwareTrigger(FTM3, true);

    while (1)
    {
    }
}
