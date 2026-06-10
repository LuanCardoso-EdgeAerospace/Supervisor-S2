#include "cmsis_os2.h"
#include "main.h"     // CubeMX GPIO pin/port defines
#include "tasks.h"

void serviceWatchdog(void *argument){
    uint32_t tick = osKernelGetTickCount();

    for (;;) {
        HAL_GPIO_TogglePin(MCU_WATCHDOG_PULSE_GPIO_Port,
                           MCU_WATCHDOG_PULSE_Pin);

        HAL_GPIO_TogglePin(LEDRED_GPIO_Port, LEDRED_Pin);

        tick += WATCHDOG_SERVICE_INTERVAL_MS;
        osDelayUntil(tick); //Preferable so it keeps the task aligned on the 1 second boundary.
    }
}

