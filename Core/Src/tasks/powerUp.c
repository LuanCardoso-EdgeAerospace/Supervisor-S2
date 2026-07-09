#include "cmsis_os2.h"
#include "main.h"     // CubeMX GPIO pin/port defines
#include "tasks.h"

volatile enum powerState powerStatus = POWER_OFF;
#define BUTTON_FLAG   (1U << 0)

void PowerEnable(){
	queuedPrintf("Powering up LS1046: ");

//	osKernelLock();

	//Enable the 12V power rail
	HAL_GPIO_WritePin(EN_12V0P_GPIO_Port, EN_12V0P_Pin, GPIO_PIN_SET);

	uint32_t tickStart = HAL_GetTick();

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
//	queuedPrintf("LCL enabled\r\n");
//	osDelay(50); //Allows power to stabilize.

	//Enable PMIC
	HAL_GPIO_WritePin(PMIC_EN_GPIO_Port, PMIC_EN_Pin, GPIO_PIN_SET);

	powerStatus = POWER_ON;

//	osKernelUnlock();

	queuedPrintf("System online \r\n");
}

void PowerDisable(){
	queuedPrintf("Powering off LS1046: ");
//	osKernelLock();
	HAL_GPIO_WritePin(PMIC_EN_GPIO_Port, PMIC_EN_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LCL_1_EN_GPIO_Port, LCL_1_EN_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(EN_12V0P_GPIO_Port, EN_12V0P_Pin, GPIO_PIN_RESET);


	powerStatus = POWER_OFF;
//	osKernelUnlock();
	queuedPrintf("System offline \r\n");

}


void powerUp(void *argument){
//	PowerEnable();
//	for(;;){
//		osDelay(1000);
//	}
	(void)argument;
	PowerEnable();

    for(;;){
		//wait for button press
		uint32_t flags = osThreadFlagsWait(
			BUTTON_FLAG,
			osFlagsWaitAny,
			osWaitForever
		);

		if (powerStatus == POWER_OFF && (flags & BUTTON_FLAG) != 0U){
			PowerEnable();
		}

		else{
			PowerDisable();
		}
    }

    
}

extern osThreadId_t powerUpHandle;
#define DEBOUNCE_MS 500
void HAL_GPIO_EXTI_Falling_Callback(uint16_t GPIO_Pin){
	static uint32_t lastPressTick = 0;

	    if (GPIO_Pin == PUSH_BUTTON_Pin)
	    {
	        uint32_t now = HAL_GetTick();

	        if ((now - lastPressTick) >= DEBOUNCE_MS)
	        {
	            lastPressTick = now;

	            if (powerUpHandle != NULL)
	            {
	                osThreadFlagsSet(powerUpHandle, BUTTON_FLAG);
	            }
	        }
	    }
}
