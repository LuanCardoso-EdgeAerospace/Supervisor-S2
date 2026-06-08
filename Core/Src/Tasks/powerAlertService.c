#include "cmsis_os2.h"

void powerAlertService(void *argument){
    for(;;)
    {
        osDelay(osWaitForever);
    }
}