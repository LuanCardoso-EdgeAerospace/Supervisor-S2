//#include "cmsis_os2.h"
//#include "TMP468.h"
//#include "main.h"
//#include "tasks.h"
//#include "thermSensors.h"
//#include "app_freertos.h"
//
///* --- Internal state --- */
//thermLogData_t thermLogData;
//
//
//static int sensorsSet=0;
//setUpThermSensors(){
//    //Init sensors
//
//    //check if TMP468_getOpenStatus() matches expectedChannelMask
//
//    //configure each of the remote channels using TMP468_configRemote
//    sensorsSet=1;
//}
//
////CMSIS RTOS task
//void logTherm(void *argument){
//    setUpThermSensors();
//
//    uint32_t tick = osKernelGetTickCount();
//    for(;;){
//
//
//
//        osDelayUntil(tick + THERM_LOG_INTERVAL_MS); //Preferable so it keeps the task aligned on the 1 second boundary.
//        tick += THERM_LOG_INTERVAL_MS;
//    }
//
//}
//
//
//
//void printPower(void *argument){
//    //Send to terminal the current temperature
//    int printDelay;
//    thermLogData_t log = {0};
//
//    for(;;){
//
//    	while(sensorsSet != 1){
//    		osDelay(1000);
//    		queuedPrintf("Therm sensors not initialized, sleeping...\r\n");
//    	}
//
//        if (osMutexAcquire(thermLogMutexHandle, THERM_LOG_MUTEX_TIMEOUT) == osOK) {
//            log = thermLogData;
//            osMutexRelease(powerLogMutexHandle);
//
//        }else{
//        	continue;
//        }
//
//        //TODO print
//
//        queuedPrintf("\r\n");
//
//        osDelay(printDelay=1000);
//    }
//}
