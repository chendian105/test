#ifndef __TBQ2_H
#define __TBQ2_H


#include "Com_Config.h"

// ����485ͨ�ŵĲ�����
#define RAD485_BAUDRATE 9600

// ����Modbus������
#define MODBUS_READ_COILS 0x01
#define MODBUS_WRITE_COIL 0x05
#define MODBUS_READ_REGISTERS 0x03
#define MODBUS_WRITE_REGISTERS 0x10

// ����Ĭ���豸��ַ
#define DEFAULT_DEVICE_ADDRESS 0x01

// ����Ĵ�����ַ
#define RADIATION_INSTANT_VALUE_ADDR 0x0000

// ���������
#define RAD485_ERROR_NONE 0
#define RAD485_ERROR_TIMEOUT 1
#define RAD485_ERROR_CRC 2
#define RAD485_ERROR_INVALID_RESPONSE 3

// ����485ͨ�ŵ�UART���
//extern UART_HandleTypeDef huart2;

// ��������
uint8_t RAD485_Init(UART_HandleTypeDef *huart);
uint8_t RAD485_ReadRadiationInstantValue(uint16_t *radiationValue);
uint16_t RAD485_CalculateCRC(uint8_t *data, uint8_t length);


#endif
