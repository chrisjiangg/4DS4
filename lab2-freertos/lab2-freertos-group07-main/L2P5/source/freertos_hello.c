/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "event_groups.h"
#include "fsl_uart.h"

/* Freescale includes. */
#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"

#define CONSUMER1_EVENT_BIT  (1 << 0)  // Event bit for Consumer1
#define CONSUMER2_EVENT_BIT  (1 << 1)  // Event bit for Consumer2
#define LEFT_BIT             (1 << 0)  // Event bit for left key press
#define DOWN_BIT             (1 << 1)  // Event bit for down key press
#define RIGHT_BIT            (1 << 2)  // Event bit for right key press
#define UP_BIT               (1 << 3)  // Event bit for up key press

#define TARGET_UART UART4

int counter = 0;
EventGroupHandle_t event_group_global_handler = NULL;  // Correctly initialize it to NULL

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

void consumer_event(void* pvParameters)
{
	EventGroupHandle_t event_group = (EventGroupHandle_t)pvParameters;
	EventBits_t bits;
	while(1)
	{
		bits = xEventGroupWaitBits(event_group,
		LEFT_BIT | RIGHT_BIT | UP_BIT | DOWN_BIT,
		pdTRUE,
		pdFALSE,
		portMAX_DELAY);
		if((bits & LEFT_BIT) == LEFT_BIT)
		{
		PRINTF("Left\r\n");
		}
		if((bits & RIGHT_BIT) == RIGHT_BIT)
		{
		PRINTF("Right\r\n");
		}
		if((bits & UP_BIT) == UP_BIT)
		{
		PRINTF("Up\r\n");
		}
		if((bits & DOWN_BIT) == DOWN_BIT)
		{
		PRINTF("Down\r\n");
		}
	}
}

/* UART setup function */
void setupUART()
{
    uart_config_t config;
    UART_GetDefaultConfig(&config);
    config.baudRate_Bps = 57600;
    config.enableTx = true;
    config.enableRx = true;
    config.enableRxRTS = true;
    config.enableTxCTS = true;
    UART_Init(TARGET_UART, &config, CLOCK_GetFreq(kCLOCK_BusClk));
    /******** Enable Interrupts *********/
    UART_EnableInterrupts(TARGET_UART, kUART_RxDataRegFullInterruptEnable);
    NVIC_SetPriority(UART4_RX_TX_IRQn, 2);
    EnableIRQ(UART4_RX_TX_IRQn);
}

/* UART interrupt handler */
void UART4_RX_TX_IRQHandler()
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    UART_GetStatusFlags(TARGET_UART);
    char ch = UART_ReadByte(TARGET_UART);

    switch(ch)
    {
        case 'a':
            xEventGroupSetBitsFromISR(event_group_global_handler, LEFT_BIT, &xHigherPriorityTaskWoken);
            break;
        case 's':
            xEventGroupSetBitsFromISR(event_group_global_handler, DOWN_BIT, &xHigherPriorityTaskWoken);
            break;
        case 'd':
            xEventGroupSetBitsFromISR(event_group_global_handler, RIGHT_BIT, &xHigherPriorityTaskWoken);
            break;
        case 'w':
            xEventGroupSetBitsFromISR(event_group_global_handler, UP_BIT, &xHigherPriorityTaskWoken);
            break;
    }
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

int main(void)
{
	BaseType_t status;
	/* Init board hardware. */
	BOARD_InitBootPins();
	BOARD_InitBootClocks();
	BOARD_InitDebugConsole();
	setupUART();
	event_group_global_handler = xEventGroupCreate();
	status = xTaskCreate(consumer_event, "consumer", 200, (void*)
	event_group_global_handler, 3, NULL);
	if (status != pdPASS)
	{
	PRINTF("Task creation failed!.\r\n");
	while (1);
	}
	vTaskStartScheduler();
	while (1)
	{}
}
