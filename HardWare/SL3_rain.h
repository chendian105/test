#ifndef __SL3_RAIN_H
#define __SL3_RAIN_H

#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif

/* 硬件配置宏（可移植性设计） */
#define RAIN_SENSOR_GPIO_PORT        GPIOA
#define RAIN_SENSOR_GPIO_PIN         GPIO_PIN_0
#define RAIN_SENSOR_EXTI_IRQn        EXTI0_IRQn

/* 降水量统计相关 */
typedef struct {
    volatile uint32_t total_tips;              // 总翻转次数
    float precipitation;              // 累计降水量（单位：mm）
} RainSensor_Data;

/* 初始化函数声明 */
void RainSensor_Init(void);
RainSensor_Data RainSensor_GetData(void);
void RainSensor_ClearData(void);

/* 中断回调原型（HAL库兼容） */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);

#ifdef __cplusplus
}
#endif


#endif
