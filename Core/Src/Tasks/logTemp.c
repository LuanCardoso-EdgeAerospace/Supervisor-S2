#include "cmsis_os2.h"

void logTemp(void *argument){
    for(;;)
    {
        osDelay(osWaitForever);
    }
}