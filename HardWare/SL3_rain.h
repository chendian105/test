#ifndef __SL3_RAIN_H
#define __SL3_RAIN_H

#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Ӳ�����ú꣨����ֲ����ƣ� */
#define RAIN_SENSOR_GPIO_PORT        GPIOA
#define RAIN_SENSOR_GPIO_PIN         GPIO_PIN_0
#define RAIN_SENSOR_EXTI_IRQn        EXTI0_IRQn

/* ��ˮ��ͳ����� */
typedef struct {
    volatile uint32_t total_tips;              // �ܷ�ת����
    float precipitation;              // �ۼƽ�ˮ������λ��mm��
} RainSensor_Data;

/* ��ʼ���������� */
void RainSensor_Init(void);
RainSensor_Data RainSensor_GetData(void);
void RainSensor_ClearData(void);

/* �жϻص�ԭ�ͣ�HAL����ݣ� */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);

#ifdef __cplusplus
}
#endif


#endif
