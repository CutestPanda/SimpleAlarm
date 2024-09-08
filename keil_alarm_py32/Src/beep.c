#include "beep.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////

TIM_HandleTypeDef BeepTimHandle;

////////////////////////////////////////////////////////////////////////////////////////////////////////////

HAL_StatusTypeDef init_beep(void){
	BeepTimHandle.Instance = TIM1; // ѡ��TIM1
  BeepTimHandle.Init.Period = 4000 - 1; // �Զ���װ��ֵ
  BeepTimHandle.Init.Prescaler = 1 - 1; // Ԥ��Ƶ��
  BeepTimHandle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1; // ʱ�Ӳ���Ƶ
  BeepTimHandle.Init.CounterMode = TIM_COUNTERMODE_UP; // ���ϼ���
  BeepTimHandle.Init.RepetitionCounter = 1 - 1; // ���ظ�����
  BeepTimHandle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE; // �Զ���װ�ؼĴ���û�л���
	
  if(HAL_TIM_Base_Init(&BeepTimHandle) != HAL_OK){ // ����ʱ�ӳ�ʼ��
    return HAL_ERROR;
  }
	
	TIM_OC_InitTypeDef sConfig;
	
	sConfig.OCMode = TIM_OCMODE_TOGGLE; // �������Ϊ��תģʽ
  sConfig.OCPolarity = TIM_OCPOLARITY_HIGH; // OCͨ������ߵ�ƽ��Ч
  sConfig.OCFastMode = TIM_OCFAST_DISABLE; // �������ʹ�ܹر�
  sConfig.OCNPolarity = TIM_OCNPOLARITY_HIGH; // OCNͨ������ߵ�ƽ��Ч
  sConfig.OCNIdleState = TIM_OCNIDLESTATE_RESET; // ����״̬OC1N����͵�ƽ
  sConfig.OCIdleState = TIM_OCIDLESTATE_RESET; // ����״̬OC1����͵�ƽ
  sConfig.Pulse = 2000 - 1; // �Ƚ�ֵ
	
  if (HAL_TIM_OC_ConfigChannel(&BeepTimHandle, &sConfig, TIM_CHANNEL_4) != HAL_OK){ // OC4ͨ������
    return HAL_ERROR;
  }
	
	return HAL_OK;
}

HAL_StatusTypeDef beep_start(void){
	if (HAL_TIM_OC_Start(&BeepTimHandle, TIM_CHANNEL_4) != HAL_OK){ // OC4ͨ����ʼ���
    return HAL_ERROR;
  }
	
	return HAL_OK;
}

HAL_StatusTypeDef beep_stop(void){
	if (HAL_TIM_OC_Stop(&BeepTimHandle, TIM_CHANNEL_4) != HAL_OK){ // OC4ͨ����ʼ���
    return HAL_ERROR;
  }
	
	return HAL_OK;
}
