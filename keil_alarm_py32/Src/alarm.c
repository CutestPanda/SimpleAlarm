#include "alarm.h"

#include "timebase.h"
#include "single_key_detect.h"
#include "oled.h"
#include "oled_gui.h"
#include "temp_sensor.h"
#include "beep.h"

#include <string.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////

// 按键编号
#define KEY_CHANGE_MODE 0 // 按键:切换运行模式
#define KEY_CHANGE_SET_POS 1 // 按键:移动设置光标
#define KEY_CHANGE_CHANGE_NUM 2 // 按键:改变数字

// 运行模式
#define MODE_NORMAL 0 // 运行模式:正常
#define MODE_SET_TIME 1 // 运行模式:设置时间
#define MODE_SET_ALARM 2 // 运行模式:设置闹钟

// 闹钟使能开关
#define ALARM_EN_GPIO GPIOA
#define ALARM_EN_PIN GPIO_PIN_12

////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct{
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
}Time;

////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void single_key_callback(uint8_t key_id, uint8_t event_type); // 独立按键按下/释放回调函数
static void upd_running_mode(uint8_t mode); // 更新运行模式
static void change_num(uint8_t mode, uint8_t pos); // 改变数字

////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern SingleKey keys[3];

////////////////////////////////////////////////////////////////////////////////////////////////////////////

static uint8_t now_mode = MODE_NORMAL; // 当前的运行模式

static uint8_t sec_cnt; // 秒分频计数器
static Time now_time; // 当前时间
static Time time_for_set; // 待设置的时间
static Time alarm_time; // 闹钟时间

static uint8_t set_pos; // 时间设置光标位置

static uint8_t alarm_beep_flag; // 响铃标志

////////////////////////////////////////////////////////////////////////////////////////////////////////////

void init_sys(void){
	HAL_Delay(500); // 延迟0.5s
	
	__HAL_RCC_GPIOA_CLK_ENABLE(); // GPIOA时钟使能
	__HAL_RCC_GPIOB_CLK_ENABLE(); // GPIOB时钟使能
	
	// 初始化OLED相关的GPIO
	GPIO_InitTypeDef GPIO_InitStruct;
	
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP; // 推挽输出
  GPIO_InitStruct.Pull = GPIO_PULLUP; // 使能上拉
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH; // GPIO速度
	
	GPIO_InitStruct.Pin = GPIO_PIN_7;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = GPIO_PIN_0;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = GPIO_PIN_7;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = GPIO_PIN_6;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = GPIO_PIN_5;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	// 初始化闹钟使能开关
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT; // 输入
	GPIO_InitStruct.Pull = GPIO_PULLDOWN; // 使能下拉
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM; // GPIO速度
	GPIO_InitStruct.Pin = ALARM_EN_PIN;
	HAL_GPIO_Init(ALARM_EN_GPIO, &GPIO_InitStruct);
	
	// 初始化独立按键
	SingleKeyInit key_init;
	key_init.callback = single_key_callback;
	key_init.GPIOx = GPIOA;
	key_init.Pin = GPIO_PIN_4;
	key_init.key_id = KEY_CHANGE_MODE;
	key_init.pos_lv_tri = 0;
	
	init_single_key(&keys[0], &key_init);
	
	key_init.GPIOx = GPIOA;
	key_init.Pin = GPIO_PIN_5;
	key_init.key_id = KEY_CHANGE_SET_POS;
	
	init_single_key(&keys[1], &key_init);
	
	key_init.GPIOx = GPIOA;
	key_init.Pin = GPIO_PIN_6;
	key_init.key_id = KEY_CHANGE_CHANGE_NUM;
	
	init_single_key(&keys[2], &key_init);
	
	// 初始化温度传感器
	init_temp_sensor();
	
	// 初始化系统时基
	init_timebase();
	
	// 初始化蜂鸣器
	init_beep();
	
	// 初始化OLED
	OLED_Init();
	OLED_Clear(0);
	
	// 显示"祝您生活愉快"
	uint32_t ids[6] = {9, 10, 11, 12, 13, 14};
	
	GUI_ShowCHinese(32, 6, 16, ids, 1, 6);
	
	// 初始化闹钟
	sec_cnt = 0;
	set_pos = 0;
	alarm_beep_flag = 0;
	now_time.hour = 0;
	now_time.minute = 0;
	now_time.second = 0;
	time_for_set.hour = 0;
	time_for_set.minute = 0;
	alarm_time.hour = 0;
	alarm_time.minute = 0;
}

void show_alarm_gui(void){
	sec_cnt++;
	
	if(sec_cnt == 50){
		sec_cnt = 0;
		
		// 更新当前时间
		if(now_time.second == 59){
			now_time.second = 0;
			
			if(now_time.minute == 59){
				now_time.minute = 0;
				
				if(now_time.hour == 23){
					now_time.hour = 0;
				}else{
					now_time.hour++;
				}
			}else{
				now_time.minute++;
			}
		}else{
			now_time.second++;
		}
		
		// 检查闹钟
		if(alarm_beep_flag){
			if(HAL_GPIO_ReadPin(ALARM_EN_GPIO, ALARM_EN_PIN) == GPIO_PIN_RESET){
				// 关闭闹钟
				beep_stop();
				alarm_beep_flag = 0;
			}
		}else{
			if((now_time.hour == alarm_time.hour) && 
				(now_time.minute == alarm_time.minute) && (now_time.second == 0)){
				if(HAL_GPIO_ReadPin(ALARM_EN_GPIO, ALARM_EN_PIN) == GPIO_PIN_SET){
					// 闹钟响铃
					beep_start();
					alarm_beep_flag = 1;
				}
			}
		}
		
		// 显示时间
		if(now_mode == MODE_NORMAL){ // 正常
			GUI_ShowChar(0, 2, now_time.hour / 10 + '0', 16, 1);
			GUI_ShowChar(8, 2, now_time.hour % 10 + '0', 16, 1);
			GUI_ShowChar(16, 2, ':', 16, 1);
			GUI_ShowChar(24, 2, now_time.minute / 10 + '0', 16, 1);
			GUI_ShowChar(32, 2, now_time.minute % 10 + '0', 16, 1);
			GUI_ShowChar(40, 2, ':', 16, 1);
			GUI_ShowChar(48, 2, now_time.second / 10 + '0', 16, 1);
			GUI_ShowChar(56, 2, now_time.second % 10 + '0', 16, 1);
		}else if(now_mode == MODE_SET_TIME){ // 设置时间
			GUI_ShowChar(0, 2, time_for_set.hour / 10 + '0', 16, set_pos != 0);
			GUI_ShowChar(8, 2, time_for_set.hour % 10 + '0', 16, set_pos != 0);
			GUI_ShowChar(16, 2, ':', 16, 1);
			GUI_ShowChar(24, 2, time_for_set.minute / 10 + '0', 16, set_pos != 1);
			GUI_ShowChar(32, 2, time_for_set.minute % 10 + '0', 16, set_pos != 2);
			GUI_ShowChar(40, 2, ' ', 16, 1);
			GUI_ShowChar(48, 2, ' ', 16, 1);
			GUI_ShowChar(56, 2, ' ', 16, 1);
		}else if(now_mode == MODE_SET_ALARM){ // 设置闹钟
			GUI_ShowChar(0, 2, alarm_time.hour / 10 + '0', 16, set_pos != 0);
			GUI_ShowChar(8, 2, alarm_time.hour % 10 + '0', 16, set_pos != 0);
			GUI_ShowChar(16, 2, ':', 16, 1);
			GUI_ShowChar(24, 2, alarm_time.minute / 10 + '0', 16, set_pos != 1);
			GUI_ShowChar(32, 2, alarm_time.minute % 10 + '0', 16, set_pos != 2);
			GUI_ShowChar(40, 2, ' ', 16, 1);
			GUI_ShowChar(48, 2, ' ', 16, 1);
			GUI_ShowChar(56, 2, ' ', 16, 1);
		}
		
		// 显示温度
		if(now_mode == MODE_NORMAL){ // 正常
			int16_t temp;
			
			if(get_temp(&temp) == HAL_OK){
				char temp_str[20];
				uint32_t id = 1;
				
				sprintf(temp_str, "%2.2d.%1.1d ", temp / 10 % 100, temp % 10);
				
				GUI_ShowString(80, 2, (u8*)temp_str, 16, 1);
				GUI_ShowCHinese(112, 2, 16, &id, 1, 1);
			}
		}
	}
}

static void single_key_callback(uint8_t key_id, uint8_t event_type){
	if(event_type == SINGLE_KEY_EVENT_RELEASE){
		if(key_id == KEY_CHANGE_MODE){ // 切换运行模式
			if(now_mode == MODE_SET_ALARM){
				now_mode = MODE_NORMAL;
			}else{
				now_mode++;
			}
			
			if(now_mode != MODE_NORMAL){
				// 清除温度显示
				uint32_t ids[3] = {8, 8, 8};
				
				GUI_ShowCHinese(80, 2, 16, ids, 1, 3);
			}
			
			upd_running_mode(now_mode);
		}else if(key_id == KEY_CHANGE_CHANGE_NUM){ // 改变数字
			change_num(now_mode, set_pos);
		}
	}else if(event_type == SINGLE_KEY_EVENT_HOLD){
		if(key_id == KEY_CHANGE_MODE){ // 移动设置光标
			if(set_pos == 2){
				set_pos = 0;
			}else{
				set_pos++;
			}
		}else if(key_id == KEY_CHANGE_CHANGE_NUM){
			if(now_mode == MODE_SET_TIME){ // 设置时间
				now_mode = MODE_NORMAL;
				
				now_time.hour = time_for_set.hour;
				now_time.minute = time_for_set.minute;
				now_time.second = 0;
			}
		}
	}
}

static void upd_running_mode(uint8_t mode){
	uint32_t ids[4] = {8, 8, 8, 8};
	
	// 清除模式显示
	GUI_ShowCHinese(16 * 4, 0, 16, ids, 1, 4);
	
	// 重置时间设置光标位置
	set_pos = 0;
	
	switch(mode){
		case MODE_NORMAL: // 正常
			break;
		case MODE_SET_TIME: // 设置时间
			time_for_set.hour = now_time.hour;
			time_for_set.minute = now_time.minute;
			
			// 显示"设置时间"
			ids[0] = 2;
			ids[1] = 3;
			ids[2] = 4;
			ids[3] = 5;
			GUI_ShowCHinese(64, 0, 16, ids, 1, 4);
			break;
		case MODE_SET_ALARM: // 设置闹钟
			// 显示"设置闹钟"
			ids[0] = 2;
			ids[1] = 3;
			ids[2] = 6;
			ids[3] = 7;
			GUI_ShowCHinese(64, 0, 16, ids, 1, 4);
			break;
	}
}

static void change_num(uint8_t mode, uint8_t pos){
	if(mode == MODE_SET_TIME){ // 设置时间
		switch(pos){
			case 0:
				if(time_for_set.hour >= 23){
					time_for_set.hour = 0;
				}else{
					time_for_set.hour++;
				}
				break;
			case 1:
				if(time_for_set.minute >= 50){
					time_for_set.minute -= 50;
				}else{
					time_for_set.minute += 10;
				}
				break;
			case 2:
				if((time_for_set.minute % 10) == 9){
					time_for_set.minute -= 9;
				}else{
					time_for_set.minute++;
				}
				break;
		}
	}else if(mode == MODE_SET_ALARM){ // 设置闹钟
		switch(pos){
			case 0:
				if(alarm_time.hour >= 23){
					alarm_time.hour = 0;
				}else{
					alarm_time.hour++;
				}
				break;
			case 1:
				if(alarm_time.minute >= 50){
					alarm_time.minute -= 50;
				}else{
					alarm_time.minute += 10;
				}
				break;
			case 2:
				if((alarm_time.minute % 10) == 9){
					alarm_time.minute -= 9;
				}else{
					alarm_time.minute++;
				}
				break;
		}
	}
}
