#include "headlibs.h"

/*̫������Ƶķ��ش�����û�ĳ������ļ��Ĵ����룬�����޸�*/

// ����UART���
//UART_HandleTypeDef huart2;

// ���峬ʱʱ��
#define RAD485_TIMEOUT 1000

// ��ʼ��485ͨ��
uint8_t RAD485_Init(UART_HandleTypeDef *huart) {
    huart2 = *huart;
    return RAD485_ERROR_NONE;
}

// ��ȡ����˲ʱֵ
uint8_t RAD485_ReadRadiationInstantValue(uint16_t *radiationValue) {
    uint8_t requestFrame[8];
    uint8_t responseFrame[7];
    uint16_t crc;

    // ��������֡
    requestFrame[0] = DEFAULT_DEVICE_ADDRESS;  // �豸��ַ
    requestFrame[1] = MODBUS_READ_REGISTERS;   // ������
    requestFrame[2] = (RADIATION_INSTANT_VALUE_ADDR >> 8) & 0xFF;  // ��ʼ��ַ���ֽ�
    requestFrame[3] = RADIATION_INSTANT_VALUE_ADDR & 0xFF;         // ��ʼ��ַ���ֽ�
    requestFrame[4] = 0x00;  // �Ĵ����������ֽ�
    requestFrame[5] = 0x01;  // �Ĵ����������ֽ�

    // ����CRC
    crc = RAD485_CalculateCRC(requestFrame, 6);
    requestFrame[6] = crc & 0xFF;  // CRC���ֽ�
    requestFrame[7] = (crc >> 8) & 0xFF;  // CRC���ֽ�

    // ��������֡
    if (HAL_UART_Transmit(&huart2, requestFrame, 8, RAD485_TIMEOUT) != HAL_OK) {
        return RAD485_ERROR_TIMEOUT;
    }

    // ������Ӧ֡
    if (HAL_UART_Receive(&huart2, responseFrame, 7, RAD485_TIMEOUT) != HAL_OK) {
        return RAD485_ERROR_TIMEOUT;
    }

    // У��CRC
    crc = RAD485_CalculateCRC(responseFrame, 5);
    if ((responseFrame[5] != (crc & 0xFF)) || (responseFrame[6] != ((crc >> 8) & 0xFF))) {
        return RAD485_ERROR_CRC;
    }

    // ������Ӧ����
    if (responseFrame[0] != DEFAULT_DEVICE_ADDRESS || responseFrame[1] != MODBUS_READ_REGISTERS) {
        return RAD485_ERROR_INVALID_RESPONSE;
    }

    *radiationValue = (responseFrame[3] << 8) | responseFrame[4];
    return RAD485_ERROR_NONE;
}

// ����CRC
uint16_t RAD485_CalculateCRC(uint8_t *data, uint8_t length) {
    uint16_t crc = 0xFFFF;
    uint8_t i, j;

    for (i = 0; i < length; i++) {
        crc ^= data[i];
        for (j = 0; j < 8; j++) {
            if (crc & 0x0001) {
                crc >>= 1;
                crc ^= 0xA001;
            } else {
                crc >>= 1;
            }
        }
    }
    return crc;
}
