#include "single_key_detect.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define KEY_DETECT_DALAY_PERIOD_N 2 // 消抖周期数
#define KEY_DETECT_HOLD_PERIOD_N 100 // 长按周期数阈值

////////////////////////////////////////////////////////////////////////////////////////////////////////////

void init_single_key(SingleKey* single_key, SingleKeyInit* single_key_init){
	single_key->sts = KEY_WAIT_PRESS;
	single_key->delay_cnt = 0;
	single_key->pos_lv_tri = single_key_init->pos_lv_tri;
	single_key->key_id = single_key_init->key_id;
	single_key->callback = single_key_init->callback;
	single_key->GPIOx = single_key_init->GPIOx;
	single_key->Pin = single_key_init->Pin;
	single_key->hold_cnt = 0;
	single_key->hold_flag = 0;
	
	GPIO_InitTypeDef GPIO_InitStruct;
	
	GPIO_InitStruct.Pin = single_key_init->Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT; // 输入
  GPIO_InitStruct.Pull = single_key_init->pos_lv_tri ? GPIO_PULLDOWN:GPIO_PULLUP; // 使能上拉或下拉
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH; // GPIO速度
	
  HAL_GPIO_Init(single_key_init->GPIOx, &GPIO_InitStruct); // GPIO初始化
}

void do_single_key_detect(SingleKey* single_key){
	if(single_key->sts == KEY_WAIT_PRESS){
		if(HAL_GPIO_ReadPin(single_key->GPIOx, single_key->Pin) == single_key->pos_lv_tri){
			single_key->sts = KEY_DELAY;
		}
	}else if(single_key->sts == KEY_DELAY){
		single_key->delay_cnt++;
		
		if(single_key->delay_cnt == KEY_DETECT_DALAY_PERIOD_N){
			single_key->delay_cnt = 0;
			
			if(HAL_GPIO_ReadPin(single_key->GPIOx, single_key->Pin) == single_key->pos_lv_tri){
				single_key->sts = KEY_PRESSED;
			}else{
				single_key->sts = KEY_WAIT_PRESS;
			}
		}
	}else if(single_key->sts == KEY_PRESSED){
		single_key->sts = KEY_WAIT_RELEASE;
		
		single_key->callback(single_key->key_id, SINGLE_KEY_EVENT_PRESS);
	}else if(single_key->sts == KEY_WAIT_RELEASE){
		if(HAL_GPIO_ReadPin(single_key->GPIOx, single_key->Pin) != single_key->pos_lv_tri){
			single_key->sts = KEY_WAIT_PRESS;
			single_key->hold_cnt = 0;
			
			if(!single_key->hold_flag){
				single_key->callback(single_key->key_id, SINGLE_KEY_EVENT_RELEASE);
			}
			
			single_key->hold_flag = 0;
		}else{
			single_key->hold_cnt++;
			
			if(single_key->hold_cnt == KEY_DETECT_HOLD_PERIOD_N){
				single_key->callback(single_key->key_id, SINGLE_KEY_EVENT_HOLD);
				
				single_key->hold_flag = 1;
			}
		}
	}
}
