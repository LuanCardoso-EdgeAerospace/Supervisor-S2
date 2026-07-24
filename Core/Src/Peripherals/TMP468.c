/*
 *  TMP468.h
 *
 *  Created on: Jan 6, 2026
 *      Author: Luan
 */

#include "TMP468.h"
#include "main.h"

/**
 * @brief Converts the register-scale temperature into milli Celsius
 *
 * @param reg
 * @return int32_t
 */
int32_t regToMillis(int16_t reg) {
    // Using division here since bitwise shift with signal extend is not
    // a guaranteed behavior of C.
    /* Register bit 3 is 0.0625 C, so a full register count is 1/128 C. */
    return ((int32_t)reg * 125L) / 16L;
}

/**
 * @brief transform a temperature in milidegrees into the format expecified by the register.
 * Since the actual resolution of the device is TMP468_TEMP_RESOLUTION per bit, for data storage
 * it is prefered to either convert to degrees, or into the register format, which fits in a 16 bit
 * signed.
 *
 * @param millis
 * @return int16_t
 */
int16_t millisToReg(int32_t millis) {
    int16_t reg = millis / 64; // truncate if necessary
    return reg << 3;
}

/**
 * @brief input value is degreeCelsius * 2 to keep resolution of register.
 * output can be directly written to THERM Limit register
 *
 * @param hlvcelsius temperature in half degree resolution in range
 * @return uint16_t bitfield on correct format for i2c write
 */
uint16_t halfDegreesToThermLimitreg(int hlvcelsius) { return hlvcelsius << TMP468_THERM_pos; }

/**
 * @brief returned value is degreeCelsius * 2 to keep resolution of register.
 *
 * @param thermlimit raw register contents of THERM limit
 * @return int Temperature in half-degrees
 */
int thermLimitRegtoHalfDegrees(uint16_t thermlimit) {
    int hlvcelsius = (thermlimit & TMP468_THERM_mask) >> TMP468_THERM_pos;
    return (thermlimit & TMP468_THERM_SIGNAL_mask) ? -hlvcelsius : hlvcelsius;
}

/********************************** Reading ***********************************/

/**
 * @brief Returns temperature of channel idx in thousandths of degrees
 *
 * @param target
 * @param idx Remote sensor channel. Zero if using local sensor
 * @return uint32_t
 */
int32_t TMP468_getRemoteTemperature(TMP468_t *target, int idx) {
    if ((target == NULL) || (target->i2c_read == NULL) || (idx < 0) || (idx > 8)) {
        return -1;
    }
    return regToMillis(target->i2c_read(target->bus, target->addr, TMP468_REG_LOCAL_TMP + idx));
    // This is only possible since the registers for the remote register are in adjacent address.
}

/**
 * @brief Returns temperature of channel idx in the raw register format (LSB= 0.0625 degrees C)
 *
 * @param target
 * @param idx Remote sensor channel. Zero if using local sensor
 * @return uint32_t
 */
int16_t TMP468_getRemoteTemperatureRAW(TMP468_t *target, int idx) {
    if ((target == NULL) || (target->i2c_read == NULL) || (idx < 0) || (idx > 8)) {
        return -1;
    }
    return target->i2c_read(target->bus, target->addr, TMP468_REG_LOCAL_TMP + idx);
    // This is only possible since the registers for the remote register are in adjacent address.
}

/**
 * @brief Returns status of the therm alerts
 *
 * Returns a bitfield with a bit set when a channel has exceeded the THERM1 limit.
 * Bit is reset when the temperature goes bellow THERM-Hysteresis.
 * Use the masks TMP468_REMOTE*_MASK and TMP468_LOCAL_MASK for checking each channel independently.
 *
 * Warning: This is the inverse of the alert pin on the IC, which goes to a logical low when the sensor
 * detects overtemperature.
 *
 * @param target
 * @return uint16_t
 */
uint16_t TMP468_getTHERM1Status(TMP468_t *target) {
    if ((target == NULL) || (target->i2c_read == NULL)) {
        return 0U;
    }
    return target->i2c_read(target->bus, target->addr, TMP468_REG_THERM1_STATUS);
}

/**
 * @brief Returns status of the therm alerts
 *
 * Returns a bitfield with a bit set when a channel has exceeded the THERM1 limit.
 * Bit is reset when the temperature goes bellow THERM-Hysteresis.
 * Use the masks TMP468_REMOTE*_MASK and TMP468_LOCAL_MASK for checking each channel independently.
 *
 * Warning: This is the inverse of the alert pin on the IC, which goes to a logical low when the sensor
 * detects overtemperature.
 *
 * @param target
 * @return uint16_t
 */
uint16_t TMP468_getTHERM2Status(TMP468_t *target) {
    if ((target == NULL) || (target->i2c_read == NULL)) {
        return 0U;
    }
    return target->i2c_read(target->bus, target->addr, TMP468_REG_THERM2_STATUS);
}

/**
 * @brief Check for connection state on the remote channels.
 *
 * Returns a bitfield with a bit set when an open circuit is detected. Use the masks
 * TMP468_REMOTE*_MASK and TMP468_LOCAL_MASK for checking each channel independently.
 * Updates target.detectedChannels the remotes, drops local.
 * Bit 1 signifies a an open-circuit condition
 *
 * @param target
 * @return uint16_t 0 if all circuits are closed, mask otherwise.
 */
uint16_t TMP468_getOpenStatus(TMP468_t *target) {
    if ((target == NULL) || (target->i2c_read == NULL)) {
        return 0U;
    }

    uint16_t e = target->i2c_read(target->bus, target->addr, TMP468_REG_REMOTE_CHANNEL_OPEN_STATUS);
    target->detectedChannels = e >> 8;
    return e;
}

/**
 * @brief Busy status of the ADC in the TMP468
 *
 * @param target
 * @return bool
 */
int TMP468_isbusy(TMP468_t *target) {
    if ((target == NULL) || (target->i2c_read == NULL)) {
        return 0U;
    }
    return target->i2c_read(target->bus, target->addr, TMP468_REG_CONFIGURATION) & TMP468_CONFIG_BUSY;
}

/******************************* Configuration ********************************/

/**
 * @brief Initialization function for TMP468. Remote channels default to on, but are not configured.
 *
 * @param addr address in the i2c bus
 * @param conversionRate One of TMP468_CONVERSION_RATE_*
 * @param hysteresis Histeresis value in degrees
 * @param THERM1 Thermal alarm, in 0.5 degrees resolution
 * @param THERM2 Thermal alarm, in 0.5 degrees resolution
 * @return TMP468_t
 */
TMP468_t TMP468_init(I2C_BusId_t bus, uint16_t addr, uint16_t conversionRate, uint16_t hysteresis, uint32_t THERM1,
                     uint32_t THERM2, uint8_t enabledChannels, TMP468_I2C_WriteFn i2c_write,
                     TMP468_I2C_ReadFn i2c_read) {

    TMP468_t target = {.bus = bus,
                       .addr = addr,
                       .hysteresis = hysteresis,
                       .localTHERMLimit1 = THERM1,
                       .localTHERMLimit2 = THERM2,
                       .enabledChannels = enabledChannels,
                       .detectedChannels = 0,
                       .enabled = 1,
                       .i2c_write = i2c_write,
                       .i2c_read = i2c_read};

    if ((i2c_write == NULL) || (i2c_read == NULL)) {
        return target;
    }
    
    //probe devoce by first reading from ID register
    uint16_t manid = target.i2c_read(target.bus, target.addr, TMP468_REG_MANID);
    uint16_t devid = target.i2c_read(target.bus, target.addr, TMP468_REG_DEVID);
    if ( (manid != TMP468_MANID) ||
         (devid != TMP468_DEVID)  ){
            //probe failed
            target.i2c_read = NULL;
            target.i2c_write = NULL;
            target.enabled = 0;  
            return target;
    }         
         
    // TMP460 powers up on a locked state
    TMP468_unlock(&target);

    uint16_t cfg = target.i2c_read(target.bus, addr, TMP468_REG_CONFIGURATION);
    cfg &= ~TMP468_CONFIG_CONVERSION_RATE_mask; // clears bits
    cfg |= conversionRate;                      // set bits to conversion rate
    target.i2c_write(target.bus, addr, TMP468_REG_CONFIGURATION, cfg);

    TMP468_setHysteresis(&target, hysteresis);
    TMP468_setThermLimit(&target, 0, THERM1, 0);
    TMP468_setThermLimit(&target, 0, THERM2, 1);
    return target;
}

/**
 * @brief Enables the remote sensors.
 *
 * @param target
 * @param flags Multiple sensors can be comined using bitwise OR
 */
void TMP468_setRemoteEnabled(TMP468_t *target, uint16_t flags) {
    if ((target == NULL) || (target->i2c_read == NULL) || (target->i2c_write == NULL)) {
        return;
    }

    uint16_t cfg = target->i2c_read(target->bus, target->addr, TMP468_REG_CONFIGURATION);
    // clears bits
    cfg &= ~TMP468_REMOTE_bitmask;
    cfg |= flags & TMP468_REMOTE_bitmask;
    target->i2c_write(target->bus, target->addr, TMP468_REG_CONFIGURATION, cfg);
    target->enabledChannels = (uint8_t)(flags >> 8);
}

/**
 * @brief Return list of enabled remote sensors, as bit flags
 *
 * @param target
 * @return uint16_t
 */
uint16_t TMP468_getRemoteEnabled(TMP468_t *target) {
    if ((target == NULL) || (target->i2c_read == NULL) || (target->i2c_write == NULL)) {
        return;
    }

    uint16_t cfg = target->i2c_read(target->bus, target->addr, TMP468_REG_CONFIGURATION);
    cfg &= TMP468_REMOTE_bitmask;
    target->enabledChannels = (uint8_t)(cfg >> 8);
    return cfg;
}

/**
 * @brief Enable log on the local channel.
 *
 * @param target
 */
void TMP468_enableLocal(TMP468_t *target) {
    if ((target == NULL) || (target->i2c_read == NULL) || (target->i2c_write == NULL)) {
        return;
    }
    uint16_t cfg = target->i2c_read(target->bus, target->addr, TMP468_REG_CONFIGURATION);
    cfg |= TMP468_LOCAL_MASK; // sets bit to 1
    target->i2c_write(target->bus, target->addr, TMP468_REG_CONFIGURATION, cfg);
}

/**
 * @brief Disable log on the local channel.
 *
 * @param target
 */
void TMP468_disableLocal(TMP468_t *target) {
    if ((target == NULL) || (target->i2c_read == NULL) || (target->i2c_write == NULL)) {
        return;
    }
    uint16_t cfg = target->i2c_read(target->bus, target->addr, TMP468_REG_CONFIGURATION);
    cfg &= ~TMP468_LOCAL_MASK; // sets bit to 1
    target->i2c_write(target->bus, target->addr, TMP468_REG_CONFIGURATION, cfg);
}

/**
 * @brief Get value of Histeresis in degrees C
 *
 * @param target
 * @return int16_t
 */
uint16_t TMP468_getHysteresis(TMP468_t *target) {
    if ((target == NULL) || (target->i2c_read == NULL) || (target->i2c_write == NULL)) {
        return;
    }
    return target->i2c_read(target->bus, target->addr, TMP468_REG_THERM_HYSTERESIS) >> TMP468_HYSTERESIS_pos;
}

/**
 * @brief Sets hysteresis value in degrees C
 *
 * @param target
 * @param hysteresis Hysteresis temperature in degrees
 */
void TMP468_setHysteresis(TMP468_t *target, uint16_t hysteresis) {
    if ((target == NULL) || (target->i2c_read == NULL) || (target->i2c_write == NULL)) {
        return;
    }
    target->i2c_write(target->bus, target->addr, TMP468_REG_THERM_HYSTERESIS, hysteresis << TMP468_HYSTERESIS_pos);
}

/** internal function, allows one to refer to the internal register as idx0 */
int16_t thermRegisterOffset(int idx, int thermSelect) {
    if (idx < 0 || idx > 8 || thermSelect < 0 || thermSelect > 1) {
        return -1;
    }

    int16_t reg;
    if (idx == 0) {
        reg = TMP468_REG_LOCAL_THERM2_LIMIT + thermSelect;
    } else {
        reg = TMP468_REG_REMOTE_CONFIG_BASE_ADDR + (TMP468_REG_REMOTE_CONFIG_REMOTE_REMOTE_OFFSET * idx) +
              TMP468_REG_REMOTE_CONFIG_REMOTE_THERM1_OFFSET + thermSelect;
    }
    return reg;
}

/**
 * @brief Returns the value of the THERM limit
 *
 * @param target
 * @param idx Measurement channel, with 0 being the local channel.
 * @param thermSelect TMP468_SELECT_THERM1 or TMP468_SELECT_THERM2
 * @return int16_t
 */
int16_t TMP468_getThermlimit(TMP468_t *target, int idx, int thermSelect) {
    if ((target == NULL) || (target->i2c_read == NULL) || (target->i2c_write == NULL)) {
        return;
    }
    int16_t reg = thermRegisterOffset(idx, thermSelect);
    if (reg == -1) {
        return -1;
    } else {
        return target->i2c_read(target->bus, target->addr, reg);
    }
}

/**
 * @brief Sets the value of the THERM limit
 *
 * @param target
 * @param idx Measurement channel, with 0 being the local channel.
 * @param thermlimit
 * @param thermSelect TMP468_SELECT_THERM1 or TMP468_SELECT_THERM2
 */
void TMP468_setThermLimit(TMP468_t *target, int idx, int16_t thermlimit, int thermSelect) {
    if ((target == NULL) || (target->i2c_read == NULL) || (target->i2c_write == NULL)) {
        return;
    }
    int16_t reg = thermRegisterOffset(idx, thermSelect);
    if (reg == -1)
        return;
    target->i2c_write(target->bus, target->addr, reg, thermlimit);
}

/**
 * @brief Disable alarm on THERM
 *
 * @param target
 * @param idx Measurement channel, with 0 being the local channel.
 * @param thermSelect TMP468_SELECT_THERM1 or TMP468_SELECT_THERM2
 */
void TMP468_disableTherm(TMP468_t *target, int idx, int thermSelect) {
    TMP468_setThermLimit(target, idx, TMP468_THERM_DISABLE, thermSelect);
}

/**
 * @brief Configure the remote temperature sensor number IDX.
 *
 * @param target Target Temperature sensor
 * @param idx Target remote sensor index (from 1 to 8)
 * @param offset Temperature offset to apply to measurement, in mili degrees [–128000, +127937]
 * @param eta ideality factor correction. Convert from float using TMP468_etatoi for compile time optimization.
 * @param THERM1 Temperature alarm 1, in mili degrees
 * @param THERM2 Temperature alarm 2, in mili degrees
 */
void TMP468_configRemote(TMP468_t *target, int idx, int32_t offset, int8_t eta, uint32_t THERM1, uint32_t THERM2) {
    if ((target == NULL) || (target->i2c_read == NULL) || (target->i2c_write == NULL)) {
        return;
    }
    // base address
    unsigned baseRegister = TMP468_REG_REMOTE_CONFIG_BASE_ADDR + (TMP468_REG_REMOTE_CONFIG_REMOTE_REMOTE_OFFSET * idx);
    unsigned offsetRegister = baseRegister + TMP468_REG_REMOTE_CONFIG_REMOTE_OFFSET_OFFSET;
    unsigned etaResister = baseRegister + TMP468_REG_REMOTE_CONFIG_REMOTE_ETA_OFFSET;
    unsigned THERM1Register = baseRegister + TMP468_REG_REMOTE_CONFIG_REMOTE_THERM1_OFFSET;
    unsigned THERM2Register = baseRegister + TMP468_REG_REMOTE_CONFIG_REMOTE_THERM2_OFFSET;

    if ((target == NULL) || (idx < 1) || (idx > 8)) {
        return;
    }

    target->i2c_write(target->bus, target->addr, offsetRegister, millisToReg(offset));
    target->i2c_write(target->bus, target->addr, etaResister, eta << TMP468_REG_REMOTE_IDEALITY_CORRECTION_pos);
    target->i2c_write(target->bus, target->addr, THERM1Register, halfDegreesToThermLimitreg(THERM1 / 2000));
    target->i2c_write(target->bus, target->addr, THERM2Register, halfDegreesToThermLimitreg(THERM2 / 2000));

    // set the configured remote as enabled
    uint16_t enabled = TMP468_getRemoteEnabled(target);
    enabled |= TMP468_CONFIG_ENABLE_REMOTE_BASE << (idx - 1);
    TMP468_setRemoteEnabled(target, enabled);

#ifdef TMP468_REMOTE_CONFIG_STRUCT
    target->remote[idx - 1].offset = offset;
    target->remote[idx - 1].idealityCorrection = eta << TMP468_REG_REMOTE_IDEALITY_CORRECTION_pos;
    target->remote[idx - 1].THERM1_limit = THERM1;
    target->remote[idx - 1].THERM2_limit = THERM2;
#endif // TMP468_REMOTE_CONFIG_STRUCT
}

/**
 * @brief enters low power mode
 *
 * @param target
 */
void TMP468_sleep(TMP468_t *target) {
    if ((target == NULL) || (target->i2c_read == NULL) || (target->i2c_write == NULL)) {
        return;
    }

    uint16_t cfg = target->i2c_read(target->bus, target->addr, TMP468_REG_CONFIGURATION);
    cfg |= TMP468_CONFIG_SHUTDOWN_mask; // 1 = enables device shutdown
    target->i2c_write(target->bus, target->addr, TMP468_REG_CONFIGURATION, cfg);
}

/**
 * @brief Enable temperature gathering according to configuration
 *
 * @param target
 */
void TMP468_wakeup(TMP468_t *target) {
    if ((target == NULL) || (target->i2c_read == NULL) || (target->i2c_write == NULL)) {
        return;
    }

    uint16_t cfg = target->i2c_read(target->bus, target->addr, TMP468_REG_CONFIGURATION);
    cfg &= ~TMP468_CONFIG_SHUTDOWN_mask; // clear bit
    target->i2c_write(target->bus, target->addr, TMP468_REG_CONFIGURATION, cfg);
}

/******************************** Lock control ********************************/

/**
 * @brief Write protect the internal registers.
 *
 * @param target
 */
void TMP468_lock(TMP468_t *target) {
    if ((target == NULL) || (target->i2c_write == NULL))
        return;
    target->i2c_write(target->bus, target->addr, TMP468_REG_LOCK, TMP468_DO_LOCK);
}

/**
 * @brief Disable write protection on internal registers
 *
 * @param target
 */
void TMP468_unlock(TMP468_t *target) {
    if ((target == NULL) || (target->i2c_write == NULL))
        return;
    target->i2c_write(target->bus, target->addr, TMP468_REG_LOCK, TMP468_DO_UNLOCK);
}

/**
 * @brief Returns current lock status
 *
 * @param target
 * @return uint16_t
 */
uint16_t TMP468_getLock(TMP468_t *target) {
    if ((target == NULL) || (target->i2c_read == NULL))
        return;
    return target->i2c_read(target->bus, target->addr, TMP468_REG_LOCK);
}
