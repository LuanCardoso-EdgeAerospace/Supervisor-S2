/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : app_freertos.h
  * Description        : FreeRTOS applicative header file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __APP_FREERTOS_H
#define __APP_FREERTOS_H

#ifdef __cplusplus
extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os2.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Exported macro -------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */
extern osThreadId_t defaultTaskHandle;
extern osThreadId_t LEDBlinkHandle;
extern osThreadId_t printerHandle;
extern osThreadId_t powerUpHandle;
extern osThreadId_t serviceWatchdogHandle;
extern osThreadId_t logPowerHandle;
extern osThreadId_t printPowerHandle;
extern osThreadId_t I2C1_ManagerHandle;
extern osThreadId_t I2C2_ManagerHandle;
extern osThreadId_t I2C3_ManagerHandle;
extern osThreadId_t printThermHandle;
extern osMutexId_t powerLogMutexHandle;
extern osMutexId_t thermLogMutexHandle;
extern osMessageQueueId_t printQueueHandle;
extern osMessageQueueId_t i2c1QueueHandle;
extern osMessageQueueId_t i2c2QueueHandle;
extern osMessageQueueId_t i2c3QueueHandle;

/* Exported function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void LEDBlink(void *argument);
void printer(void *argument);
void powerUp(void *argument);
void serviceWatchdog(void *argument);
void logPower(void *argument);
extern void printPower(void *argument);
extern void I2C1_ManagerTask(void *argument);
extern void I2C2_ManagerTask(void *argument);
extern void I2C3_ManagerTask(void *argument);
void printTherm(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* Hook prototypes */
void configureTimerForRunTimeStats(void);
unsigned long getRunTimeCounterValue(void);
/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

#ifdef __cplusplus
}
#endif
#endif /* __APP_FREERTOS_H */
