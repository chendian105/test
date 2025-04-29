#ifndef __LASERC12_H
#define __LASERC12_H

#include "Com_Config.h"

/* 设备通信参数默认配置 */
#define C12_RX_BUFFER_SIZE     256
#define C12_FRAME_MAX_LENGTH   128

// 定义云高仪的通讯协议类型
typedef enum {
    MLD0_MODE = 0,  // MLD协议的命令召测模式
    MLD1_MODE,      // MLD协议的主动上传模式
    BOOK0_MODE,     // 数据字典协议的命令召测模式
    BOOK1_MODE      // 数据字典协议的主动上传模式
} ProtocolMode;

/* 协议字段定义 */
typedef struct {
    
    char date[11];          // 日期，格式为YYYY-MM-DD
    char time[9];           // 时间，格式为HH:MM:SS
    uint16_t deviceID;      // 设备ID
    uint8_t skyStatus;      // 天空状态
    uint16_t cloudHeight[5];// 云高（最多5层）
    uint16_t cloudThickness[5]; // 云厚（最多5层）
    uint16_t verticalVisibility; // 垂直能见度
    uint8_t lowCloudCover;  // 低云云量
    uint8_t midCloudCover;  // 中云云量
    uint8_t highCloudCover; // 高云云量
    uint8_t totalCloudCover;// 总云量
    uint16_t statusCode;    // 设备状态码
    uint16_t checksum;      // 和校验

} MLD_Data_t;


DEVICE_Status_t LaserC12_Driver_Init(DEVICE_Handle_t *Dnq4v30, UART_HandleTypeDef *huart);

#endif
