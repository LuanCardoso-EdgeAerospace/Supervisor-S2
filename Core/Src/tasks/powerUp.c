#include "cmsis_os2.h"
#include "main.h"     // CubeMX GPIO pin/port defines
#include "tasks.h"

volatile enum powerState powerStatus = POWER_OFF;
#define BUTTON_FLAG   (1U << 0)

void powerUp(void *argument){
	    uint32_t tickStart;

	    if (powerStatus == POWER_ON) {
	        osThreadExit();
	    }

    for(;;){
		uint32_t flags = osThreadFlagsWait(
			BUTTON_FLAG,
			osFlagsWaitAny,
			osWaitForever
		);
//		osDelay(30);   // debounce delay
//		if (HAL_GPIO_ReadPin(PUSH_BUTTON_GPIO_Port, PUSH_BUTTON_Pin) == GPIO_PIN_RESET){
			queuedPrintf("Push button event triggered!\r\n");
			osDelay(500);
			break;
//		}
    }

    queuedPrintf("Powering up LS1046, locking interrupts. \r\n");

	osKernelLock();

	//Enable the 12V power rail
	HAL_GPIO_WritePin(EN_12V0P_GPIO_Port, EN_12V0P_Pin, GPIO_PIN_SET);

	tickStart = HAL_GetTick();

	//wait for power-good signal
	//Note: Development hardware is missing a pullup resistor
	while (HAL_GPIO_ReadPin(PG_12V_PS_GPIO_Port, PG_12V_PS_Pin) != GPIO_PIN_SET) {
		if ((HAL_GetTick() - tickStart) >= POWER_GOOD_TIMEOUT_MS) {
			HAL_GPIO_WritePin(EN_12V0P_GPIO_Port, EN_12V0P_Pin, GPIO_PIN_RESET);
			powerStatus = POWER_ANOMALOUS;
			osKernelUnlock();
			queuedPrintf("Anomaly detected: Powergood signal timeout \r\n");
			osThreadExit();
		}
	}

	//TODO: Enable the Latching Current Limiter. Refer to https://eur.365.altium.com/librarycomponentsapi/api/v1/References/F8DC7D3A-E283-48E4-B7CA-8AD078A58B04
	HAL_GPIO_WritePin(LCL_1_EN_GPIO_Port, LCL_1_EN_Pin, GPIO_PIN_SET);
	queuedPrintf("LCL enabled\r\n");
//	osDelay(50); //Allows power to stabilize.

	//Enable PMIC
	HAL_GPIO_WritePin(PMIC_EN_GPIO_Port, PMIC_EN_Pin, GPIO_PIN_SET);

	powerStatus = POWER_ON;

	osKernelUnlock();

	queuedPrintf("System online \r\n");

	for(;;){
		osDelay(osWaitForever);
	}

}

extern osThreadId_t powerUpHandle;
void HAL_GPIO_EXTI_Falling_Callback(uint16_t GPIO_Pin){
	if (GPIO_Pin == PUSH_BUTTON_Pin)
	    {
	        if (powerUpHandle != NULL)
	        {
	            osThreadFlagsSet(powerUpHandle, BUTTON_FLAG);
	        }
	    }
}
