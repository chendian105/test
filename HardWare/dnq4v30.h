#ifndef __DNQ4V30_H
#define __DNQ4V30_H


#include "Com_Config.h"

/* 设备通信参数默认配置 */
#define VIS_RX_BUFFER_SIZE     256
#define VIS_FRAME_MAX_LENGTH   128


/* 协议字段定义 */
typedef struct {
    uint32_t device_id;
    char date[9];           // YYYYMMDD
    char time[7];           // HHMMSS
    float visibility_1min;  // 1分钟能见度
    float visibility_10min; // 10分钟能见度
//    float tx_power;         // 发射能量强度
//    float rx_power;         // 接收能量强度
    float voltage;          // 工作电压
    float temperature;      // 温度
    float humidity;         // 湿度
    uint8_t weather_code;   // 天气现象代码
    float background_light; // 背景光亮度
    uint32_t status_code;   // 设备状态码
} VIS_Data_t;




//extern VIS_Handle_t Dnq4v30;

DEVICE_Status_t VIS_Driver_Init(DEVICE_Handle_t *Dnq4v30, UART_HandleTypeDef *huart);


#endif /* VIS_SENSOR_H */
