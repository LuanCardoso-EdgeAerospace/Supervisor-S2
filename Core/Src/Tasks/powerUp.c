#include "cmsis_os2.h"
#include "main.h"     // CubeMX GPIO pin/port defines
#include "tasks.h"

volatile enum powerState powerStatus = POWER_OFF;
#define BUTTON_FLAG   (1U << 0)

void powerUp(void *argument)
{
    uint32_t tickStart;

    if (powerStatus == POWER_ON) {
        osThreadExit();
    }

//    for(;;){
//		uint32_t flags = osThreadFlagsWait(
//			BUTTON_FLAG,
//			osFlagsWaitAny,
//			osWaitForever
//		);
//		osDelay(30);   // debounce delay
//		if (HAL_GPIO_ReadPin(PUSH_BUTTON_GPIO_Port, PUSH_BUTTON_Pin) == GPIO_PIN_RESET) break;
//    }

    osKernelLock();

    //Enable the 12V power rail
    HAL_GPIO_WritePin(EN_12V0P_GPIO_Port, EN_12V0P_Pin, GPIO_PIN_SET);

    tickStart = HAL_GetTick();

    //wait for power-good signal
    while (HAL_GPIO_ReadPin(PG_12V_PS_GPIO_Port, PG_12V_PS_Pin) != GPIO_PIN_SET) {
        if ((HAL_GetTick() - tickStart) >= POWER_GOOD_TIMEOUT_MS) {
            HAL_GPIO_WritePin(EN_12V0P_GPIO_Port, EN_12V0P_Pin, GPIO_PIN_RESET);
            powerStatus = POWER_ANOMALOUS;
            osKernelUnlock();
            osThreadExit();
        }
    }

    //TODO: Enable the Latching Current Limiter. Refer to https://eur.365.altium.com/librarycomponentsapi/api/v1/References/F8DC7D3A-E283-48E4-B7CA-8AD078A58B04

    //Enable PMIC
    HAL_GPIO_WritePin(PMIC_EN_GPIO_Port, PMIC_EN_Pin, GPIO_PIN_SET);

    powerStatus = POWER_ON;

    osKernelUnlock();

    queuedPrintf("System online \r\n");

    osThreadExit();
}

void powerDown(void *argument)
{
    uint32_t tickStart;

    if (powerStatus == POWER_OFF) {
        osThreadExit();
    }

    osKernelLock();

    //disable PMIC
    HAL_GPIO_WritePin(PMIC_EN_GPIO_Port, PMIC_EN_Pin, GPIO_PIN_RESET);

    //TODO: Disable the latching current limiter

    //disable 12V power rail
    HAL_GPIO_WritePin(EN_12V0P_GPIO_Port, EN_12V0P_Pin, GPIO_PIN_RESET);

    tickStart = HAL_GetTick();

    //Check that PG signal went down.
    while (HAL_GPIO_ReadPin(PG_12V_PS_GPIO_Port, PG_12V_PS_Pin) != GPIO_PIN_RESET) {
        if ((HAL_GetTick() - tickStart) >= POWER_GOOD_TIMEOUT_MS) {
            powerStatus = POWER_ANOMALOUS;
            osKernelUnlock();
            osThreadExit();
        }
    }

    powerStatus = POWER_OFF;

    osKernelUnlock();

    osThreadExit();
}
//extern osThreadId_t powerUpHandle;
//void HAL_GPIO_EXTI_Falling_Callback(uint16_t GPIO_Pin){
//	if (GPIO_Pin == PUSH_BUTTON_Pin)
//	    {
//	        if (powerUpHandle != NULL)
//	        {
//	            osThreadFlagsSet(powerUpHandle, BUTTON_FLAG);
//	        }
//	    }
//}

