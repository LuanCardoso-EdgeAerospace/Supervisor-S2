#include "i2c_manager.h"
#include "cmsis_os2.h"

/* --- Extern queue handles  --- */
extern osMessageQueueId_t i2c1QueueHandle;
extern osMessageQueueId_t i2c2QueueHandle;
extern osMessageQueueId_t i2c3QueueHandle;

/* --- Internal state --- */
static I2C_HandleTypeDef *s_hi2c[I2C_BUS_COUNT];
static osMessageQueueId_t s_queues[I2C_BUS_COUNT]; /* indexed by I2C_BusId_t */

/* --- Module Init --- */
void I2C_Manager_Init(I2C_HandleTypeDef *hi2c1,
                      I2C_HandleTypeDef *hi2c2,
                      I2C_HandleTypeDef *hi2c3){

    s_hi2c[I2C_BUS_1] = hi2c1;
    s_hi2c[I2C_BUS_2] = hi2c2;
    s_hi2c[I2C_BUS_3] = hi2c3;
    
    s_queues[I2C_BUS_1] = i2c1QueueHandle;
    s_queues[I2C_BUS_2] = i2c2QueueHandle;
    s_queues[I2C_BUS_3] = i2c3QueueHandle;
}

/* --- internal helpers ---- */
static HAL_StatusTypeDef i2c_submit(I2C_BusId_t bus,
                                    I2C_Request_t *op,
                                    uint32_t timeout_ms){

    if (bus >= I2C_BUS_COUNT || s_queues[bus] == NULL) {
        return HAL_ERROR;
    }

    /* Binary semaphore: caller will block on it until the task signals done */
    op->done_sem = osSemaphoreNew(1U, 0U, NULL);
    if (op->done_sem == NULL) {
        return HAL_ERROR;
    }

    /* Post the op descriptor (by value — safe because caller is blocked) */
    osStatus_t st = osMessageQueuePut(s_queues[bus], op, 0U,
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

HAL_StatusTypeDef I2C_Mem_Write(I2C_BusId_t  bus,
                                uint16_t     DevAddress,
                                uint16_t     MemAddress,
                                uint16_t     MemAddSize,
                                uint8_t     *pData,
                                uint16_t     Size,
                                uint32_t     timeout_ms)
{
    if (Size > I2C_MGR_MAX_DATA_LEN || pData == NULL) {
        return HAL_ERROR;
    }

    I2C_Request_t op = {
        .op            = I2C_OP_WRITE,
        .dev_addr      = DevAddress,
        .mem_addr      = MemAddress,
        .mem_addr_size = MemAddSize,
        .size          = Size,
    };
    memcpy(op.data, pData, Size);

    return i2c_submit(bus, &op, timeout_ms);
}

HAL_StatusTypeDef I2C_Mem_Read(I2C_BusId_t  bus,
                               uint16_t     DevAddress,
                               uint16_t     MemAddress,
                               uint16_t     MemAddSize,
                               uint8_t     *pData,
                               uint16_t     Size,
                               uint32_t     timeout_ms)
{
    if (Size > I2C_MGR_MAX_DATA_LEN || pData == NULL) {
        return HAL_ERROR;
    }

    I2C_Request_t op = {
        .op            = I2C_OP_READ,
        .dev_addr      = DevAddress,
        .mem_addr      = MemAddress,
        .mem_addr_size = MemAddSize,
        .size          = Size,
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
        if (osMessageQueueGet(s_queues[bus], &op, NULL, osWaitForever) != osOK) {
            continue;
        }

        I2C_HandleTypeDef *hi2c = s_hi2c[bus];
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