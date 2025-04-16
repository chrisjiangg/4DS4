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

//void testFunction() {
//
//	int x = 0;
//	int *ptr = &x;
//	int *ptr_location = 0x20001000;
//
//	*ptr = 10;
//	*ptr_location = 11;
//
//}
//It reads\writes the value of 0x20001004
#define MEM_LOC(x) *((int*)x)
#define MEM_LOC_short(x) *((short int*)x)
#define MEM_LOC_char(x) *((char*)x)

#define Loc1 MEM_LOC_char(0x20001000)
#define Loc2 MEM_LOC(0x20001001)
#define Loc3 MEM_LOC_short(0x20001005)
#define Loc4 MEM_LOC(0x20001007)

typedef struct __attribute__((__packed__)) {
int location_1;
char location_2;
int location_3;
}MemoryStruct;

void testFunction() {

	Loc1 = 0xAC;
	Loc2 = 0xAABBCCDD;
	Loc3 = 0xABCD;
	Loc4 = 0xAABBCCDD;

}

#define module ((MemoryStruct*)0x20001000)

void testFunction2() {
//struct ARBITRARY_MODULE* module = (struct ARBITRARY_MODULE*)0x20001000;
module->location_1 = 0xAAAAAAAA;
module->location_2 = 0xBB;
module->location_3 = 0xCCCCCCCC;
}

int main(void) {
    char ch;

    /* Init board hardware. */
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitDebugConsole();

    PRINTF("hello world.\r\n");

    testFunction2();
    while (1)
    {
        ch = GETCHAR();
        PUTCHAR(ch);
    }
}
