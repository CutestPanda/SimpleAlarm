#include "alarm.h"

#include "timebase.h"
#include "single_key_detect.h"
#include "oled.h"
#include "oled_gui.h"
#include "temp_sensor.h"
#include "beep.h"

#include <string.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////

// �������
#define KEY_CHANGE_MODE 0 // ����:�л�����ģʽ
#define KEY_CHANGE_SET_POS 1 // ����:�ƶ����ù��
#define KEY_CHANGE_CHANGE_NUM 2 // ����:�ı�����

// ����ģʽ
#define MODE_NORMAL 0 // ����ģʽ:����
#define MODE_SET_TIME 1 // ����ģʽ:����ʱ��
#define MODE_SET_ALARM 2 // ����ģʽ:��������

// ����ʹ�ܿ���
#define ALARM_EN_GPIO GPIOA
#define ALARM_EN_PIN GPIO_PIN_12

////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct{
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
}Time;

////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void single_key_callback(uint8_t key_id, uint8_t event_type); // ������������/�ͷŻص�����
static void upd_running_mode(uint8_t mode); // ��������ģʽ
static void change_num(uint8_t mode, uint8_t pos); // �ı�����

////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern SingleKey keys[3];

////////////////////////////////////////////////////////////////////////////////////////////////////////////

static uint8_t now_mode = MODE_NORMAL; // ��ǰ������ģʽ

static uint8_t sec_cnt; // ���Ƶ������
static Time now_time; // ��ǰʱ��
static Time time_for_set; // �����õ�ʱ��
static Time alarm_time; // ����ʱ��

static uint8_t set_pos; // ʱ�����ù��λ��

static uint8_t alarm_beep_flag; // �����־

////////////////////////////////////////////////////////////////////////////////////////////////////////////

void init_sys(void){
	HAL_Delay(500); // �ӳ�0.5s
	
	__HAL_RCC_GPIOA_CLK_ENABLE(); // GPIOAʱ��ʹ��
	__HAL_RCC_GPIOB_CLK_ENABLE(); // GPIOBʱ��ʹ��
	
	// ��ʼ��OLED��ص�GPIO
	GPIO_InitTypeDef GPIO_InitStruct;
	
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP; // �������
  GPIO_InitStruct.Pull = GPIO_PULLUP; // ʹ������
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH; // GPIO�ٶ�
	
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
	
	// ��ʼ������ʹ�ܿ���
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT; // ����
	GPIO_InitStruct.Pull = GPIO_PULLDOWN; // ʹ������
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM; // GPIO�ٶ�
	GPIO_InitStruct.Pin = ALARM_EN_PIN;
	HAL_GPIO_Init(ALARM_EN_GPIO, &GPIO_InitStruct);
	
	// ��ʼ����������
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
	
	// ��ʼ���¶ȴ�����
	init_temp_sensor();
	
	// ��ʼ��ϵͳʱ��
	init_timebase();
	
	// ��ʼ��������
	init_beep();
	
	// ��ʼ��OLED
	OLED_Init();
	OLED_Clear(0);
	
	// ��ʾ"ף���������"
	uint32_t ids[6] = {9, 10, 11, 12, 13, 14};
	
	GUI_ShowCHinese(32, 6, 16, ids, 1, 6);
	
	// ��ʼ������
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
		
		// ���µ�ǰʱ��
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
		
		// �������
		if(alarm_beep_flag){
			if(HAL_GPIO_ReadPin(ALARM_EN_GPIO, ALARM_EN_PIN) == GPIO_PIN_RESET){
				// �ر�����
				beep_stop();
				alarm_beep_flag = 0;
			}
		}else{
			if((now_time.hour == alarm_time.hour) && 
				(now_time.minute == alarm_time.minute) && (now_time.second == 0)){
				if(HAL_GPIO_ReadPin(ALARM_EN_GPIO, ALARM_EN_PIN) == GPIO_PIN_SET){
					// ��������
					beep_start();
					alarm_beep_flag = 1;
				}
			}
		}
		
		// ��ʾʱ��
		if(now_mode == MODE_NORMAL){ // ����
			GUI_ShowChar(0, 2, now_time.hour / 10 + '0', 16, 1);
			GUI_ShowChar(8, 2, now_time.hour % 10 + '0', 16, 1);
			GUI_ShowChar(16, 2, ':', 16, 1);
			GUI_ShowChar(24, 2, now_time.minute / 10 + '0', 16, 1);
			GUI_ShowChar(32, 2, now_time.minute % 10 + '0', 16, 1);
			GUI_ShowChar(40, 2, ':', 16, 1);
			GUI_ShowChar(48, 2, now_time.second / 10 + '0', 16, 1);
			GUI_ShowChar(56, 2, now_time.second % 10 + '0', 16, 1);
		}else if(now_mode == MODE_SET_TIME){ // ����ʱ��
			GUI_ShowChar(0, 2, time_for_set.hour / 10 + '0', 16, set_pos != 0);
			GUI_ShowChar(8, 2, time_for_set.hour % 10 + '0', 16, set_pos != 0);
			GUI_ShowChar(16, 2, ':', 16, 1);
			GUI_ShowChar(24, 2, time_for_set.minute / 10 + '0', 16, set_pos != 1);
			GUI_ShowChar(32, 2, time_for_set.minute % 10 + '0', 16, set_pos != 2);
			GUI_ShowChar(40, 2, ' ', 16, 1);
			GUI_ShowChar(48, 2, ' ', 16, 1);
			GUI_ShowChar(56, 2, ' ', 16, 1);
		}else if(now_mode == MODE_SET_ALARM){ // ��������
			GUI_ShowChar(0, 2, alarm_time.hour / 10 + '0', 16, set_pos != 0);
			GUI_ShowChar(8, 2, alarm_time.hour % 10 + '0', 16, set_pos != 0);
			GUI_ShowChar(16, 2, ':', 16, 1);
			GUI_ShowChar(24, 2, alarm_time.minute / 10 + '0', 16, set_pos != 1);
			GUI_ShowChar(32, 2, alarm_time.minute % 10 + '0', 16, set_pos != 2);
			GUI_ShowChar(40, 2, ' ', 16, 1);
			GUI_ShowChar(48, 2, ' ', 16, 1);
			GUI_ShowChar(56, 2, ' ', 16, 1);
		}
		
		// ��ʾ�¶�
		if(now_mode == MODE_NORMAL){ // ����
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
		if(key_id == KEY_CHANGE_MODE){ // �л�����ģʽ
			if(now_mode == MODE_SET_ALARM){
				now_mode = MODE_NORMAL;
			}else{
				now_mode++;
			}
			
			if(now_mode != MODE_NORMAL){
				// ����¶���ʾ
				uint32_t ids[3] = {8, 8, 8};
				
				GUI_ShowCHinese(80, 2, 16, ids, 1, 3);
			}
			
			upd_running_mode(now_mode);
		}else if(key_id == KEY_CHANGE_CHANGE_NUM){ // �ı�����
			change_num(now_mode, set_pos);
		}
	}else if(event_type == SINGLE_KEY_EVENT_HOLD){
		if(key_id == KEY_CHANGE_MODE){ // �ƶ����ù��
			if(set_pos == 2){
				set_pos = 0;
			}else{
				set_pos++;
			}
		}else if(key_id == KEY_CHANGE_CHANGE_NUM){
			if(now_mode == MODE_SET_TIME){ // ����ʱ��
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
	
	// ���ģʽ��ʾ
	GUI_ShowCHinese(16 * 4, 0, 16, ids, 1, 4);
	
	// ����ʱ�����ù��λ��
	set_pos = 0;
	
	switch(mode){
		case MODE_NORMAL: // ����
			break;
		case MODE_SET_TIME: // ����ʱ��
			time_for_set.hour = now_time.hour;
			time_for_set.minute = now_time.minute;
			
			// ��ʾ"����ʱ��"
			ids[0] = 2;
			ids[1] = 3;
			ids[2] = 4;
			ids[3] = 5;
			GUI_ShowCHinese(64, 0, 16, ids, 1, 4);
			break;
		case MODE_SET_ALARM: // ��������
			// ��ʾ"��������"
			ids[0] = 2;
			ids[1] = 3;
			ids[2] = 6;
			ids[3] = 7;
			GUI_ShowCHinese(64, 0, 16, ids, 1, 4);
			break;
	}
}

static void change_num(uint8_t mode, uint8_t pos){
	if(mode == MODE_SET_TIME){ // ����ʱ��
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
	}else if(mode == MODE_SET_ALARM){ // ��������
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
