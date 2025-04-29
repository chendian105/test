/********************************云量********************************/

//BG,123456,04,RWIS,001,20231010120000,001,03,02,0A,01000,1,1A,02000,1,2A,03000,1,0B,0080,0,1B,0050,0,2B,0030,0,ZD1A,1,ZD2A,0,1234,ED

#include "headlibs.h"
//#include "timers.h"


// 定义串口句柄
extern UART_HandleTypeDef huart1;


// 定时器回调函数
void vTimerCallback(TimerHandle_t xTimer) {
    //DataFrame dataFrame;

    // 发送读取数据命令
    ASC_SendCommand("READDATA");

    // 接收数据
    //ASC_ReceiveData(&asc200_data);

    // 处理数据
    //ASC_ProcessData(&dataFrame);
}

// 初始化函数
DEVICE_Status_t ASC_Init(PASVDEVICE_Handle_t *ASC200,UART_HandleTypeDef *huart) {
	
		printf("云量云状初始化\r\n");
	
		if(ASC200 == NULL || huart == NULL) return DEVICE_FRAME_ERR;
		
		/*1. 申请一块内存*/
		memset(ASC200, 0, sizeof(PASVDEVICE_Handle_t));
		ASC200->huart = huart;
	
	  /*2.创建闹钟,定时触发数据采集*/
		ASC200->TimerHandle_t = xTimerCreate(
															"HourlyTimer",              // 定时器名称
															pdMS_TO_TICKS(5 * 1000),    // 定时器周期（1小时）
															pdTRUE,                     // 自动重载
															(void*)0,                   // 定时器ID
															vTimerCallback              // 定时器回调函数
    );
		// 启动定时器
    if (ASC200->TimerHandle_t != NULL) {
        xTimerStart(ASC200->TimerHandle_t, 0);
    }
		
		/*3. 创建信号量：数据解析完成，触发数据转存*/
		ASC200->data_processing = xSemaphoreCreateBinary();
		if (ASC200->data_processing == NULL) {
				return DEVICE_FRAME_ERR; // 错误处理
		}
		return DEVICE_OK;
}

// 发送命令函数
void ASC_SendCommand(const char* command) {
    // 设置为发送模式
    //HAL_GPIO_WritePin(DE_RE_GPIO_PORT, DE_RE_PIN, GPIO_PIN_SET);

    // 发送命令
    HAL_UART_Transmit(&huart2, (uint8_t*)command, strlen(command), HAL_MAX_DELAY);

    // 设置为接收模式
    //HAL_GPIO_WritePin(DE_RE_GPIO_PORT, DE_RE_PIN, GPIO_PIN_RESET);
}

// 接收数据函数
void ASC_ReceiveData(ASC200_DataFrame* dataFrame) {
    char buffer[MAX_FRAME_LENGTH];
    //uint16_t length = 0;

    // 接收数据
		printf("开始接收到云量云状数据\r\n");
		HAL_Delay(100);
    HAL_UART_Receive(&huart2, (uint8_t*)buffer, MAX_FRAME_LENGTH, 2000);
	//printf("strlen(buffer):%d\r\n",strlen(buffer));
		printf("%s\r\n",buffer);
		
    // 检查数据帧的起始和结束标识
    if (strncmp(buffer, FRAME_START, 2) == 0 && strncmp(buffer + strlen(buffer) - 2, FRAME_END, 2) == 0) {
				printf("接收到云量云状数据\r\n");
        // 解析数据帧
        memcpy(dataFrame->start, buffer, 2);
        memcpy(dataFrame->stationID, buffer + 2, 6);
        memcpy(dataFrame->serviceType, buffer + 8, 2);
        memcpy(dataFrame->deviceID, buffer + 10, 4);
        memcpy(dataFrame->deviceNumber, buffer + 14, 3);
        memcpy(dataFrame->timestamp, buffer + 17, 14);
        memcpy(dataFrame->frameFlag, buffer + 31, 3);
        memcpy(dataFrame->observationVariableCount, buffer + 34, 2);
        memcpy(dataFrame->statusVariableCount, buffer + 36, 2);

        // 解析观测要素
        int observationCount = atoi(dataFrame->observationVariableCount);
        int offset = 38;
        for (int i = 0; i < observationCount; i++) {
            memcpy(dataFrame->observationVariables[i].variableName, buffer + offset, 4);
            offset += 4;
            dataFrame->observationVariables[i].variableValue = atoi(buffer + offset);
            offset += 5;
            dataFrame->observationVariables[i].qualityControl = atoi(buffer + offset);
            offset += 1;
        }

        // 解析状态变量
        int statusCount = atoi(dataFrame->statusVariableCount);
        for (int i = 0; i < statusCount; i++) {
            memcpy(dataFrame->statusVariables[i].statusName, buffer + offset, 4);
            offset += 4;
            dataFrame->statusVariables[i].statusValue = atoi(buffer + offset);
            offset += 1;
        }

        // 解析校验码
        memcpy(dataFrame->checksum, buffer + offset, CHECKSUM_LENGTH);
        offset += CHECKSUM_LENGTH;

        // 解析结束标识
        memcpy(dataFrame->end, buffer + offset, 2);
				
				//发送信号量，触发数据处理
				xSemaphoreGive(ASC200.data_processing);
    }
		//清除数组
		memset(&buffer,0x00,MAX_FRAME_LENGTH);//对数组进行清零操作
}

// 处理数据函数
void ASC_ProcessData(ASC200_DataFrame* dataFrame) {
    // 在这里处理接收到的数据
    // 例如：打印观测要素和状态变量
    for (int i = 0; i < atoi(dataFrame->observationVariableCount); i++) {
        printf("Observation Variable %s: %d, Quality Control: %d\n",
               dataFrame->observationVariables[i].variableName,
               dataFrame->observationVariables[i].variableValue,
               dataFrame->observationVariables[i].qualityControl);
    }

    for (int i = 0; i < atoi(dataFrame->statusVariableCount); i++) {
        printf("Status Variable %s: %d\n",
               dataFrame->statusVariables[i].statusName,
               dataFrame->statusVariables[i].statusValue);
    }
}

// 计算校验和函数
uint16_t ASC_CalculateChecksum(const char* data, uint16_t length) {
    uint16_t checksum = 0;
    for (uint16_t i = 0; i < length; i++) {
        checksum += data[i];
    }
    return checksum & 0xFFFF;
}




