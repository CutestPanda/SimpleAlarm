#include "temp_sensor.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define TMEP_SENSOR_SLAVE_ADDR 0x90

////////////////////////////////////////////////////////////////////////////////////////////////////////////

static I2C_HandleTypeDef I2cHandle;

static uint8_t temp_i2c_rx_buf[2];

////////////////////////////////////////////////////////////////////////////////////////////////////////////

HAL_StatusTypeDef init_temp_sensor(void){
	I2cHandle.Instance = I2C; // I2C
  I2cHandle.Init.ClockSpeed = 100000; // I2C通讯速度
  I2cHandle.Init.DutyCycle = I2C_DUTYCYCLE_16_9; // I2C占空比
  I2cHandle.Init.OwnAddress1 = TMEP_SENSOR_SLAVE_ADDR; // I2C地址
  I2cHandle.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE; // 禁止广播呼叫
  I2cHandle.Init.NoStretchMode = I2C_NOSTRETCH_ENABLE; // 允许时钟延长
	
  if (HAL_I2C_Init(&I2cHandle) != HAL_OK){ // I2C初始化
    return HAL_ERROR;
  }
	
	return HAL_OK;
}

HAL_StatusTypeDef get_temp(int16_t* temp){
	uint8_t send_buf[1] = {0x00};
	
	if(HAL_I2C_Master_Transmit(&I2cHandle, TMEP_SENSOR_SLAVE_ADDR, send_buf, 1, 5000) != HAL_OK){
		return HAL_ERROR;
  }
	
	while(HAL_I2C_GetState(&I2cHandle) != HAL_I2C_STATE_READY);
	
	if(HAL_I2C_Master_Receive(&I2cHandle, TMEP_SENSOR_SLAVE_ADDR, temp_i2c_rx_buf, 2, 5000) != HAL_OK){
    return HAL_ERROR;
  }
	
	while(HAL_I2C_GetState(&I2cHandle) != HAL_I2C_STATE_READY);
	
	int16_t temp_sensor_value = (temp_i2c_rx_buf[1] >> 5);
	temp_sensor_value |= (((uint16_t)temp_i2c_rx_buf[0]) << 3);
	
	if(temp_i2c_rx_buf[0] & 0x80000000){
		temp_sensor_value |= 0xF800;
	}
	
	*temp = temp_sensor_value;
	
	return HAL_OK;
}
