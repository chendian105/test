#ifndef __LASERC12_H
#define __LASERC12_H

#include "Com_Config.h"

/* �豸ͨ�Ų���Ĭ������ */
#define C12_RX_BUFFER_SIZE     256
#define C12_FRAME_MAX_LENGTH   128

// �����Ƹ��ǵ�ͨѶЭ������
typedef enum {
    MLD0_MODE = 0,  // MLDЭ��������ٲ�ģʽ
    MLD1_MODE,      // MLDЭ��������ϴ�ģʽ
    BOOK0_MODE,     // �����ֵ�Э��������ٲ�ģʽ
    BOOK1_MODE      // �����ֵ�Э��������ϴ�ģʽ
} ProtocolMode;

/* Э���ֶζ��� */
typedef struct {
    
    char date[11];          // ���ڣ���ʽΪYYYY-MM-DD
    char time[9];           // ʱ�䣬��ʽΪHH:MM:SS
    uint16_t deviceID;      // �豸ID
    uint8_t skyStatus;      // ���״̬
    uint16_t cloudHeight[5];// �Ƹߣ����5�㣩
    uint16_t cloudThickness[5]; // �ƺ����5�㣩
    uint16_t verticalVisibility; // ��ֱ�ܼ���
    uint8_t lowCloudCover;  // ��������
    uint8_t midCloudCover;  // ��������
    uint8_t highCloudCover; // ��������
    uint8_t totalCloudCover;// ������
    uint16_t statusCode;    // �豸״̬��
    uint16_t checksum;      // ��У��

} MLD_Data_t;


DEVICE_Status_t LaserC12_Driver_Init(DEVICE_Handle_t *Dnq4v30, UART_HandleTypeDef *huart);

#endif
