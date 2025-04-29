#ifndef __DNQ4V30_H
#define __DNQ4V30_H


#include "Com_Config.h"

/* �豸ͨ�Ų���Ĭ������ */
#define VIS_RX_BUFFER_SIZE     256
#define VIS_FRAME_MAX_LENGTH   128


/* Э���ֶζ��� */
typedef struct {
    uint32_t device_id;
    char date[9];           // YYYYMMDD
    char time[7];           // HHMMSS
    float visibility_1min;  // 1�����ܼ���
    float visibility_10min; // 10�����ܼ���
//    float tx_power;         // ��������ǿ��
//    float rx_power;         // ��������ǿ��
    float voltage;          // ������ѹ
    float temperature;      // �¶�
    float humidity;         // ʪ��
    uint8_t weather_code;   // �����������
    float background_light; // ����������
    uint32_t status_code;   // �豸״̬��
} VIS_Data_t;




//extern VIS_Handle_t Dnq4v30;

DEVICE_Status_t VIS_Driver_Init(DEVICE_Handle_t *Dnq4v30, UART_HandleTypeDef *huart);


#endif /* VIS_SENSOR_H */
