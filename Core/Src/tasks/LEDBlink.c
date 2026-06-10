#include "cmsis_os2.h"
#include "main.h"
#include "tasks.h"


void LEDBlink(void *argument)
{

  uint32_t tick = osKernelGetTickCount();
  for(;;)
  {
    HAL_GPIO_TogglePin(LEDGRN_GPIO_Port, LEDGRN_Pin);
    osDelayUntil(tick+=250);
  }
  /* USER CODE END LEDBlink */
}
