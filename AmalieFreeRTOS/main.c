#include "main.h"
#include "functions.h"

#include <avr/io.h>
#include <util/delay.h>

// Free-RTOS includes
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "portmacro.h"
#include "FreeRTOSConfig.h"

int count = 0;
float u_k1[4];
int reading;
xTaskHandle xHandle;
//SemaphoreHandle_t xSemaphore = NULL;

int main()
{
	// Initialization
	PWM_init(0);
	USART_Init(MYUBRR);
	LED_DDR = 0xFF;
	LED = 0x00;
	LED2_DDR = 0xFF;
	LED2 = 0x00;

	// Task Creation
	//xSemaphore = xSemaphoreCreateMutex();
	xTaskCreate(Controllers, "Control", 1000, NULL, configMAX_PRIORITIES - 1, NULL );
	xTaskCreate(Comunication, "Com", 1000, NULL, configMAX_PRIORITIES - 2, &xHandle);

	//while (1)
	//{
	//	LED = 0xFF;
	//	_delay_ms(1000);
	//	LED = 0x00;
	//	_delay_ms(1000);
	//}

	LED |= (~0x00);
	_delay_ms(1000);
	int duty = 128;
	Set_PWM_duty(duty, duty, duty, duty);
	_delay_ms(10000);

	//unsigned char dummy[21] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	//int i = 0;
	//while (1)
	//{
	//	//LED = 0xFF;
	//	for (i = 0; i < 1; i++)
	//	{
	//		dummy[i] = USART_Receive();
	//	}
	//	for (i = 0; i < 1; i++)
	//	{
	//		USART_Transmit(dummy[i]);
	//	}
	//}

	// Scheduler Start
	vTaskStartScheduler();
	return 0;
}


void Controllers(void *pvParameters)
{
	portTickType xLastWakeTime;
	xLastWakeTime = xTaskGetTickCount();

	while (1)
	{
				if (count<400)
				{
				LED = 0xFF;
				AngularController();
				count++;
				ApplyVelocities();
				LED = 0x00;
				}
				else
				{
					Set_PWM_duty(128, 128, 128, 128);
					count = 1200;
				}
				if (reading)
				{
					vTaskDelete(xHandle);
					xTaskCreate(Comunication, "Com", 1000, NULL, configMAX_PRIORITIES - 2, &xHandle);
					reading = 0;
				}
				vTaskDelayUntil(&xLastWakeTime, 35);
	}
	vTaskDelete(NULL);
}


void Comunication(void *pvParameters)
{
	while (1)
	{
		int pack = 0;
		pack = CheckPackageArrival();
		if (pack)
		{
			reading = 1;
			//vTaskSuspendAll();
			GetPackage();
			//xTaskResumeAll();
			reading = 0;
		}
	}
	vTaskDelete(NULL);
}


void vApplicationIdleHook( void ) //N�r FreeRTOS ikke har en task den skal lave
{
}