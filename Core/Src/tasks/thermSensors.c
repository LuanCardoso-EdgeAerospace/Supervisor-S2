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
        queuedPrintf("TMP468 write failed: addr=0x%02X reg=0x%02X data=0x%04X stat=%d\r\n", addr>>1, reg, pData, stat);
    }
}

uint16_t tmp468read(I2C_BusId_t bus, uint16_t addr, uint8_t reg) {
    addr <<= 1;
    uint8_t data[2] = {0};
    HAL_StatusTypeDef stat = I2C_Mem_Read(bus, addr, reg, I2C_MEMADD_SIZE_8BIT, data, 2, 1000);
    if (stat != HAL_OK) {
        queuedPrintf("TMP468 read failed: addr=0x%02X reg=0x%02X stat=%d\r\n", addr>>1, reg, stat);

        return 0;
    }

    uint16_t res = (data[0] << 8) | (data[1]);
    return res;
}


static int sensorsSet=0;

static void setUpThermSensors(){
    static int configDelay = 2000;
    /* Instantiate and initialize every TMP468 in TMP468_LIST. */
#define TMP468_INITIALIZE(name, addr, i2cbus, convrate, hyst, therm1, therm2, cmask) \
    do { \
        name = TMP468_init( \
            (i2cbus), \
            (addr), \
            (convrate), \
            (hyst), \
            (therm1), \
            (therm2), \
            (uint8_t)(((cmask) & TMP468_REMOTE_bitmask) >> 8), \
            tmp468write, \
            tmp468read); \
            \
        queuedPrintf(#name" probed %d \r\n", name.enabled);\
    } while (0);

    TMP468_LIST(TMP468_INITIALIZE)
#undef TMP468_INITIALIZE

    /* Channel zero is local and was configured by TMP468_init(). */
#define TMP468_CONFIGURE_CHANNEL(parent, channel, therm1, therm2) \
    do { \
        if ((channel) > 0) { \
            TMP468_configRemote( \
                &(parent), \
                (channel), \
                0, /*TODO: temperature offset*/ \
                0, /*TODO: ideality factor*/ \
                (int32_t)(therm1), \
                (int32_t)(therm2)); \
        } \
    } while (0);
    thermSENSORS_a_LIST(TMP468_CONFIGURE_CHANNEL)
    thermSENSORS_b_LIST(TMP468_CONFIGURE_CHANNEL)
    thermSENSORS_c_LIST(TMP468_CONFIGURE_CHANNEL)
    thermSENSORS_p_LIST(TMP468_CONFIGURE_CHANNEL)
#undef TMP468_CONFIGURE_CHANNEL

    /* OPEN flags become valid after a conversion of the enabled channels. */
    osDelay(configDelay);

#define TMP468_CHECK_CHANNELS(name, addr, i2cbus, convrate, hyst, therm1, therm2, cmask) \
    do { \
        if (name.enabled == 0) break; \
        uint16_t expected = (uint16_t)((cmask) & TMP468_REMOTE_bitmask); \
        uint16_t enabled = TMP468_getRemoteEnabled(&(name)); \
        uint16_t openStatus = TMP468_getOpenStatus(&(name)); \
        uint16_t missing = (uint16_t)(expected & openStatus); \
        if (enabled != expected) { \
            queuedPrintf( \
                "%s remote-enable mismatch: expected=0x%04X actual=0x%04X\r\n", \
                #name, \
                (unsigned int)expected, \
                (unsigned int)enabled); \
        } \
        if (missing != 0U) { \
            queuedPrintf( \
                "%s open expected channel(s): expected=0x%04X open=0x%04X missing=0x%04X\r\n", \
                #name, \
                (unsigned int)expected, \
                (unsigned int)openStatus, \
                (unsigned int)missing); \
        } \
    } while (0);
    TMP468_LIST(TMP468_CHECK_CHANNELS)
#undef TMP468_CHECK_CHANNELS

    sensorsSet = 1;
}


// CMSIS RTOS task
void logTherm(void *argument) {
	(void)argument;
     testTherm();
    setUpThermSensors();

    uint32_t tick = osKernelGetTickCount();
    for (;;) {

        thermLogData_t latest = {0};

        //TODO: Be more clever here in case there are holes in the channel array.
       #define TMP468_READ_CHANNEL(parent, channel, therm1, therm2) \
       do{ \
            if(parent.enabled == 0) break; \
            latest.parent[channel] = TMP468_getRemoteTemperatureRAW(&parent, channel); \
       }while(0);

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
       else{
    	   queuedPrintf("TMP468Log: Mutex timedout:Continuing	 \r\n");
       }

        osDelayUntil(tick + THERM_LOG_INTERVAL_MS); // Preferable so it keeps the task aligned on the 1 second boundary.
        tick += THERM_LOG_INTERVAL_MS;
    }
}

#define TMP468_RAW_TO_MILLIC(raw_) \
    (((int32_t)(raw_) * 128) / 16)

void printTherm(void *argument) {
	(void)argument;
    // Send to terminal the current temperature
    int printDelay = 5000;
    thermLogData_t log = {0};

    for (;;) {

    	while (sensorsSet != 1) {
            osDelay(printDelay);
            queuedPrintf("Therm sensors not initialized, sleeping...\r\n");
        }

//        if (osMutexAcquire(thermLogMutexHandle, THERM_LOG_MUTEX_TIMEOUT) == osOK) {
            log = thermLogData;
//            osMutexRelease(powerLogMutexHandle);
//
//        } else {
//        	queuedPrintf("TMP468Print: Mutex timedout:sleeping \r\n");
//        	osDelay(printDelay);
//            continue;
//        }

        queuedPrintf("Thermal log timestamp: %lu ms\r\n", (unsigned long)log.timestamp);

        //todo: Be clever'er here.
        queuedPrintf("TMP468a [%ld, %ld, %ld, %ld, %ld] mdegC\r\n",
                     (long)TMP468_RAW_TO_MILLIC(log.TMP468a[0]),
                     (long)TMP468_RAW_TO_MILLIC(log.TMP468a[1]),
                     (long)TMP468_RAW_TO_MILLIC(log.TMP468a[2]),
                     (long)TMP468_RAW_TO_MILLIC(log.TMP468a[3]),
                     (long)TMP468_RAW_TO_MILLIC(log.TMP468a[4]));
        
        queuedPrintf("TMP468b [%ld, %ld, %ld, %ld, %ld] mdegC\r\n",
                     (long)TMP468_RAW_TO_MILLIC(log.TMP468b[0]),
                     (long)TMP468_RAW_TO_MILLIC(log.TMP468b[1]),
                     (long)TMP468_RAW_TO_MILLIC(log.TMP468b[2]),
                     (long)TMP468_RAW_TO_MILLIC(log.TMP468b[3]),
                     (long)TMP468_RAW_TO_MILLIC(log.TMP468b[4]));
        
        queuedPrintf("TMP468c [%ld, %ld, %ld, %ld, %ld] mdegC\r\n",
                     (long)TMP468_RAW_TO_MILLIC(log.TMP468c[0]),
                     (long)TMP468_RAW_TO_MILLIC(log.TMP468c[1]),
                     (long)TMP468_RAW_TO_MILLIC(log.TMP468c[2]),
                     (long)TMP468_RAW_TO_MILLIC(log.TMP468c[3]),
                     (long)TMP468_RAW_TO_MILLIC(log.TMP468c[4]));

        queuedPrintf("TMP468p [%ld, %ld] mdegC\r\n",
                     (long)TMP468_RAW_TO_MILLIC(log.TMP468p[0]),
                     (long)TMP468_RAW_TO_MILLIC(log.TMP468p[1]));


        osDelay(printDelay);
    }
}

void testTherm(){
	//init a sensor and test the features.
	queuedPrintf("TMP468A test\r\n");

    TMP468_t tmpa =  TMP468_init(I2C_BUS_2, 0x48, TMP468_DEFAULT_CONVERSION_RATE, 0,
                              0, 0, (0b1111000000000000)>>8, 
                              tmp468write,
                              tmp468read );

    uint16_t manid = tmpa.i2c_read(tmpa.bus, tmpa.addr, TMP468_REG_MANID);
    uint16_t devid = tmpa.i2c_read(tmpa.bus, tmpa.addr, TMP468_REG_DEVID);

	queuedPrintf("TMP468A manid %x devid %x \r\n", manid, devid);
	return;
}



