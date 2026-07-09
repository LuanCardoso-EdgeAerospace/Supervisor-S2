#include "cmsis_os2.h"
#include "main.h"     // CubeMX GPIO pin/port defines
#include "tasks.h"

//before reactivating, remember to set the pin as pull up/down
void serviceWatchdog(void *argument){
    uint32_t tick = osKernelGetTickCount();

    for (;;) {
        // HAL_GPIO_TogglePin(MCU_WATCHDOG_PULSE_GPIO_Port,
        //                    MCU_WATCHDOG_PULSE_Pin);
        HAL_GPIO_TogglePin(LEDRED_GPIO_Port, LEDRED_Pin);
        tick += WATCHDOG_SERVICE_INTERVAL_MS;
//        queuedPrintf(">>Dog petted!\r\n");
        osDelayUntil(tick); //Preferable so it keeps the task aligned on the 1 second boundary.
    }


    //if Pin is open drain, this sets it to hiZ
    //  HAL_GPIO_WritePin(MCU_WATCHDOG_PULSE_GPIO_Port,
    //                     MCU_WATCHDOG_PULSE_Pin, GPIO_PIN_SET);
    //Pin in HiZ will cause the watchdog to feed itself.
}

