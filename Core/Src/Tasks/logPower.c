#include "cmsis_os2.h"
#include "INA230.h"
#include "main.h"
#include "tasks.h"
#include "app_freertos.h"

PowerLogData_t powerLogData;

//sensors
#define INA230_DECLARE(name) INA230_t name;
INA230_LIST(INA230_DECLARE)
#undef INA230_DECLARE

extern I2C_HandleTypeDef hi2c3;
void ina230write(uint16_t addr, uint8_t reg, uint16_t pData){
  addr <<=1;
  uint8_t data[2]={0};
  data[0]=pData >> 8;
  data[1]=pData & 0xff;

  HAL_I2C_Mem_Write(&hi2c3, addr, reg, 1, data, 2, 1000);
}

uint16_t ina230read(uint16_t addr, uint8_t reg){
  addr <<=1;
  uint8_t data[2]={0};
  HAL_I2C_Mem_Read(&hi2c3, addr, reg, 1, data, 2, 1000);
  uint16_t res = (data[0]<<8)|(data[1]);
  return res;
}

#define INA230_SETUP_SENSOR(name)                                      \
    do {                                                              \
        name = INA230_init(name##_address,                            \
                           name##_current_LSB,                        \
                           name##_shuntResistor_mohm,                 \
                           name##_shuntConvertTime,                   \
                           name##_busConvertTime,                     \
                           name##_averagingMode,                      \
						   ina230write,								  \
						   ina230read);								  \
		INA230_start(name, INA230_MODE_CONTINOUS_ALL);                \
    } while (0);


void setUpPowerSensors(void){
	INA230_LIST(INA230_SETUP_SENSOR)
#undef INA230_SETUP_SENSOR
	//TODO: Configure Alarms
}

//CMSIS RTOS task
void logPower(void *argument){

    setUpPowerSensors();

	uint32_t tick = osKernelGetTickCount();
    for(;;)
    {
    	PowerLogData_t latest = {0};
    	//Read data (this task owns the I2C by design, so preempting while reading is no problem.
		#define INA230_UPDATE_READOUT(name)                                      \
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


//TODO: Tie alarms to interrups
//TODO: Create an emergency shutdown task, called by the interrupt.



