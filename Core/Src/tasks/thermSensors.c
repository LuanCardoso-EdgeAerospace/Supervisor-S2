#include "cmsis_os2.h"
#include "TMP468.h"
#include "main.h"
#include "tasks.h"
#include "thermSensors.h"
#include "app_freertos.h"

/* --- Internal state --- */
thermLogData_t thermLogData;

//sensors
#define TMP468_DECLARE(name, addr, i2cbus, convrate, hyst, therm1, therm2, cmask) TMP468_t name;
TMP468_LIST(TMP468_DECLARE)
#undef TMP468_DECLARE

void tmp468write(I2C_BusId_t bus, uint16_t addr, uint8_t reg, uint16_t pData) {
    addr <<= 1;
    uint8_t data[2] = {0};
    data[0] = pData >> 8;
    data[1] = pData & 0xff;

    HAL_StatusTypeDef stat = I2C_Mem_Write(bus, addr, reg, I2C_MEMADD_SIZE_8BIT, data, 2, 1000);
    if (stat != HAL_OK) {
        queuedPrintf("TMP468 write failed: addr=0x%02X reg=0x%02X data=0x%04X stat=%d\r\n", addr, reg, pData, stat);
    }
}

uint16_t tmp468read(I2C_BusId_t bus, uint16_t addr, uint8_t reg) {
    addr <<= 1;
    uint8_t data[2] = {0};
    HAL_StatusTypeDef stat = I2C_Mem_Read(bus, addr, reg, I2C_MEMADD_SIZE_8BIT, data, 2, 1000);
    if (stat != HAL_OK) {
        queuedPrintf("TMP468 read failed: addr=0x%02X reg=0x%02X stat=%d\r\n", addr, reg, stat);

        return 0;
    }

    uint16_t res = (data[0] << 8) | (data[1]);
    return res;
}


static int sensorsSet=0;
setUpThermSensors(){
	//Init sensors

	//check if TMP468_getOpenStatus() matches expectedChannelMask

	//configure each of the remote channels using TMP468_configRemote

	// At the end, set global variable.
	//   sensorsSet=1;
}


// CMSIS RTOS task
void logTherm(void *argument) {
    setUpThermSensors();
    testTherm();

    uint32_t tick = osKernelGetTickCount();
    for (;;) {

        thermLogData_t latest;

        //TODO: Be more clever here in case there are holes in the channel array.
       #define TMP468_READ_CHANNEL(parent, channel, therm1, therm2) \
           latest.parent[channel] = TMP468_getRemoteTemperatureRAW(parent, channel);

       thermSENSORS_a_LIST(TMP468_READ_CHANNEL);
       thermSENSORS_b_LIST(TMP468_READ_CHANNEL);
       thermSENSORS_c_LIST(TMP468_READ_CHANNEL);
       thermSENSORS_p_LIST(TMP468_READ_CHANNEL);

       #undef TMP468_READ_CHANNEL

       latest.timestamp = osKernelGetTickCount();

       if (osMutexAcquire(thermLogMutexHandle, THERM_LOG_MUTEX_TIMEOUT) == osOK) {
           thermLogData = latest;
           osMutexRelease(thermLogMutexHandle);
       }

        osDelayUntil(tick + THERM_LOG_INTERVAL_MS); // Preferable so it keeps the task aligned on the 1 second boundary.
        tick += THERM_LOG_INTERVAL_MS;
    }
}

#define TMP468_RAW_TO_MILLIC(raw_) \
    (((int32_t)(raw_) * 1000) / 16)

void printTherm(void *argument) {
    // Send to terminal the current temperature
    int printDelay = 5000;
    thermLogData_t log = {0};

    for (;;) {

        while (sensorsSet != 1) {
            osDelay(printDelay);
            queuedPrintf("Therm sensors not initialized, sleeping...\r\n");
        }

        if (osMutexAcquire(thermLogMutexHandle, THERM_LOG_MUTEX_TIMEOUT) == osOK) {
            log = thermLogData;
            osMutexRelease(powerLogMutexHandle);

        } else {
            continue;
        }

        queuedPrintf("Thermal log timestamp: %lu ms\r\n", (unsigned long)log.timestamp);

        //todo: Be clever'er here.
        queuedPrintf("TMP468a [%ld, %ld, %ld, %ld, %ld] mdegC\r\n",
                     (long)TMP468_RAW_TO_MILLIC(log.TMP468a[0]),
                     (long)TMP468_RAW_TO_MILLIC(log.TMP468a[1]),
                     (long)TMP468_RAW_TO_MILLIC(log.TMP468a[2]),
                     (long)TMP468_RAW_TO_MILLIC(log.TMP468a[3]),
                     (long)TMP468_RAW_TO_MILLIC(log.TMP468a[4]));
        
        queuedPrintf("TMP468b [%ld, %ld, %ld, %ld, %ld] mdegC\r\n",
                     (long)TMP468_RAW_TO_MILLIC(log.TMP468a[0]),
                     (long)TMP468_RAW_TO_MILLIC(log.TMP468a[1]),
                     (long)TMP468_RAW_TO_MILLIC(log.TMP468a[2]),
                     (long)TMP468_RAW_TO_MILLIC(log.TMP468a[3]),
                     (long)TMP468_RAW_TO_MILLIC(log.TMP468a[4]));
        
        queuedPrintf("TMP468c [%ld, %ld, %ld, %ld, %ld] mdegC\r\n",
                     (long)TMP468_RAW_TO_MILLIC(log.TMP468a[0]),
                     (long)TMP468_RAW_TO_MILLIC(log.TMP468a[1]),
                     (long)TMP468_RAW_TO_MILLIC(log.TMP468a[2]),
                     (long)TMP468_RAW_TO_MILLIC(log.TMP468a[3]),
                     (long)TMP468_RAW_TO_MILLIC(log.TMP468a[4]));

        queuedPrintf("TMP468p [%ld, %ld, %ld, %ld, %ld] mdegC\r\n",
                     (long)TMP468_RAW_TO_MILLIC(log.TMP468a[0]),
                     (long)TMP468_RAW_TO_MILLIC(log.TMP468a[1]));


        osDelay(printDelay);
    }
}

void testTherm(){
	//init a sensor and test the features.
	queuedPrintf("TMP468p test\r\n");

	if (!sensorsSet){
		queuedPrintf("TMP468p Sensor not initialized");
	}
	return;
}



