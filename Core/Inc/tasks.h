#ifndef TASKS_H
#define TASKS_H

#include <stdint.h>
#include <stdarg.h>
#include "INA230.h"

enum powerState{
    POWER_ON,
    POWER_OFF,
    POWER_ANOMALOUS,
};

#define POWER_GOOD_TIMEOUT_MS   100U
#define WATCHDOG_SERVICE_INTERVAL_MS 1500U

/***POWER SENSORS***/
#define POWER_LOG_INTERVAL_MS 1000U

/***TEMPERATURE_SENSORS***/
#define TEMP_LOG_INTERVAL_MS 1000U
#define POWER_LOG_MUTEX_TIMEOUT 10U

#define INA230_LIST(X)        \
    X(INA230_1V2_3A)          \
    X(INA230_1V35_0A4)        \
    X(INA230_3V3_10A)         \
    X(INA230_0V6_1A)          \
    X(INA230_2V5_1A)          \
    X(INA230_1V8_2A)          \
    X(INA230_12V_3A)

struct INA230_readout {
    int16_t current;
    uint16_t power;
    int16_t voltageShunt;
    int16_t voltageBus;
    uint32_t timestamp;
};

typedef struct {
    #define INA230_READOUT_FIELD(name) struct INA230_readout name##_readout;
    	INA230_LIST(INA230_READOUT_FIELD)
	#undef INA230_READOUT_FIELD
} PowerLogData_t;

extern PowerLogData_t powerLogData;

//define common values to be shared by all the sensors, but allows individual sensors to be overwritten.
#define INA230_COMMON_SHUNT_ADC_CT        INA230_SHUNT_ADC_CT_588
#define INA230_COMMON_VBUS_ADC_CT         INA230_VBUS_ADC_CT_588
#define INA230_COMMON_AVG                 INA230_AVG_4
#define INA230_COMMON_CURRENT_LSB         (uint16_t)(1000)

#define INA230_1V2_3A_address               (uint16_t)(0b1000111)
#define INA230_1V2_3A_shuntResistor_mohm    (uint16_t)(2)
#define INA230_1V2_3A_current_LSB           (INA230_COMMON_CURRENT_LSB)
#define INA230_1V2_3A_shuntConvertTime      (INA230_COMMON_SHUNT_ADC_CT)
#define INA230_1V2_3A_busConvertTime        (INA230_COMMON_VBUS_ADC_CT)
#define INA230_1V2_3A_averagingMode         (INA230_COMMON_AVG)

#define INA230_1V35_0A4_address             (uint16_t)(0b1000010)
#define INA230_1V35_0A4_shuntResistor_mohm  (uint16_t)(4)
#define INA230_1V35_0A4_current_LSB         (INA230_COMMON_CURRENT_LSB)
#define INA230_1V35_0A4_shuntConvertTime    (INA230_COMMON_SHUNT_ADC_CT)
#define INA230_1V35_0A4_busConvertTime      (INA230_COMMON_VBUS_ADC_CT)
#define INA230_1V35_0A4_averagingMode       (INA230_COMMON_AVG)

#define INA230_3V3_10A_address              (uint16_t)(0b1001000)
#define INA230_3V3_10A_shuntResistor_mohm   (uint16_t)(1)
#define INA230_3V3_10A_current_LSB          (INA230_COMMON_CURRENT_LSB)
#define INA230_3V3_10A_shuntConvertTime     (INA230_COMMON_SHUNT_ADC_CT)
#define INA230_3V3_10A_busConvertTime       (INA230_COMMON_VBUS_ADC_CT)
#define INA230_3V3_10A_averagingMode        (INA230_COMMON_AVG)

#define INA230_0V6_1A_address               (uint16_t)(0b1000110)
#define INA230_0V6_1A_shuntResistor_mohm    (uint16_t)(2)
#define INA230_0V6_1A_current_LSB           (INA230_COMMON_CURRENT_LSB)
#define INA230_0V6_1A_shuntConvertTime      (INA230_COMMON_SHUNT_ADC_CT)
#define INA230_0V6_1A_busConvertTime        (INA230_COMMON_VBUS_ADC_CT)
#define INA230_0V6_1A_averagingMode         (INA230_COMMON_AVG)

#define INA230_2V5_1A_address               (uint16_t)(0b1000000)
#define INA230_2V5_1A_shuntResistor_mohm    (uint16_t)(2)
#define INA230_2V5_1A_current_LSB           (INA230_COMMON_CURRENT_LSB)
#define INA230_2V5_1A_shuntConvertTime      (INA230_COMMON_SHUNT_ADC_CT)
#define INA230_2V5_1A_busConvertTime        (INA230_COMMON_VBUS_ADC_CT)
#define INA230_2V5_1A_averagingMode         (INA230_COMMON_AVG)

#define INA230_1V8_2A_address               (uint16_t)(0b1000001)
#define INA230_1V8_2A_shuntResistor_mohm    (uint16_t)(2)
#define INA230_1V8_2A_current_LSB           (INA230_COMMON_CURRENT_LSB)
#define INA230_1V8_2A_shuntConvertTime      (INA230_COMMON_SHUNT_ADC_CT)
#define INA230_1V8_2A_busConvertTime        (INA230_COMMON_VBUS_ADC_CT)
#define INA230_1V8_2A_averagingMode         (INA230_COMMON_AVG)

#define INA230_12V_3A_address               (uint16_t)(0b1001010)
#define INA230_12V_3A_shuntResistor_mohm    (uint16_t)(2)
#define INA230_12V_3A_current_LSB           (INA230_COMMON_CURRENT_LSB)
#define INA230_12V_3A_shuntConvertTime      (INA230_COMMON_SHUNT_ADC_CT)
#define INA230_12V_3A_busConvertTime        (INA230_COMMON_VBUS_ADC_CT)
#define INA230_12V_3A_averagingMode         (INA230_COMMON_AVG)

extern PowerLogData_t powerLogData;

/***PRINTER***/
#define PRINT_QUEUE_ITEM_SIZE   128U
#define PRINT_UART_TIMEOUT_MS   1000U
void printer(void *argument);
int queuedPrintf(const char *format, ...);


#endif //TASKS_H
