#include "headlibs.h"

/* 私有变量 */
static RainSensor_Data sensor_data = {0};
static QueueHandle_t xRainQueue = NULL;

/* 初始化GPIO和中断 */
void RainSensor_Init(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    __HAL_RCC_GPIOA_CLK_ENABLE();
    
    /* 配置GPIO为上拉输入 */
    GPIO_InitStruct.Pin = RAIN_SENSOR_GPIO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING; // 下降沿触发
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(RAIN_SENSOR_GPIO_PORT, &GPIO_InitStruct);
    
    /* 配置外部中断优先级 */
    HAL_NVIC_SetPriority(RAIN_SENSOR_EXTI_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(RAIN_SENSOR_EXTI_IRQn);
    
    /* 创建FreeRTOS队列用于中断通信 */
    xRainQueue = xQueueCreate(1, sizeof(uint32_t));
}

/* 获取当前降水量数据 */
RainSensor_Data RainSensor_GetData(void) {
    RainSensor_Data temp;
    taskENTER_CRITICAL();
    temp = sensor_data;
    taskEXIT_CRITICAL();
    return temp;
}

/* 清除累计数据 */
void RainSensor_ClearData(void) {
    taskENTER_CRITICAL();
    sensor_data.total_tips = 0;
    sensor_data.precipitation = 0.0f;
    taskEXIT_CRITICAL();
}

/* 中断回调函数 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    
    if (GPIO_Pin == RAIN_SENSOR_GPIO_PIN) {
        /* 消抖处理（硬件消抖已通过RC电路实现） */
        if (HAL_GPIO_ReadPin(RAIN_SENSOR_GPIO_PORT, RAIN_SENSOR_GPIO_PIN) == GPIO_PIN_RESET) {
            sensor_data.total_tips++;
            sensor_data.precipitation += 0.1f; // 每次翻转0.1mm
            
            /* 发送信号到队列（非阻塞） */
            xQueueSendFromISR(xRainQueue, (uint32_t *)&sensor_data.total_tips, &xHigherPriorityTaskWoken);
        }
    }
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

/* EXTI0中断服务函数 */
void EXTI0_IRQHandler(void) {
    HAL_GPIO_EXTI_IRQHandler(RAIN_SENSOR_GPIO_PIN);
}
