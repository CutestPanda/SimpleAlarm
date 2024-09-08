#include "beep.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////

TIM_HandleTypeDef BeepTimHandle;

////////////////////////////////////////////////////////////////////////////////////////////////////////////

HAL_StatusTypeDef init_beep(void){
	BeepTimHandle.Instance = TIM1; // 选择TIM1
  BeepTimHandle.Init.Period = 4000 - 1; // 自动重装载值
  BeepTimHandle.Init.Prescaler = 1 - 1; // 预分频数
  BeepTimHandle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1; // 时钟不分频
  BeepTimHandle.Init.CounterMode = TIM_COUNTERMODE_UP; // 向上计数
  BeepTimHandle.Init.RepetitionCounter = 1 - 1; // 不重复计数
  BeepTimHandle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE; // 自动重装载寄存器没有缓冲
	
  if(HAL_TIM_Base_Init(&BeepTimHandle) != HAL_OK){ // 基础时钟初始化
    return HAL_ERROR;
  }
	
	TIM_OC_InitTypeDef sConfig;
	
	sConfig.OCMode = TIM_OCMODE_TOGGLE; // 输出配置为翻转模式
  sConfig.OCPolarity = TIM_OCPOLARITY_HIGH; // OC通道输出高电平有效
  sConfig.OCFastMode = TIM_OCFAST_DISABLE; // 输出快速使能关闭
  sConfig.OCNPolarity = TIM_OCNPOLARITY_HIGH; // OCN通道输出高电平有效
  sConfig.OCNIdleState = TIM_OCNIDLESTATE_RESET; // 空闲状态OC1N输出低电平
  sConfig.OCIdleState = TIM_OCIDLESTATE_RESET; // 空闲状态OC1输出低电平
  sConfig.Pulse = 2000 - 1; // 比较值
	
  if (HAL_TIM_OC_ConfigChannel(&BeepTimHandle, &sConfig, TIM_CHANNEL_4) != HAL_OK){ // OC4通道配置
    return HAL_ERROR;
  }
	
	return HAL_OK;
}

HAL_StatusTypeDef beep_start(void){
	if (HAL_TIM_OC_Start(&BeepTimHandle, TIM_CHANNEL_4) != HAL_OK){ // OC4通道开始输出
    return HAL_ERROR;
  }
	
	return HAL_OK;
}

HAL_StatusTypeDef beep_stop(void){
	if (HAL_TIM_OC_Stop(&BeepTimHandle, TIM_CHANNEL_4) != HAL_OK){ // OC4通道开始输出
    return HAL_ERROR;
  }
	
	return HAL_OK;
}
