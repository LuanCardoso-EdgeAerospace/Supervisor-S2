#include "cmsis_os2.h"
#include "main.h"
#include "tasks.h"


void LEDBlink(void *argument)
{
  /* USER CODE BEGIN LEDBlink */
  /* Infinite loop */

  for(;;)
  {
    HAL_GPIO_TogglePin(LEDGRN_GPIO_Port, LEDGRN_Pin);
    osDelay(500);
    HAL_GPIO_TogglePin(LEDRED_GPIO_Port, LEDRED_Pin);
    osDelay(500);
  }
  /* USER CODE END LEDBlink */
}
