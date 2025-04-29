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

/***************相关宏定义***************/
#define DEVICE_RX_BUFFER_SIZE     256   //数据大小

#define SENDDATA_HEAD          0xAA   //数据帧头
#define SENDDATA_TIAL          0x55   //数据帧头

/***************设备串口****************/
#define DEBUG_UART       huart1            //调试
#define DNQ4V30_UART     huart2            //能见度
#define LASERC12_UART    huart2
#define DSG9RAD485_UART  huart2
#define ASC200_UART      huart2            //云量云状

/***************************************/
#define TRANSFOR_TIME_MAX 3

/***********************************/
extern uint8_t dnq4v30_buf;  //中断接收数据缓存（能见度）
extern uint8_t c12_buf;      //中断接收数据缓存（云高仪）

extern uint16_t send_data_flag;  //数据接收完成标志

// 全局测试状态标识
typedef enum {
    TEST_INIT,
    TEST_SET_INTERVAL,
    TEST_DATA_RECEIVED,
    TEST_ERROR
} TestState_t;

/* 协议字段定义 */
typedef struct {
	uint8_t header;         // 帧头
	char timestamp[15];     // 当前时间
	uint8_t flags;          // 标志位
	
	float visibility_1min;  // 1分钟能见度
	float visibility_10min; // 10分钟能见度
	
	uint8_t checksum;       // 校验码
	uint8_t tail;           // 帧尾

} Com_Data_Status;

/*设备状态枚举*/
typedef enum {
   DEVICE_OK          = 0x00,
   DEVICE_CRC_ERR     = 0x01,
   DEVICE_TIMEOUT     = 0x02,
   DEVICE_FRAME_ERR   = 0x03,
	 DEVICE_ERR         = 0x04,
} DEVICE_Status_t;

/* 主动握手串口设备驱动控制结构体 */
typedef struct {
    UART_HandleTypeDef *huart;              //设备串口
																			     
		SemaphoreHandle_t data_analysis;        //启动数据解析(分段放入结构体)
		BaseType_t DataAnalysis;                //数据解析信号量接收句柄
																			     
		SemaphoreHandle_t data_processing;      //启动数据处理（转存，发送等）
		BaseType_t DataProcess;                 //数据处理信号量接收句柄
																			     
    uint8_t rx_buffer[DEVICE_RX_BUFFER_SIZE];  //数据缓冲区
    uint16_t rx_index;                      //数组索引
		TaskHandle_t processTaskHandle;         //任务句柄
	
} DEVICE_Handle_t;

/* 被动握手串口设备驱动控制结构体 */
typedef struct {
    UART_HandleTypeDef *huart;              //设备串口
	
		TimerHandle_t xTimerHandle;             //定时器句柄			

		SemaphoreHandle_t data_analysis;        //启动数据解析(分段放入结构体)
		BaseType_t DataAnalysis;                //数据解析信号量接收句柄	
																			     
		SemaphoreHandle_t data_processing;      //启动数据处理（转存，发送等）
		BaseType_t DataProcess;                 //数据处理信号量接收句柄
																			     
    uint8_t rx_buffer[DEVICE_RX_BUFFER_SIZE];  //数据缓冲区
    uint16_t rx_index;                      //数组索引
		TaskHandle_t processTaskHandle;         //任务句柄
	
} PASVDEVICE_Handle_t;

uint16_t VIS_CRC16(const uint8_t *data, uint16_t length);

void pack_data(void);
void GetTimestampNow(char* timebuff, size_t bufferSize);
void ComSendData(UART_HandleTypeDef *huart_sensor, const char* data);
DEVICE_Status_t ReceiveResponse(UART_HandleTypeDef *huart_sensor,char* response, uint16_t maxLen);

//输出数据结构体
extern Com_Data_Status ComDataStruct;
extern uint16_t COMMUNICATION_EXEC_CYCLE;    //传输结构体

#endif
