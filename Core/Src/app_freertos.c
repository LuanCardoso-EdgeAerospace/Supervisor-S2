/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : app_freertos.c
  * Description        : FreeRTOS applicative file
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

/* Includes ------------------------------------------------------------------*/
#include "app_freertos.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 128 * 4
};
/* Definitions for LEDBlink */
osThreadId_t LEDBlinkHandle;
const osThreadAttr_t LEDBlink_attributes = {
  .name = "LEDBlink",
  .priority = (osPriority_t) osPriorityLow,
  .stack_size = 128 * 4
};
/* Definitions for printer */
osThreadId_t printerHandle;
const osThreadAttr_t printer_attributes = {
  .name = "printer",
  .priority = (osPriority_t) osPriorityLow,
  .stack_size = 512 * 4
};
/* Definitions for powerUp */
osThreadId_t powerUpHandle;
const osThreadAttr_t powerUp_attributes = {
  .name = "powerUp",
  .priority = (osPriority_t) osPriorityHigh,
  .stack_size = 256 * 4
};
/* Definitions for serviceWatchdog */
osThreadId_t serviceWatchdogHandle;
const osThreadAttr_t serviceWatchdog_attributes = {
  .name = "serviceWatchdog",
  .priority = (osPriority_t) osPriorityRealtime,
  .stack_size = 128 * 4
};
/* Definitions for logPower */
osThreadId_t logPowerHandle;
const osThreadAttr_t logPower_attributes = {
  .name = "logPower",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 128 * 4
};
/* Definitions for powerLogMutex */
osMutexId_t powerLogMutexHandle;
const osMutexAttr_t powerLogMutex_attributes = {
  .name = "powerLogMutex"
};
/* Definitions for printQueue */
osMessageQueueId_t printQueueHandle;
const osMessageQueueAttr_t printQueue_attributes = {
  .name = "printQueue"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */
  /* creation of powerLogMutex */
  powerLogMutexHandle = osMutexNew(&powerLogMutex_attributes);

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */
  /* creation of printQueue */
  printQueueHandle = osMessageQueueNew (16, 128, &printQueue_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of LEDBlink */
  LEDBlinkHandle = osThreadNew(LEDBlink, NULL, &LEDBlink_attributes);

  /* creation of printer */
  printerHandle = osThreadNew(printer, NULL, &printer_attributes);

  /* creation of powerUp */
  powerUpHandle = osThreadNew(powerUp, NULL, &powerUp_attributes);

  /* creation of serviceWatchdog */
  serviceWatchdogHandle = osThreadNew(serviceWatchdog, NULL, &serviceWatchdog_attributes);

  /* creation of logPower */
  logPowerHandle = osThreadNew(logPower, NULL, &logPower_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}
/* USER CODE BEGIN Header_StartDefaultTask */
/**
* @brief Function implementing the defaultTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN defaultTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END defaultTask */
}

/* USER CODE BEGIN Header_LEDBlink */
/**
* @brief Function implementing the LEDBlink thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_LEDBlink */
__weak void LEDBlink(void *argument)
{
  /* USER CODE BEGIN LEDBlink */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END LEDBlink */
}

/* USER CODE BEGIN Header_printer */
/**
* @brief Function implementing the printer thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_printer */
__weak void printer(void *argument)
{
  /* USER CODE BEGIN printer */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END printer */
}

/* USER CODE BEGIN Header_powerUp */
/**
* @brief Function implementing the powerUp thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_powerUp */
__weak void powerUp(void *argument)
{
  /* USER CODE BEGIN powerUp */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END powerUp */
}

/* USER CODE BEGIN Header_serviceWatchdog */
/**
* @brief Function implementing the serviceWatchdog thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_serviceWatchdog */
__weak void serviceWatchdog(void *argument)
{
  /* USER CODE BEGIN serviceWatchdog */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1000);
  }
  /* USER CODE END serviceWatchdog */
}

/* USER CODE BEGIN Header_logPower */
/**
* @brief Function implementing the logPower thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_logPower */
__weak void logPower(void *argument)
{
  /* USER CODE BEGIN logPower */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END logPower */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

