/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32u0xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define CFG2_Pin GPIO_PIN_13
#define CFG2_GPIO_Port GPIOC
#define PWR_ALRT_1V2_Pin GPIO_PIN_2
#define PWR_ALRT_1V2_GPIO_Port GPIOC
#define PWR_ALRT_1V35_Pin GPIO_PIN_3
#define PWR_ALRT_1V35_GPIO_Port GPIOC
#define MCU_RS422_TX_Pin GPIO_PIN_2
#define MCU_RS422_TX_GPIO_Port GPIOA
#define MCU_RS422_RX_Pin GPIO_PIN_3
#define MCU_RS422_RX_GPIO_Port GPIOA
#define SNS_I2C_SDA_Pin GPIO_PIN_6
#define SNS_I2C_SDA_GPIO_Port GPIOA
#define SNS_I2C_SCL_Pin GPIO_PIN_7
#define SNS_I2C_SCL_GPIO_Port GPIOA
#define PWR_ALRT_3V3_Pin GPIO_PIN_4
#define PWR_ALRT_3V3_GPIO_Port GPIOC
#define PWR_ALRT_0V6_Pin GPIO_PIN_5
#define PWR_ALRT_0V6_GPIO_Port GPIOC
#define STATUS_LED2_RED_Pin GPIO_PIN_7
#define STATUS_LED2_RED_GPIO_Port GPIOE
#define STATUS_LED1_GRN_Pin GPIO_PIN_8
#define STATUS_LED1_GRN_GPIO_Port GPIOE
#define UART3_CPU_RX_Pin GPIO_PIN_10
#define UART3_CPU_RX_GPIO_Port GPIOB
#define UART3_CPU_TX_Pin GPIO_PIN_11
#define UART3_CPU_TX_GPIO_Port GPIOB
#define TEMP_I2C_SCL_Pin GPIO_PIN_13
#define TEMP_I2C_SCL_GPIO_Port GPIOB
#define TEMP_I2C_SDA_Pin GPIO_PIN_14
#define TEMP_I2C_SDA_GPIO_Port GPIOB
#define EN_12V0P_Pin GPIO_PIN_8
#define EN_12V0P_GPIO_Port GPIOD
#define PG_12V_PS_Pin GPIO_PIN_9
#define PG_12V_PS_GPIO_Port GPIOD
#define PMIC_EN_Pin GPIO_PIN_10
#define PMIC_EN_GPIO_Port GPIOD
#define PWR_ALRT_2V5_Pin GPIO_PIN_6
#define PWR_ALRT_2V5_GPIO_Port GPIOC
#define PWR_ALRT_1V8_Pin GPIO_PIN_7
#define PWR_ALRT_1V8_GPIO_Port GPIOC
#define PWR_ALRT_12V_Pin GPIO_PIN_8
#define PWR_ALRT_12V_GPIO_Port GPIOC
#define FRAM_SCL_Pin GPIO_PIN_9
#define FRAM_SCL_GPIO_Port GPIOA
#define FRAM_SDA_Pin GPIO_PIN_10
#define FRAM_SDA_GPIO_Port GPIOA
#define PUSH_BUTTON_Pin GPIO_PIN_11
#define PUSH_BUTTON_GPIO_Port GPIOC
#define CFG1_Pin GPIO_PIN_12
#define CFG1_GPIO_Port GPIOC
#define CFG3_Pin GPIO_PIN_0
#define CFG3_GPIO_Port GPIOD
#define MCU_WATCHDOG_PULSE_Pin GPIO_PIN_2
#define MCU_WATCHDOG_PULSE_GPIO_Port GPIOD
#define MCU_RS422_EN_Pin GPIO_PIN_3
#define MCU_RS422_EN_GPIO_Port GPIOD
#define LCL_1_Pin GPIO_PIN_4
#define LCL_1_GPIO_Port GPIOB
#define LCL_2_Pin GPIO_PIN_5
#define LCL_2_GPIO_Port GPIOB
#define LCL_3_Pin GPIO_PIN_6
#define LCL_3_GPIO_Port GPIOB
#define LCL_4_Pin GPIO_PIN_7
#define LCL_4_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
