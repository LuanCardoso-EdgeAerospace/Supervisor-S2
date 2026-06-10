#ifndef POWERSENSORS_H
#define POWERSENSORS_H

#include <stdint.h>

/***POWER SENSORS***/
#define POWER_LOG_INTERVAL_MS 1000U
#define POWER_LOG_MUTEX_TIMEOUT 10U

//define common values to be shared by all the sensors, but allows individual sensors to be overwritten.
#define INA230_COMMON_SHUNT_ADC_CT        INA230_SHUNT_ADC_CT_588
#define INA230_COMMON_VBUS_ADC_CT         INA230_VBUS_ADC_CT_588
#define INA230_COMMON_AVG                 INA230_AVG_4
#define INA230_COMMON_CURRENT_LSB         (uint16_t)(1000)


//name, address, shuntResistor(mOhm), i2cbus, currentLSB, ShuntConvertTime, BusConvertTime, AveragingMode,
#define INA230_LIST(X)        \
    X(INA230_1V2_3A,   (0b1000111), 2, hi2c3, INA230_COMMON_CURRENT_LSB, INA230_COMMON_SHUNT_ADC_CT, INA230_COMMON_VBUS_ADC_CT, INA230_COMMON_AVG)          \
    X(INA230_1V35_0A4, (0b1000010), 4, hi2c3, INA230_COMMON_CURRENT_LSB, INA230_COMMON_SHUNT_ADC_CT, INA230_COMMON_VBUS_ADC_CT, INA230_COMMON_AVG)        \
    X(INA230_3V3_10A,  (0b1001000), 1, hi2c3, INA230_COMMON_CURRENT_LSB, INA230_COMMON_SHUNT_ADC_CT, INA230_COMMON_VBUS_ADC_CT, INA230_COMMON_AVG)         \
    X(INA230_0V6_1A,   (0b1000110), 2, hi2c3, INA230_COMMON_CURRENT_LSB, INA230_COMMON_SHUNT_ADC_CT, INA230_COMMON_VBUS_ADC_CT, INA230_COMMON_AVG)          \
    X(INA230_2V5_1A,   (0b1000000), 2, hi2c3, INA230_COMMON_CURRENT_LSB, INA230_COMMON_SHUNT_ADC_CT, INA230_COMMON_VBUS_ADC_CT, INA230_COMMON_AVG)          \
    X(INA230_1V8_2A,   (0b1000001), 2, hi2c3, INA230_COMMON_CURRENT_LSB, INA230_COMMON_SHUNT_ADC_CT, INA230_COMMON_VBUS_ADC_CT, INA230_COMMON_AVG)          \
    X(INA230_12V_3A,   (0b1001010), 2, hi2c2, INA230_COMMON_CURRENT_LSB, INA230_COMMON_SHUNT_ADC_CT, INA230_COMMON_VBUS_ADC_CT, INA230_COMMON_AVG)

struct INA230_readout {
    int16_t current;
    uint16_t power;
    int16_t voltageShunt;
    int16_t voltageBus;
    uint32_t timestamp;
};

typedef struct {
    #define INA230_READOUT_FIELD(name, a, b, c, d, e, f, g) struct INA230_readout name##_readout;
    	INA230_LIST(INA230_READOUT_FIELD)
	#undef INA230_READOUT_FIELD
} PowerLogData_t;

extern PowerLogData_t powerLogData;


#endif
