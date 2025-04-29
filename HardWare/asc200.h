#ifndef __ASC200_H
#define __ASC200_H

#include "Com_config.h"

// 定义串口句柄
extern UART_HandleTypeDef huart1;

//// 定义485通信的DE/RE控制引脚
//#define DE_RE_GPIO_PORT GPIOA
//#define DE_RE_PIN GPIO_PIN_8

// 定义数据帧的最大长度
#define MAX_FRAME_LENGTH 256

// 定义数据帧的起始和结束标识
#define FRAME_START "BG"
#define FRAME_END "ED"

// 定义校验和长度
#define CHECKSUM_LENGTH 4

// 定义观测要素和状态变量的最大数量
#define MAX_OBSERVATION_VARIABLES 99
#define MAX_STATUS_VARIABLES 99

// 定义观测要素和状态变量的结构体
typedef struct {
    char variableName[4];
    int variableValue;
    int qualityControl;
} ObservationVariable;

typedef struct {
    char statusName[4];
    int statusValue;
} StatusVariable;

// 定义数据帧结构体
typedef struct {
    char start[2];
    char stationID[6];
    char serviceType[2];
    char deviceID[4];
    char deviceNumber[3];
    char timestamp[14];
    char frameFlag[3];
    char observationVariableCount[2];
    char statusVariableCount[2];
    ObservationVariable observationVariables[MAX_OBSERVATION_VARIABLES];
    StatusVariable statusVariables[MAX_STATUS_VARIABLES];
    char checksum[CHECKSUM_LENGTH];
    char end[2];
} ASC200_DataFrame;

// 函数声明
DEVICE_Status_t ASC_Init(PASVDEVICE_Handle_t *ASC200,UART_HandleTypeDef *huart);
void ASC_SendCommand(const char* command);
void ASC_ReceiveData(ASC200_DataFrame* dataFrame);
void ASC_ProcessData(ASC200_DataFrame* dataFrame);
uint16_t ASC_CalculateChecksum(const char* data, uint16_t length);

#endif // ASC_DRIVER_H

