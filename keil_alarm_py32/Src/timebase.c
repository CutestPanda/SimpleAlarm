#include "timebase.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////

TIM_HandleTypeDef TimHandle;

////////////////////////////////////////////////////////////////////////////////////////////////////////////

HAL_StatusTypeDef init_timebase(void){
	TimHandle.Instance = TIM3;                                           //ѡ��TIM3
  TimHandle.Init.Period            = 8000 - 1;                         //�Զ���װ��ֵ
  TimHandle.Init.Prescaler         = 20 - 1;                           //Ԥ��Ƶ��
  TimHandle.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;           //ʱ�Ӳ���Ƶ
  TimHandle.Init.CounterMode       = TIM_COUNTERMODE_UP;               //���ϼ���
  TimHandle.Init.RepetitionCounter = 1 - 1;                            //���ظ�����
  TimHandle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;   //�Զ���װ�ؼĴ���û�л���
	
	if (HAL_TIM_Base_Init(&TimHandle) != HAL_OK)                         //TIM3��ʼ��
  {
    return HAL_ERROR;
  }

  if (HAL_TIM_Base_Start_IT(&TimHandle) != HAL_OK)                     //TIM3ʹ����������ʹ���ж�
  {
    return HAL_ERROR;
  }
	
  return HAL_OK;
}
