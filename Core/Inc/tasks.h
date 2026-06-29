#ifndef TASKS_H
#define TASKS_H

#include <stdint.h>
#include <stdarg.h>

int queuedPrintf(const char *format, ...);

/***POWER***/
enum powerState{
    POWER_ON,
    POWER_OFF,
    POWER_ANOMALOUS,
};

#define POWER_GOOD_TIMEOUT_MS   5000U
#define WATCHDOG_SERVICE_INTERVAL_MS 500U


/***PRINTER***/
#define PRINT_QUEUE_ITEM_SIZE   128U
#define PRINT_UART_TIMEOUT_MS   1000U
int queuedPrintf(const char *format, ...);


#endif //TASKS_H
