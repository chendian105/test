#include "headlibs.h"

/* ˽�б��� */
static RainSensor_Data sensor_data = {0};
static QueueHandle_t xRainQueue = NULL;

/* ��ʼ��GPIO���ж� */
void RainSensor_Init(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    __HAL_RCC_GPIOA_CLK_ENABLE();
    
    /* ����GPIOΪ�������� */
    GPIO_InitStruct.Pin = RAIN_SENSOR_GPIO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING; // �½��ش���
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(RAIN_SENSOR_GPIO_PORT, &GPIO_InitStruct);
    
    /* �����ⲿ�ж����ȼ� */
    HAL_NVIC_SetPriority(RAIN_SENSOR_EXTI_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(RAIN_SENSOR_EXTI_IRQn);
    
    /* ����FreeRTOS���������ж�ͨ�� */
    xRainQueue = xQueueCreate(1, sizeof(uint32_t));
}

/* ��ȡ��ǰ��ˮ������ */
RainSensor_Data RainSensor_GetData(void) {
    RainSensor_Data temp;
    taskENTER_CRITICAL();
    temp = sensor_data;
    taskEXIT_CRITICAL();
    return temp;
}

/* ����ۼ����� */
void RainSensor_ClearData(void) {
    taskENTER_CRITICAL();
    sensor_data.total_tips = 0;
    sensor_data.precipitation = 0.0f;
    taskEXIT_CRITICAL();
}

/* �жϻص����� */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    
    if (GPIO_Pin == RAIN_SENSOR_GPIO_PIN) {
        /* ��������Ӳ��������ͨ��RC��·ʵ�֣� */
        if (HAL_GPIO_ReadPin(RAIN_SENSOR_GPIO_PORT, RAIN_SENSOR_GPIO_PIN) == GPIO_PIN_RESET) {
            sensor_data.total_tips++;
            sensor_data.precipitation += 0.1f; // ÿ�η�ת0.1mm
            
            /* �����źŵ����У��������� */
            xQueueSendFromISR(xRainQueue, (uint32_t *)&sensor_data.total_tips, &xHigherPriorityTaskWoken);
        }
    }
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

/* EXTI0�жϷ����� */
void EXTI0_IRQHandler(void) {
    HAL_GPIO_EXTI_IRQHandler(RAIN_SENSOR_GPIO_PIN);
}
