#ifndef __COM_CONFIG_H
#define __COM_CONFIG_H

#include "stm32f1xx_hal.h"
#include "FreeRTOS.h"
//#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "stdint.h"
#include "stdio.h"
#include "timers.h"

/***************��غ궨��***************/
#define DEVICE_RX_BUFFER_SIZE     256   //���ݴ�С

#define SENDDATA_HEAD          0xAA   //����֡ͷ
#define SENDDATA_TIAL          0x55   //����֡ͷ

/***************�豸����****************/
#define DEBUG_UART       huart1            //����
#define DNQ4V30_UART     huart2            //�ܼ���
#define LASERC12_UART    huart2
#define DSG9RAD485_UART  huart2
#define ASC200_UART      huart2            //������״

/***************************************/
#define TRANSFOR_TIME_MAX 3

/***********************************/
extern uint8_t dnq4v30_buf;  //�жϽ������ݻ��棨�ܼ��ȣ�
extern uint8_t c12_buf;      //�жϽ������ݻ��棨�Ƹ��ǣ�

extern uint16_t send_data_flag;  //���ݽ�����ɱ�־

// ȫ�ֲ���״̬��ʶ
typedef enum {
    TEST_INIT,
    TEST_SET_INTERVAL,
    TEST_DATA_RECEIVED,
    TEST_ERROR
} TestState_t;

/* Э���ֶζ��� */
typedef struct {
	uint8_t header;         // ֡ͷ
	char timestamp[15];     // ��ǰʱ��
	uint8_t flags;          // ��־λ
	
	float visibility_1min;  // 1�����ܼ���
	float visibility_10min; // 10�����ܼ���
	
	uint8_t checksum;       // У����
	uint8_t tail;           // ֡β

} Com_Data_Status;

/*�豸״̬ö��*/
typedef enum {
   DEVICE_OK          = 0x00,
   DEVICE_CRC_ERR     = 0x01,
   DEVICE_TIMEOUT     = 0x02,
   DEVICE_FRAME_ERR   = 0x03,
	 DEVICE_ERR         = 0x04,
} DEVICE_Status_t;

/* �������ִ����豸�������ƽṹ�� */
typedef struct {
    UART_HandleTypeDef *huart;              //�豸����
																			     
		SemaphoreHandle_t data_analysis;        //�������ݽ���(�ֶη���ṹ��)
		BaseType_t DataAnalysis;                //���ݽ����ź������վ��
																			     
		SemaphoreHandle_t data_processing;      //�������ݴ���ת�棬���͵ȣ�
		BaseType_t DataProcess;                 //���ݴ����ź������վ��
																			     
    uint8_t rx_buffer[DEVICE_RX_BUFFER_SIZE];  //���ݻ�����
    uint16_t rx_index;                      //��������
		TaskHandle_t processTaskHandle;         //������
	
} DEVICE_Handle_t;

/* �������ִ����豸�������ƽṹ�� */
typedef struct {
    UART_HandleTypeDef *huart;              //�豸����
	
		TimerHandle_t xTimerHandle;             //��ʱ�����			

		SemaphoreHandle_t data_analysis;        //�������ݽ���(�ֶη���ṹ��)
		BaseType_t DataAnalysis;                //���ݽ����ź������վ��	
																			     
		SemaphoreHandle_t data_processing;      //�������ݴ���ת�棬���͵ȣ�
		BaseType_t DataProcess;                 //���ݴ����ź������վ��
																			     
    uint8_t rx_buffer[DEVICE_RX_BUFFER_SIZE];  //���ݻ�����
    uint16_t rx_index;                      //��������
		TaskHandle_t processTaskHandle;         //������
	
} PASVDEVICE_Handle_t;

uint16_t VIS_CRC16(const uint8_t *data, uint16_t length);

void pack_data(void);
void GetTimestampNow(char* timebuff, size_t bufferSize);
void ComSendData(UART_HandleTypeDef *huart_sensor, const char* data);
DEVICE_Status_t ReceiveResponse(UART_HandleTypeDef *huart_sensor,char* response, uint16_t maxLen);

//������ݽṹ��
extern Com_Data_Status ComDataStruct;
extern uint16_t COMMUNICATION_EXEC_CYCLE;    //����ṹ��

#endif
