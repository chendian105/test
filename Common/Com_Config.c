#include "headlibs.h"

//������ݽṹ��
Com_Data_Status ComDataStruct;

RTC_HandleTypeDef hrtcnow;

//���ݽ�����ɱ�־λ
uint16_t send_data_flag = 0x0000;
//�豸�Ƿ�������־λ
uint16_t device_flag = 0x0000;

uint8_t debug_data = 0;
uint8_t dnq4v30_buf=0;  //�жϽ������ݻ���
uint8_t c12_buf=0;	
uint8_t asc_buf = 0;

//�������궨��
uint16_t COMMUNICATION_EXEC_CYCLE = 60*1000;

/* CRC16���� */
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
    for (int i = 0; i < sizeof(Com_Data_Status) - 2; i++) { // �ų�У�����֡β
        checksum ^= p[i];
    }
    return checksum;
}

//��ȡʱ���
void GetTimestampNow(char* timebuff, size_t bufferSize)
{
    RTC_TimeTypeDef sTime = {0};
    RTC_DateTypeDef sDate = {0};
    

		// ��ȡ��ǰʱ��
		HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
		HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

		// ��ʽ��ʱ��ΪYYYYMMDDHHmmSS
		snprintf(timebuff, bufferSize, "%04d%02d%02d%02d%02d%02d",
						 sDate.Year + 2000, sDate.Month, sDate.Date,
						 sTime.Hours, sTime.Minutes, sTime.Seconds);
		//printf("timebuff:%s",timebuff);
}


//�������
void pack_data(void) 
{
	ComDataStruct.header = SENDDATA_HEAD;   //֡ͷ
	//��ǰʱ��
	GetTimestampNow(ComDataStruct.timestamp,sizeof(ComDataStruct.timestamp));
	//У��
	ComDataStruct.checksum = calculate_checksum(&ComDataStruct);
	
	ComDataStruct.flags = device_flag;      //��־λ
	
	ComDataStruct.tail = SENDDATA_TIAL;     //֡β
}

// ��������
//void ComSendData(UART_HandleTypeDef *huart_sensor, uint8_t* data) 
//{
//	HAL_UART_Transmit(huart_sensor, (uint8_t*)data, sizeof(data), HAL_MAX_DELAY);
//}

// �����豸��Ӧ
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

/*�����жϻص�����*/
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	//printf("�жϻص�����\r\n");
	
	if(huart == &DEBUG_UART)
	{
		 if(Debug.rx_index >= 255)
		 {
			 memset(Debug.rx_buffer,0x00,sizeof(Debug.rx_buffer));//����������������
			 Debug.rx_index = 0;
		 }
		 else 
		 {
			 Debug.rx_buffer[Debug.rx_index++] = debug_data;
			 if((Debug.rx_buffer[Debug.rx_index-1] == 0x0A)&&(Debug.rx_buffer[Debug.rx_index-2]==0X0D))
			 {
				 //�жϺ������ܹ����������źŴ������ݽ�������
				 xSemaphoreGive(Debug.data_analysis);
			 }
		 }
		 // ���¿����ж�
     HAL_UART_Receive_IT(Debug.huart, &debug_data, 1);
	}
			
	
	if(huart == Dnq4v30.huart)
	{
		 if(Dnq4v30.rx_index >= 255)
		 {
			 memset(Dnq4v30.rx_buffer,0x00,sizeof(Dnq4v30.rx_buffer));//����������������
			 Dnq4v30.rx_index = 0;
		 }
		 else 
		 {
			 Dnq4v30.rx_buffer[Dnq4v30.rx_index++] = dnq4v30_buf;
			 if((Dnq4v30.rx_buffer[Dnq4v30.rx_index-1] == 0x0A)&&(Dnq4v30.rx_buffer[Dnq4v30.rx_index-2]==0X0D))
			 {
				 //�жϺ������ܹ����������źŴ������ݽ�������
				 xSemaphoreGive(Dnq4v30.data_analysis);
			 }
		 }
		 // ���¿����ж�
     HAL_UART_Receive_IT(Dnq4v30.huart, &dnq4v30_buf, 1);
	}
		
	if(huart == LaserC12.huart)
	{
		 if(LaserC12.rx_index >= 255)
		 {
			 memset(LaserC12.rx_buffer,0x00,sizeof(LaserC12.rx_buffer));//����������������
			 LaserC12.rx_index = 0;
		 }
		 else 
		 {
			 LaserC12.rx_buffer[LaserC12.rx_index++] = c12_buf;
			 if((LaserC12.rx_buffer[LaserC12.rx_index-1] == 0x0A)&&(LaserC12.rx_buffer[LaserC12.rx_index-2]==0X0D))
			 {
				 //�жϺ������ܹ����������źŴ������ݽ�������
				 xSemaphoreGive(LaserC12.data_analysis);
			 }
		 }
		 // ���¿����ж�
     HAL_UART_Receive_IT(LaserC12.huart, &c12_buf, 1);
	}
		
	if(huart == ASC200.huart)
	{
		 if(ASC200.rx_index >= 255)
		 {
			 memset(ASC200.rx_buffer,0x00,sizeof(ASC200.rx_buffer));//����������������
			 ASC200.rx_index = 0;
		 }
		 else 
		 {
			 ASC200.rx_buffer[ASC200.rx_index++] = asc_buf;
			 if((ASC200.rx_buffer[ASC200.rx_index-1] == 'E')&&(ASC200.rx_buffer[ASC200.rx_index-2]=='D'))
			 {
				 //�жϺ������ܹ����������źŴ������ݽ�������
				 xSemaphoreGive(ASC200.data_analysis);
			 }
		 }
		 // ���¿����ж�
     HAL_UART_Receive_IT(ASC200.huart, &asc_buf, 1);
	}
}

//void Com_Delay(uint32_t Delay)
//{
//	uint32_t _base, _delay;
//	_base = SystemCoreClock / 1000; /*1us�����ʱ�ӽ���*/
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

