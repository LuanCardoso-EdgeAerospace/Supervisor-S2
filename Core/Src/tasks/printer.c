#include "cmsis_os2.h"
#include "main.h"     // CubeMX GPIO pin/port defines
#include "tasks.h"
#include "stdio.h"
#include "string.h"
#include "app_freertos.h"

extern UART_HandleTypeDef hlpuart1;

int queuedPrintf(const char *format, ...) {
    char msg[PRINT_QUEUE_ITEM_SIZE];
    va_list args;
    int len;
    osStatus_t status;

    if ((format == NULL) || (printQueueHandle == NULL)) {
        return -1;
    }

    va_start(args, format);
    len = vsnprintf(msg, sizeof(msg), format, args);
    va_end(args);

    if (len < 0) {
        return -1;
    }

    // Ensure null termination even if the message was truncated.
    msg[PRINT_QUEUE_ITEM_SIZE - 1U] = '\0';

    /*
     * Timeout = 0 makes this non-blocking.
     * If the queue is full, the message is dropped.
     */
    status = osMessageQueuePut(printQueueHandle, msg, 0U, 0U);

    if (status != osOK) {
        return -1;
    }

    return len;
}

void printer(void *argument) {
    char msg[PRINT_QUEUE_ITEM_SIZE];

    for (;;) {
        if (osMessageQueueGet(printQueueHandle, msg, NULL, osWaitForever) == osOK) {

            msg[PRINT_QUEUE_ITEM_SIZE - 1U] = '\0';

            HAL_GPIO_WritePin(MCU_RS422_EN_GPIO_Port, MCU_RS422_EN_Pin, GPIO_PIN_SET);
            HAL_UART_Transmit(&hlpuart1, (uint8_t *)msg, (uint16_t)strlen(msg), PRINT_UART_TIMEOUT_MS);
            HAL_GPIO_WritePin(MCU_RS422_EN_GPIO_Port, MCU_RS422_EN_Pin, GPIO_PIN_RESET);
        }
    }
}
