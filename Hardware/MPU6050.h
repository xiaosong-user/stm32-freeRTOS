#ifndef _MPU6050_H_
#define _MPU6050_H_

#include "stm32f10x.h" 


typedef struct Six_axis
{
	int16_t Accx;
	int16_t Accy;
	int16_t Accz;
	int16_t Gyrox;
	int16_t Gyroy;
	int16_t Gyroz;

}MPU6050_Six_axis;

void MPU6050_WriteReg(uint8_t RegAddress, uint8_t Data);
uint8_t MPU6050_ReadReg(uint8_t RegAddress);
void MPU6050_Init(void);
uint8_t MPU6050_GetID(void);
void MPU6050_GetData(MPU6050_Six_axis *Axis);
void MPU6050_data_display(MPU6050_Six_axis *Axis,uint8_t *ID);//数据显示


#endif
