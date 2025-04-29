#include "headlibs.h"

/* 数据解析任务 */
void C12_ProcessRawData( void * arg);
#define C12_DATA_PROCESS_TASK_NAME "DNQ_DATA_PROCESS"
#define C12_DATA_PROCESS_TASK_STACK_SIZE 256
#define C12_DATA_PROCESS_TASK_PRIORITY 10

// 设置云高仪的通讯协议模式
//DEVICE_Status_t CloudHeightSensor_SetProtocolMode(UART_HandleTypeDef *huart_sensor, ProtocolMode mode) {
//    char command[16];
//    switch (mode) {
//        case MLD0_MODE:
//            strcpy(command, "MLDSET,MLD0");
//            break;
//        case MLD1_MODE:
//            strcpy(command, "MLDSET,MLD1");
//            break;
//        case BOOK0_MODE:
//            strcpy(command, "MLDSET,BOOK0");
//            break;
//        case BOOK1_MODE:
//            strcpy(command, "MLDSET,BOOK1");
//            break;
//        default:
//            return DEVICE_ERR;
//    }
//    SendCommand(huart_sensor,command);

//    char response[16];
//    if (ReceiveResponse(huart_sensor,response, sizeof(response))) 
//		{
//        if (strcmp(response, "<T>") == 0) 
//				{
//            return DEVICE_OK;
//        }
//    }
//    return DEVICE_ERR;
//}

/*1. 设备初始化*/
DEVICE_Status_t LaserC12_Driver_Init(DEVICE_Handle_t *LaserC12, UART_HandleTypeDef *huart) 
{
	printf("云高仪初始化\r\n");
	
	if(LaserC12 == NULL || huart == NULL) return DEVICE_FRAME_ERR;
	
	/*1. 申请一块内存*/
  memset(LaserC12, 0, sizeof(DEVICE_Handle_t));
  LaserC12->huart = huart;
	
	/*2. 打开中断接收*/
	HAL_UART_Receive_IT(LaserC12->huart, &c12_buf, 1);
	
	
	/*3. 创建信号量：数据接收完成，触发数据解析*/
	LaserC12->data_processing = xSemaphoreCreateBinary();
	if (LaserC12->data_processing == NULL) {
			return DEVICE_FRAME_ERR; // 错误处理
	}
	
	/*4. 创建信号量：数据解析完成，触发数据转存*/
	LaserC12->data_analysis = xSemaphoreCreateBinary();
	if (LaserC12->data_analysis == NULL) {
			return DEVICE_FRAME_ERR; // 错误处理
	}
	
	/*5. 设置云高仪为主动发送模式*/
	//提前设置好
	
	/*6. 创建数据处理函数*/
	printf("创建数据处理任务\r\n");
	
	xTaskCreate(
		C12_ProcessRawData,
		C12_DATA_PROCESS_TASK_NAME,
		C12_DATA_PROCESS_TASK_STACK_SIZE,
		NULL,
		C12_DATA_PROCESS_TASK_PRIORITY,
		&LaserC12->processTaskHandle
	);

	return DEVICE_OK;
}

/* 协议解析实现 */
static DEVICE_Status_t MLD_ParseFrame(uint8_t *frame, MLD_Data_t *data) {
    char *token;
    uint16_t crc_cal, crc_recv;
    
    /* 验证起始标识 */
    if(strstr((char*)frame, "MLD") == NULL) return DEVICE_FRAME_ERR;
    
     // 分割字段
    token = strtok((char*)frame, " ");
    for (uint8_t i = 0; token != NULL && i < 21; i++) 
		{
        switch (i) {
            case 0:  // 日期
                strncpy(data->date, token, 10);
                data->date[10] = '\0';  // 确保字符串以NULL结尾
                break;
            case 1:  // 时间
                strncpy(data->time, token, 8);
                data->time[8] = '\0';  // 确保字符串以NULL结尾
                break;
            case 2:  // 设备ID
                data->deviceID = (uint16_t)atoi(token);
                break;
            case 3:  // 天空状态
                data->skyStatus = (uint8_t)atoi(token);
                break;
            case 4:  // 第一层云高
                data->cloudHeight[0] = (uint16_t)atoi(token);
                break;
            case 5:  // 第一层云厚
                data->cloudThickness[0] = (uint16_t)atoi(token);
                break;
            case 6:  // 第二层云高
                data->cloudHeight[1] = (uint16_t)atoi(token);
                break;
            case 7:  // 第二层云厚
                data->cloudThickness[1] = (uint16_t)atoi(token);
                break;
            case 8:  // 第三层云高
                data->cloudHeight[2] = (uint16_t)atoi(token);
                break;
            case 9:  // 第三层云厚
                data->cloudThickness[2] = (uint16_t)atoi(token);
                break;
            case 10: // 第四层云高
                data->cloudHeight[3] = (uint16_t)atoi(token);
                break;
            case 11: // 第四层云厚
                data->cloudThickness[3] = (uint16_t)atoi(token);
                break;
            case 12: // 第五层云高
                data->cloudHeight[4] = (uint16_t)atoi(token);
                break;
            case 13: // 第五层云厚
                data->cloudThickness[4] = (uint16_t)atoi(token);
                break;
            case 14: // 垂直能见度
                data->verticalVisibility = (uint16_t)atoi(token);
                break;
            case 15: // 低云云量
                data->lowCloudCover = (uint8_t)atoi(token);
                break;
            case 16: // 中云云量
                data->midCloudCover = (uint8_t)atoi(token);
                break;
            case 17: // 高云云量
                data->highCloudCover = (uint8_t)atoi(token);
                break;
            case 18: // 总云量
                data->totalCloudCover = (uint8_t)atoi(token);
                break;
            case 19: // 设备状态码
                data->statusCode = (uint16_t)strtoul(token, NULL, 2);  // 二进制转十进制
                break;
            case 20: // 和校验
                data->checksum = (uint16_t)atoi(token);
                break;
            default:
                break;
        }
        token = strtok(NULL, " ");
    }
 
    return DEVICE_OK;
}

/* 数据解析任务 */
static void C12_ProcessRawData(void *argument) 
{
	printf("数据解析任务启动\r\n");
	for(;;)
	{
		LaserC12.DataAnalysis = xSemaphoreTake(LaserC12.data_analysis,portMAX_DELAY);
		
		if(LaserC12.DataAnalysis==pdTRUE)
		{
			uint8_t frame[C12_FRAME_MAX_LENGTH];

			memcpy(&frame, LaserC12.rx_buffer, LaserC12.rx_index);
		  printf("数据解析任务收到信号量\r\n");
			if(MLD_ParseFrame(frame, &laserc12_data) == DEVICE_OK) 
			{
				
				xSemaphoreGive(LaserC12.data_processing);
			}
			
			LaserC12.rx_index = 0;
			memset(LaserC12.rx_buffer,0x00,sizeof(LaserC12.rx_buffer));//对数组进行清零操作
		}
	}
}

