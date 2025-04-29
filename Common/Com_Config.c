#include "headlibs.h"

//输出数据结构体
Com_Data_Status ComDataStruct;

RTC_HandleTypeDef hrtcnow;

//数据接收完成标志位
uint16_t send_data_flag = 0x0000;
//设备是否正常标志位
uint16_t device_flag = 0x0000;

uint8_t debug_data = 0;
uint8_t dnq4v30_buf=0;  //中断接收数据缓存
uint8_t c12_buf=0;	
uint8_t asc_buf = 0;

//传输间隔宏定义
uint16_t COMMUNICATION_EXEC_CYCLE = 60*1000;

/* CRC16计算 */
uint16_t VIS_CRC16(const uint8_t *data, uint16_t length) {
    uint16_t crc = 0xFFFF;
    for(uint16_t i=0; i<length; i++) {
        crc ^= (uint16_t)data[i];
        for(uint8_t j=0; j<8; j++) {
            if(crc & 0x0001)
                crc = (crc >> 1) ^ 0xA001;
            else
                crc >>= 1;
        }
    }
    return crc;
}

//
uint8_t calculate_checksum(Com_Data_Status *packet) {
    uint8_t checksum = 0;
    uint8_t *p = (uint8_t *)packet;
    for (int i = 0; i < sizeof(Com_Data_Status) - 2; i++) { // 排除校验码和帧尾
        checksum ^= p[i];
    }
    return checksum;
}

//获取时间戳
void GetTimestampNow(char* timebuff, size_t bufferSize)
{
    RTC_TimeTypeDef sTime = {0};
    RTC_DateTypeDef sDate = {0};
    

		// 获取当前时间
		HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
		HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

		// 格式化时间为YYYYMMDDHHmmSS
		snprintf(timebuff, bufferSize, "%04d%02d%02d%02d%02d%02d",
						 sDate.Year + 2000, sDate.Month, sDate.Date,
						 sTime.Hours, sTime.Minutes, sTime.Seconds);
		//printf("timebuff:%s",timebuff);
}


//打包数据
void pack_data(void) 
{
	ComDataStruct.header = SENDDATA_HEAD;   //帧头
	//当前时间
	GetTimestampNow(ComDataStruct.timestamp,sizeof(ComDataStruct.timestamp));
	//校验
	ComDataStruct.checksum = calculate_checksum(&ComDataStruct);
	
	ComDataStruct.flags = device_flag;      //标志位
	
	ComDataStruct.tail = SENDDATA_TIAL;     //帧尾
}

// 发送数据
//void ComSendData(UART_HandleTypeDef *huart_sensor, uint8_t* data) 
//{
//	HAL_UART_Transmit(huart_sensor, (uint8_t*)data, sizeof(data), HAL_MAX_DELAY);
//}

// 接收设备响应
DEVICE_Status_t ReceiveResponse(UART_HandleTypeDef *huart_sensor,char* response, uint16_t maxLen) {
    uint16_t index = 0;
    while (index < maxLen - 1) {
        if (HAL_UART_Receive(huart_sensor, (uint8_t*)&response[index], 1, HAL_MAX_DELAY) == HAL_OK) {
            if (response[index] == '\r') 
						{
                response[index] = '\0';
                return DEVICE_OK;
            }
            index++;
        }
    }
    return DEVICE_ERR;
}

/*串口中断回调函数*/
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	//printf("中断回调函数\r\n");
	
	if(huart == &DEBUG_UART)
	{
		 if(Debug.rx_index >= 255)
		 {
			 memset(Debug.rx_buffer,0x00,sizeof(Debug.rx_buffer));//对数组进行清零操作
			 Debug.rx_index = 0;
		 }
		 else 
		 {
			 Debug.rx_buffer[Debug.rx_index++] = debug_data;
			 if((Debug.rx_buffer[Debug.rx_index-1] == 0x0A)&&(Debug.rx_buffer[Debug.rx_index-2]==0X0D))
			 {
				 //中断函数不能过长，发送信号触发数据解析任务
				 xSemaphoreGive(Debug.data_analysis);
			 }
		 }
		 // 重新开启中断
     HAL_UART_Receive_IT(Debug.huart, &debug_data, 1);
	}
			
	
	if(huart == Dnq4v30.huart)
	{
		 if(Dnq4v30.rx_index >= 255)
		 {
			 memset(Dnq4v30.rx_buffer,0x00,sizeof(Dnq4v30.rx_buffer));//对数组进行清零操作
			 Dnq4v30.rx_index = 0;
		 }
		 else 
		 {
			 Dnq4v30.rx_buffer[Dnq4v30.rx_index++] = dnq4v30_buf;
			 if((Dnq4v30.rx_buffer[Dnq4v30.rx_index-1] == 0x0A)&&(Dnq4v30.rx_buffer[Dnq4v30.rx_index-2]==0X0D))
			 {
				 //中断函数不能过长，发送信号触发数据解析任务
				 xSemaphoreGive(Dnq4v30.data_analysis);
			 }
		 }
		 // 重新开启中断
     HAL_UART_Receive_IT(Dnq4v30.huart, &dnq4v30_buf, 1);
	}
		
	if(huart == LaserC12.huart)
	{
		 if(LaserC12.rx_index >= 255)
		 {
			 memset(LaserC12.rx_buffer,0x00,sizeof(LaserC12.rx_buffer));//对数组进行清零操作
			 LaserC12.rx_index = 0;
		 }
		 else 
		 {
			 LaserC12.rx_buffer[LaserC12.rx_index++] = c12_buf;
			 if((LaserC12.rx_buffer[LaserC12.rx_index-1] == 0x0A)&&(LaserC12.rx_buffer[LaserC12.rx_index-2]==0X0D))
			 {
				 //中断函数不能过长，发送信号触发数据解析任务
				 xSemaphoreGive(LaserC12.data_analysis);
			 }
		 }
		 // 重新开启中断
     HAL_UART_Receive_IT(LaserC12.huart, &c12_buf, 1);
	}
		
	if(huart == ASC200.huart)
	{
		 if(ASC200.rx_index >= 255)
		 {
			 memset(ASC200.rx_buffer,0x00,sizeof(ASC200.rx_buffer));//对数组进行清零操作
			 ASC200.rx_index = 0;
		 }
		 else 
		 {
			 ASC200.rx_buffer[ASC200.rx_index++] = asc_buf;
			 if((ASC200.rx_buffer[ASC200.rx_index-1] == 'E')&&(ASC200.rx_buffer[ASC200.rx_index-2]=='D'))
			 {
				 //中断函数不能过长，发送信号触发数据解析任务
				 xSemaphoreGive(ASC200.data_analysis);
			 }
		 }
		 // 重新开启中断
     HAL_UART_Receive_IT(ASC200.huart, &asc_buf, 1);
	}
}

//void Com_Delay(uint32_t Delay)
//{
//	uint32_t _base, _delay;
//	_base = SystemCoreClock / 1000; /*1us所需的时钟节拍*/
//	_delay = _base * Delay;

//	while(_delay--)
//	{
//		__nop();
//	}
//}

int fputc(int c, FILE *file)
{
	HAL_UART_Transmit(&huart1,(uint8_t *)&c,1,1000);
	return c;
}

