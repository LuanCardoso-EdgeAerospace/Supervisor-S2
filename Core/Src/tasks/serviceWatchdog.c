#include "cmsis_os2.h"
#include "main.h"     // CubeMX GPIO pin/port defines
#include "tasks.h"

//before reactivating, remember to set the pin as pull up/down
void serviceWatchdog_(void *argument){
    uint32_t tick = osKernelGetTickCount();

    for (;;) {
        HAL_GPIO_TogglePin(MCU_WATCHDOG_PULSE_GPIO_Port,
                           MCU_WATCHDOG_PULSE_Pin);

        tick += WATCHDOG_SERVICE_INTERVAL_MS;
        osDelayUntil(tick); //Preferable so it keeps the task aligned on the 1 second boundary.
    }
}

