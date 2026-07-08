#ifndef I2C_MANAGER_H
#define I2C_MANAGER_H

#include "cmsis_os2.h"
#include "app_freertos.h"
#include "main.h"
#include <stdint.h>

/* Maximum I2C payload size — tune to your largest transfer */
#define I2C_MGR_MAX_DATA_LEN   64U

/* I2C bus identifiers */
typedef enum {
    I2C_BUS_1 = 0,
    I2C_BUS_2,
    I2C_BUS_3,
    I2C_BUS_COUNT
} I2C_BusId_t;

/* Operation type */
typedef enum {
    I2C_OP_WRITE = 0,
    I2C_OP_READ
} I2C_OpType_t;

/* Operation descriptor — posted into the queue */
typedef struct {
    I2C_OpType_t    op;
    uint16_t        dev_addr;       /* 7-bit, left-shifted (<<1) as HAL expects */
    uint16_t        mem_addr;
    uint16_t        mem_addr_size;  /* I2C_MEMADD_SIZE_8BIT or _16BIT */
    uint16_t        size;
    uint8_t         data[I2C_MGR_MAX_DATA_LEN];
    HAL_StatusTypeDef result;       /* filled by task before releasing sem */
    osSemaphoreId_t done_sem;       /* caller blocks on this */
} I2C_Request_t;

/*
 * Drop-in replacements — same signature as HAL_I2C_Mem_Write/Read.
 * bus: which physical bus to use.
 * timeout_ms: how long the *caller* will wait for the op to complete.
 */
HAL_StatusTypeDef I2C_Mem_Write(I2C_BusId_t     bus,
                                uint16_t        DevAddress,
                                uint16_t        MemAddress,
                                uint16_t        MemAddSize,
                                uint8_t        *pData,
                                uint16_t        Size,
                                uint32_t        timeout_ms);

HAL_StatusTypeDef I2C_Mem_Read(I2C_BusId_t     bus,
                               uint16_t        DevAddress,
                               uint16_t        MemAddress,
                               uint16_t        MemAddSize,
                               uint8_t        *pData,
                               uint16_t        Size,
                               uint32_t        timeout_ms);

/* Task entry points — assign these in CubeMX */
void I2C1_ManagerTask(void *argument);
void I2C2_ManagerTask(void *argument);
void I2C3_ManagerTask(void *argument);


#endif //I2C_MANAGER_H

