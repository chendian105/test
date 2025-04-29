#ifndef __DEBUG_H
#define __DEBUG_H

#include "Com_Config.h"

DEVICE_Status_t Debug_Driver_Init(DEVICE_Handle_t *Debug, UART_HandleTypeDef *huart);
int8_t Debug_Receive_Process(char *command);

#endif
