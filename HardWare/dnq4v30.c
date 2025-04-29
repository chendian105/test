#include "headlibs.h"

/* 数据解析任务 */
void VIS_ProcessRawData( void * arg);
#define VIS_DATA_PROCESS_TASK_NAME "DNQ_DATA_PROCESS"
#define VIS_DATA_PROCESS_TASK_STACK_SIZE 256
#define VIS_DATA_PROCESS_TASK_PRIORITY 10



/*设备初始化*/
DEVICE_Status_t VIS_Driver_Init(DEVICE_Handle_t *Dnq4v30, UART_HandleTypeDef *huart) 
{
	printf("初始化：能见度仪\r\n");
	if(Dnq4v30 == NULL || huart == NULL) return DEVICE_FRAME_ERR;
	
	/*申请一块内存*/
  memset(Dnq4v30, 0, sizeof(DEVICE_Handle_t));
  Dnq4v30->huart = huart;
	
	/*1.打开中断接收*/
	HAL_UART_Receive_IT(Dnq4v30->huart, &dnq4v30_buf, 1);
	
	
	/*4. 创建信号量：数据接收完成，触发数据解析*/
	Dnq4v30->data_processing = xSemaphoreCreateBinary();
	if (Dnq4v30->data_processing == NULL) {
			return DEVICE_FRAME_ERR; // 错误处理
	}
	
		/*4. 创建信号量：数据解析完成，触发数据转存*/
	Dnq4v30->data_analysis = xSemaphoreCreateBinary();
	if (Dnq4v30->data_analysis == NULL) {
			return DEVICE_FRAME_ERR; // 错误处理
	}
	
	printf("创建数据处理任务\r\n");
	/* 创建数据处理任务 */
	xTaskCreate(
		VIS_ProcessRawData,
		VIS_DATA_PROCESS_TASK_NAME,
		VIS_DATA_PROCESS_TASK_STACK_SIZE,
		NULL,
		VIS_DATA_PROCESS_TASK_PRIORITY,
		&Dnq4v30->processTaskHandle
	);
	return DEVICE_OK;
}

/* 协议解析实现 */
static DEVICE_Status_t VIS_ParseFrame(uint8_t *frame, VIS_Data_t *data) {
    char *token;
    uint16_t crc_cal, crc_recv;
    
    /* 验证起始标识 */
    if(strncmp((char*)frame, "VIS", 3) != 0) return DEVICE_FRAME_ERR;
    
    /* 分割字段 */
    token = strtok((char*)frame, " ");
    for(uint8_t i=0; token != NULL && i<18; i++) {
        switch(i) {
            case 1:  /* 设备ID */
                data->device_id = atol(token);
                break;
            case 2:  /* 日期 */
                strncpy(data->date, token, 8);
                break;
            case 3:  /* 时间 */
                strncpy(data->time, token, 6);
                break;
            case 4:  /* 1分钟能见度 */
                data->visibility_1min = atof(token)/10.0;
                break;
            case 5:  /* 10分钟能见度 */
                data->visibility_10min = atof(token)/10.0;
                break;
            case 8:  /* 工作电压 */
                data->voltage = atof(token)/10.0;
                break;
            case 9:  /* 温度 */
                data->temperature = atof(token)/10.0;
                break;
            case 10:  /* 湿度 */
                data->humidity = atof(token)/10.0;
                break;
            case 11:  /* 天气现象代码 */
                data->weather_code = atol(token);
                break;
						case 15:  /* 设备状态码 */
								if (strcmp(token, "/") != 0) {
										data->status_code = strtoul(token, NULL, 16); // 解析16进制字符串
								} else {
										data->status_code = 0;
								}
                break;
            case 16: /* CRC校验 */
                crc_recv = (uint16_t)strtoul(token, NULL, 16);
                break;
        }
        token = strtok(NULL, " ");
    }
    
//    /* CRC校验 */
//    crc_cal = VIS_CRC16(frame, strlen((char*)frame)-5);
//    if(crc_cal != crc_recv) return VIS_CRC_ERR; 

		//printf("data->voltage:%f\r\n",data->voltage);
		
    return DEVICE_OK;
}

/* 数据解析任务 */
static void VIS_ProcessRawData(void *argument) 
{

	//printf("数据解析任务启动\r\n");
	for(;;)
	{
		Dnq4v30.DataAnalysis = xSemaphoreTake(Dnq4v30.data_analysis,portMAX_DELAY);
		if(Dnq4v30.DataAnalysis==pdTRUE)
		{
			//printf("开始解析\r\n");
			uint8_t frame[VIS_FRAME_MAX_LENGTH];

			memcpy(frame, Dnq4v30.rx_buffer, Dnq4v30.rx_index);
		
			if(VIS_ParseFrame(frame, &dnq4v30_data) == DEVICE_OK) 
			{
				xSemaphoreGive(Dnq4v30.data_processing);
			}
			
			Dnq4v30.rx_index = 0;
			//memset(Dnq4v30.rx_buffer,0x00,sizeof(Dnq4v30.rx_buffer));//对数组进行清零操作
		}
	}
}


