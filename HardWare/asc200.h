#ifndef __ASC200_H
#define __ASC200_H

#include "Com_config.h"

// ���崮�ھ��
extern UART_HandleTypeDef huart1;

//// ����485ͨ�ŵ�DE/RE��������
//#define DE_RE_GPIO_PORT GPIOA
//#define DE_RE_PIN GPIO_PIN_8

// ��������֡����󳤶�
#define MAX_FRAME_LENGTH 256

// ��������֡����ʼ�ͽ�����ʶ
#define FRAME_START "BG"
#define FRAME_END "ED"

// ����У��ͳ���
#define CHECKSUM_LENGTH 4

// ����۲�Ҫ�غ�״̬�������������
#define MAX_OBSERVATION_VARIABLES 99
#define MAX_STATUS_VARIABLES 99

// ����۲�Ҫ�غ�״̬�����Ľṹ��
typedef struct {
    char variableName[4];
    int variableValue;
    int qualityControl;
} ObservationVariable;

typedef struct {
    char statusName[4];
    int statusValue;
} StatusVariable;

// ��������֡�ṹ��
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

// ��������
DEVICE_Status_t ASC_Init(PASVDEVICE_Handle_t *ASC200,UART_HandleTypeDef *huart);
void ASC_SendCommand(const char* command);
void ASC_ReceiveData(ASC200_DataFrame* dataFrame);
void ASC_ProcessData(ASC200_DataFrame* dataFrame);
uint16_t ASC_CalculateChecksum(const char* data, uint16_t length);

#endif // ASC_DRIVER_H

