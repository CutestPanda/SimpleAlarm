#include "timebase.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////

TIM_HandleTypeDef TimHandle;

////////////////////////////////////////////////////////////////////////////////////////////////////////////

HAL_StatusTypeDef init_timebase(void){
	TimHandle.Instance = TIM3;                                           //选择TIM3
  TimHandle.Init.Period            = 8000 - 1;                         //自动重装载值
  TimHandle.Init.Prescaler         = 20 - 1;                           //预分频数
  TimHandle.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;           //时钟不分频
  TimHandle.Init.CounterMode       = TIM_COUNTERMODE_UP;               //向上计数
  TimHandle.Init.RepetitionCounter = 1 - 1;                            //不重复计数
  TimHandle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;   //自动重装载寄存器没有缓冲
	
	if (HAL_TIM_Base_Init(&TimHandle) != HAL_OK)                         //TIM3初始化
  {
    return HAL_ERROR;
  }

  if (HAL_TIM_Base_Start_IT(&TimHandle) != HAL_OK)                     //TIM3使能启动，并使能中断
  {
    return HAL_ERROR;
  }
	
  return HAL_OK;
}
