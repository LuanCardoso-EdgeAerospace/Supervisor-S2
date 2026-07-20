#include "cmsis_os2.h"
#include "INA230.h"
#include "main.h"
#include "tasks.h"
#include "powerSensors.h"
#include "app_freertos.h"
#include "i2c_manager.h"

PowerLogData_t powerLogData;

//sensors
#define INA230_DECLARE(name, a, b, c, d, e, f, g) INA230_t name;
INA230_LIST(INA230_DECLARE)
#undef INA230_DECLARE


void ina230write(I2C_BusId_t bus, uint16_t addr, uint8_t reg, uint16_t pData) {
    addr <<= 1;
    uint8_t data[2] = {0};
    data[0] = pData >> 8;
    data[1] = pData & 0xff;

    HAL_StatusTypeDef stat = I2C_Mem_Write(bus, addr, reg, I2C_MEMADD_SIZE_8BIT, data, 2, 1000);
    if (stat != HAL_OK) {
        queuedPrintf("INA230 write failed: addr=0x%02X reg=0x%02X data=0x%04X stat=%d\r\n", addr, reg, pData, stat);
    }
}

uint16_t ina230read(I2C_BusId_t bus, uint16_t addr, uint8_t reg) {
    addr <<= 1;
    uint8_t data[2] = {0};
    HAL_StatusTypeDef stat = I2C_Mem_Read(bus, addr, reg, I2C_MEMADD_SIZE_8BIT, data, 2, 1000);
    if (stat != HAL_OK) {
        queuedPrintf("INA230 read failed: addr=0x%02X reg=0x%02X stat=%d\r\n", addr, reg, stat);

        return 0;
    }

    uint16_t res = (data[0] << 8) | (data[1]);
    return res;
}

#define INA230_SETUP_SENSOR(name,                                     \
                            addr,                                     \
                            shunt,                                    \
                            bus,                                      \
                            clsb,                                     \
                            shct,                                     \
                            bct,                                      \
                            avgm)                                     \
    do {                                                              \
        name = INA230_init(bus,                                      \
                           addr,                                      \
                           clsb,                                      \
                           shunt,                                     \
                           shct,                                      \
                           bct,                                       \
                           avgm,                                      \
						   ina230write,								  \
						   ina230read);								  \
		INA230_start(name, INA230_MODE_CONTINOUS_ALL);                \
    } while (0);


int sensorsSet=0;
void setUpPowerSensors(void){

	if (!sensorsSet){
	INA230_LIST(INA230_SETUP_SENSOR)
	#undef INA230_SETUP_SENSOR


	//TODO: Configure Alarms
	sensorsSet=1;
	}
}

extern enum powerState powerStatus;


//CMSIS RTOS task
void logPower(void *argument){
    (void)argument;

    //todo: Check if board is powered up before INA setups
    for(;;){
        if (powerStatus == POWER_ON) break;
        osDelay(1000);
    }
//    testINA();
    setUpPowerSensors();

	uint32_t tick = osKernelGetTickCount();
    for(;;)
    {
    	PowerLogData_t latest = {0};
    	//Read data (this task owns the I2C by design, so preempting while reading is no problem.
		#define INA230_UPDATE_READOUT(name, a, b, c, d, e, f, g)                 \
			do {                                                                 \
				latest.name##_readout.current = INA230_getCurrent(name);         \
				latest.name##_readout.power = INA230_getPower(name);             \
				latest.name##_readout.voltageBus = INA230_getVoltageBus(name);   \
				latest.name##_readout.voltageShunt = INA230_getVoltageShunt(name); \
				latest.name##_readout.timestamp = osKernelGetTickCount();        \
			} while (0);

			INA230_LIST(INA230_UPDATE_READOUT)

		#undef INA230_UPDATE_READOUT

        if (osMutexAcquire(powerLogMutexHandle, POWER_LOG_MUTEX_TIMEOUT) == osOK) {
            powerLogData = latest;
            osMutexRelease(powerLogMutexHandle);
        }

        osDelayUntil(tick + POWER_LOG_INTERVAL_MS); //Preferable so it keeps the task aligned on the 1 second boundary.
        tick += POWER_LOG_INTERVAL_MS;
    }
}


void printPower(void *argument){
	(void)argument;
    int printDelay;
    PowerLogData_t log = {0};

    for(;;){

    	while(sensorsSet != 1){
    		osDelay(1000);
    		queuedPrintf("Power sensors not initialized, sleeping...\r\n");
    	}

        if (osMutexAcquire(powerLogMutexHandle, POWER_LOG_MUTEX_TIMEOUT) == osOK) {
            log = powerLogData;
            osMutexRelease(powerLogMutexHandle);

        }else{
        	continue;
        }

        #define INA230_PRINT(name, a, b, c, d, e, f, g)                         \
            do {                                                                \
                queuedPrintf(                                                   \
                    "%16s: Vbus=%5u mV, Vshunt=%5d uV, I=%4d mA, P=%4u mW, ts[%lu]\r\n",   \
					#name,                                                      \
                    log.name##_readout.voltageBus,                              \
                    log.name##_readout.voltageShunt,                            \
                    log.name##_readout.current,                                 \
                    log.name##_readout.power,                                   \
                    log.name##_readout.timestamp                                \
                );                                                              \
            } while (0);

        INA230_LIST(INA230_PRINT);
        #undef INA230_PRINT
        queuedPrintf("\r\n");

        osDelay(printDelay=1000);
    }
}

//TODO: Tie alarms to interrups
//TODO: Create an emergency shutdown task, called by the interrupt.


void testINA(void){
    //Call this function before the freeRTOS goes up and debug with watchpoits
    #define INA230_DECLARE(name, a, b, c, d, e, f, g) INA230_t name;

    INA230_T_LIST(INA230_DECLARE)

	#undef INA230_DECLARE


    #define INA230_SETUP_SENSOR(name,                                 \
                            addr,                                     \
                            shunt,                                    \
                            bus,                                      \
                            clsb,                                     \
                            shct,                                     \
                            bct,                                      \
                            avgm)                                     \
    do {                                                              \
        name = INA230_init(bus,                                       \
                           addr,                                      \
                           clsb,                                      \
                           shunt,                                     \
                           shct,                                      \
                           bct,                                       \
                           avgm,                                      \
						   ina230write,								  \
						   ina230read);								  \
		INA230_start(name, INA230_MODE_CONTINOUS_ALL);                \
    } while (0);

    INA230_T_LIST(INA230_SETUP_SENSOR)

	#undef INA230_SETUP_SENSOR

	extern I2C_HandleTypeDef hi2c3;

    volatile uint16_t id = INA230_getID(INA230_3V3_10A);

    uint8_t data[2] = {0xFF, 0xFF};
    HAL_StatusTypeDef ret= HAL_I2C_Mem_Read(&hi2c3, INA230_3V3_10A.address << 1, INA230_REG_ID, I2C_MEMADD_SIZE_8BIT, data, 2, 1000);

    uint32_t err = HAL_I2C_GetError(&hi2c3);

    queuedPrintf("INA230T read ret=%d err=0x%08lx data=%02x %02x\r\n",
                 ret, err, data[0], data[1]);


//    for(;;) continue;
    
}


