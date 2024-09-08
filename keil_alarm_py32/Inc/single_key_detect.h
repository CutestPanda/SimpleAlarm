#include "main.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define SINGLE_KEY_EVENT_PRESS 0 // 事件:按下
#define SINGLE_KEY_EVENT_HOLD 1 // 事件:长按
#define SINGLE_KEY_EVENT_RELEASE 2 // 事件:释放

////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __SINGLE_KEY_DETECT_H
#define __SINGLE_KEY_DETECT_H

typedef enum{
	KEY_WAIT_PRESS,
	KEY_DELAY,
	KEY_PRESSED,
	KEY_WAIT_RELEASE
}SingleKeySts;

typedef void (*Single_Key_Callback)(uint8_t key_id, uint8_t event_type);

typedef struct{
	SingleKeySts sts;
	uint16_t delay_cnt;
	uint8_t pos_lv_tri;
	uint8_t key_id;
	Single_Key_Callback callback;
	GPIO_TypeDef* GPIOx;
	uint32_t Pin;
	uint32_t hold_cnt;
	uint8_t hold_flag;
}SingleKey;

typedef struct{
	uint8_t pos_lv_tri;
	uint8_t key_id;
	Single_Key_Callback callback;
	GPIO_TypeDef* GPIOx;
	uint32_t Pin;
}SingleKeyInit;
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////

void init_single_key(SingleKey* single_key, SingleKeyInit* single_key_init);

void do_single_key_detect(SingleKey* single_key);
