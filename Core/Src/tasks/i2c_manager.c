//TODO: Validate design of the manager
#include "i2c_manager.h"
#include "cmsis_os2.h"
#include <string.h>

/* --- Extern handles generated in main.c / app_freertos.c --- */
extern I2C_HandleTypeDef hi2c1;
extern I2C_HandleTypeDef hi2c2;
extern I2C_HandleTypeDef hi2c3;

extern osMessageQueueId_t i2c1QueueHandle;
extern osMessageQueueId_t i2c2QueueHandle;
extern osMessageQueueId_t i2c3QueueHandle;

static I2C_HandleTypeDef *i2c_get_handle(I2C_BusId_t bus)
{
    switch (bus) {
    case I2C_BUS_1: return &hi2c1;
    case I2C_BUS_2: return &hi2c2;
    case I2C_BUS_3: return &hi2c3;
    default:        return NULL;
    }
}

static osMessageQueueId_t i2c_get_queue(I2C_BusId_t bus)
{
    switch (bus) {
    case I2C_BUS_1: return i2c1QueueHandle;
    case I2C_BUS_2: return i2c2QueueHandle;
    case I2C_BUS_3: return i2c3QueueHandle;
    default:        return NULL;
    }
}

/* --- internal helpers ---- */
static HAL_StatusTypeDef i2c_submit(I2C_BusId_t bus,
                                    I2C_Request_t *op,
                                    uint32_t timeout_ms){

    osMessageQueueId_t queue = i2c_get_queue(bus);
    if (bus >= I2C_BUS_COUNT || queue == NULL) {
        return HAL_ERROR;
    }

    /* Binary semaphore: caller will block on it until the task signals done */
    op->done_sem = osSemaphoreNew(1U, 0U, NULL);
    if (op->done_sem == NULL) {
        return HAL_ERROR;
    }

    /* Post the op descriptor (by value — safe because caller is blocked) */
    osStatus_t st = osMessageQueuePut(queue, op, 0U,
                                      pdMS_TO_TICKS(timeout_ms));
    if (st != osOK) {
        osSemaphoreDelete(op->done_sem);
        return HAL_TIMEOUT;
    }

    /* Block until the I2C task signals completion */
    st = osSemaphoreAcquire(op->done_sem, pdMS_TO_TICKS(timeout_ms));
    osSemaphoreDelete(op->done_sem);

    if (st != osOK) {
        return HAL_TIMEOUT;
    }

    return op->result;
}

/* --- Public api ---*/

HAL_StatusTypeDef I2C_Mem_Write(I2C_BusId_t bus, 
                                uint16_t DevAddress, 
                                uint16_t MemAddress, 
                                uint16_t MemAddSize,
                                uint8_t *pData, 
                                uint16_t Size, 
                                uint32_t timeout_ms) {
    if (Size > I2C_MGR_MAX_DATA_LEN || pData == NULL) {
        return HAL_ERROR;
    }

    I2C_Request_t op = {
        .op = I2C_OP_WRITE,
        .dev_addr = DevAddress,
        .mem_addr = MemAddress,
        .mem_addr_size = MemAddSize,
        .size = Size,
    };
    memcpy(op.data, pData, Size);

    return i2c_submit(bus, &op, timeout_ms);
}

HAL_StatusTypeDef I2C_Mem_Read(I2C_BusId_t bus, 
                                uint16_t DevAddress, 
                                uint16_t MemAddress, 
                                uint16_t MemAddSize,
                                uint8_t *pData, 
                                uint16_t Size, 
                                uint32_t timeout_ms) {
    if (Size > I2C_MGR_MAX_DATA_LEN || pData == NULL) {
        return HAL_ERROR;
    }

    I2C_Request_t op = {
        .op = I2C_OP_READ,
        .dev_addr = DevAddress,
        .mem_addr = MemAddress,
        .mem_addr_size = MemAddSize,
        .size = Size,
    };

    HAL_StatusTypeDef ret = i2c_submit(bus, &op, timeout_ms);
    if (ret == HAL_OK) {
        memcpy(pData, op.data, Size);
    }
    return ret;
}

/* ── Shared task body ───────────────────────────────────────────────────── */
static void i2c_task_run(I2C_BusId_t bus)
{
    I2C_Request_t op;

    for (;;) {
        /* Block indefinitely waiting for an operation */
        osMessageQueueId_t queue = i2c_get_queue(bus);
        I2C_HandleTypeDef *hi2c = i2c_get_handle(bus);

        if (queue == NULL || hi2c == NULL) {
            osDelay(1U);
            continue;
        }

        if (osMessageQueueGet(queue, &op, NULL, osWaitForever) != osOK) {
            continue;
        }
        HAL_StatusTypeDef  res;

        if (op.op == I2C_OP_WRITE) {
            res = HAL_I2C_Mem_Write(hi2c,
                                    op.dev_addr,
                                    op.mem_addr,
                                    op.mem_addr_size,
                                    op.data,
                                    op.size,
                                    HAL_MAX_DELAY);
        } else {
            res = HAL_I2C_Mem_Read(hi2c,
                                   op.dev_addr,
                                   op.mem_addr,
                                   op.mem_addr_size,
                                   op.data,
                                   op.size,
                                   HAL_MAX_DELAY);
        }

        op.result = res;

        /* Signal the waiting caller */
        osSemaphoreRelease(op.done_sem);
    }
}

/* ── Task entry points (assign in CubeMX) ───────────────────────────────── */
void I2C1_ManagerTask(void *argument) { (void)argument; i2c_task_run(I2C_BUS_1); }
void I2C2_ManagerTask(void *argument) { (void)argument; i2c_task_run(I2C_BUS_2); }
void I2C3_ManagerTask(void *argument) { (void)argument; i2c_task_run(I2C_BUS_3); }