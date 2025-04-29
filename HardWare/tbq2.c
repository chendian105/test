#include "headlibs.h"

/*太阳辐射计的返回错误码没改成配置文件的错误码，后续修改*/

// 定义UART句柄
//UART_HandleTypeDef huart2;

// 定义超时时间
#define RAD485_TIMEOUT 1000

// 初始化485通信
uint8_t RAD485_Init(UART_HandleTypeDef *huart) {
    huart2 = *huart;
    return RAD485_ERROR_NONE;
}

// 读取辐射瞬时值
uint8_t RAD485_ReadRadiationInstantValue(uint16_t *radiationValue) {
    uint8_t requestFrame[8];
    uint8_t responseFrame[7];
    uint16_t crc;

    // 构建请求帧
    requestFrame[0] = DEFAULT_DEVICE_ADDRESS;  // 设备地址
    requestFrame[1] = MODBUS_READ_REGISTERS;   // 功能码
    requestFrame[2] = (RADIATION_INSTANT_VALUE_ADDR >> 8) & 0xFF;  // 起始地址高字节
    requestFrame[3] = RADIATION_INSTANT_VALUE_ADDR & 0xFF;         // 起始地址低字节
    requestFrame[4] = 0x00;  // 寄存器数量高字节
    requestFrame[5] = 0x01;  // 寄存器数量低字节

    // 计算CRC
    crc = RAD485_CalculateCRC(requestFrame, 6);
    requestFrame[6] = crc & 0xFF;  // CRC低字节
    requestFrame[7] = (crc >> 8) & 0xFF;  // CRC高字节

    // 发送请求帧
    if (HAL_UART_Transmit(&huart2, requestFrame, 8, RAD485_TIMEOUT) != HAL_OK) {
        return RAD485_ERROR_TIMEOUT;
    }

    // 接收响应帧
    if (HAL_UART_Receive(&huart2, responseFrame, 7, RAD485_TIMEOUT) != HAL_OK) {
        return RAD485_ERROR_TIMEOUT;
    }

    // 校验CRC
    crc = RAD485_CalculateCRC(responseFrame, 5);
    if ((responseFrame[5] != (crc & 0xFF)) || (responseFrame[6] != ((crc >> 8) & 0xFF))) {
        return RAD485_ERROR_CRC;
    }

    // 解析响应数据
    if (responseFrame[0] != DEFAULT_DEVICE_ADDRESS || responseFrame[1] != MODBUS_READ_REGISTERS) {
        return RAD485_ERROR_INVALID_RESPONSE;
    }

    *radiationValue = (responseFrame[3] << 8) | responseFrame[4];
    return RAD485_ERROR_NONE;
}

// 计算CRC
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
