#include "headlibs.h"

/* ���ݽ������� */
void VIS_ProcessRawData( void * arg);
#define VIS_DATA_PROCESS_TASK_NAME "DNQ_DATA_PROCESS"
#define VIS_DATA_PROCESS_TASK_STACK_SIZE 256
#define VIS_DATA_PROCESS_TASK_PRIORITY 10



/*�豸��ʼ��*/
DEVICE_Status_t VIS_Driver_Init(DEVICE_Handle_t *Dnq4v30, UART_HandleTypeDef *huart) 
{
	printf("��ʼ�����ܼ�����\r\n");
	if(Dnq4v30 == NULL || huart == NULL) return DEVICE_FRAME_ERR;
	
	/*����һ���ڴ�*/
  memset(Dnq4v30, 0, sizeof(DEVICE_Handle_t));
  Dnq4v30->huart = huart;
	
	/*1.���жϽ���*/
	HAL_UART_Receive_IT(Dnq4v30->huart, &dnq4v30_buf, 1);
	
	
	/*4. �����ź��������ݽ�����ɣ��������ݽ���*/
	Dnq4v30->data_processing = xSemaphoreCreateBinary();
	if (Dnq4v30->data_processing == NULL) {
			return DEVICE_FRAME_ERR; // ������
	}
	
		/*4. �����ź��������ݽ�����ɣ���������ת��*/
	Dnq4v30->data_analysis = xSemaphoreCreateBinary();
	if (Dnq4v30->data_analysis == NULL) {
			return DEVICE_FRAME_ERR; // ������
	}
	
	printf("�������ݴ�������\r\n");
	/* �������ݴ������� */
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

/* Э�����ʵ�� */
static DEVICE_Status_t VIS_ParseFrame(uint8_t *frame, VIS_Data_t *data) {
    char *token;
    uint16_t crc_cal, crc_recv;
    
    /* ��֤��ʼ��ʶ */
    if(strncmp((char*)frame, "VIS", 3) != 0) return DEVICE_FRAME_ERR;
    
    /* �ָ��ֶ� */
    token = strtok((char*)frame, " ");
    for(uint8_t i=0; token != NULL && i<18; i++) {
        switch(i) {
            case 1:  /* �豸ID */
                data->device_id = atol(token);
                break;
            case 2:  /* ���� */
                strncpy(data->date, token, 8);
                break;
            case 3:  /* ʱ�� */
                strncpy(data->time, token, 6);
                break;
            case 4:  /* 1�����ܼ��� */
                data->visibility_1min = atof(token)/10.0;
                break;
            case 5:  /* 10�����ܼ��� */
                data->visibility_10min = atof(token)/10.0;
                break;
            case 8:  /* ������ѹ */
                data->voltage = atof(token)/10.0;
                break;
            case 9:  /* �¶� */
                data->temperature = atof(token)/10.0;
                break;
            case 10:  /* ʪ�� */
                data->humidity = atof(token)/10.0;
                break;
            case 11:  /* ����������� */
                data->weather_code = atol(token);
                break;
						case 15:  /* �豸״̬�� */
								if (strcmp(token, "/") != 0) {
										data->status_code = strtoul(token, NULL, 16); // ����16�����ַ���
								} else {
										data->status_code = 0;
								}
                break;
            case 16: /* CRCУ�� */
                crc_recv = (uint16_t)strtoul(token, NULL, 16);
                break;
        }
        token = strtok(NULL, " ");
    }
    
//    /* CRCУ�� */
//    crc_cal = VIS_CRC16(frame, strlen((char*)frame)-5);
//    if(crc_cal != crc_recv) return VIS_CRC_ERR; 

		//printf("data->voltage:%f\r\n",data->voltage);
		
    return DEVICE_OK;
}

/* ���ݽ������� */
static void VIS_ProcessRawData(void *argument) 
{

	//printf("���ݽ�����������\r\n");
	for(;;)
	{
		Dnq4v30.DataAnalysis = xSemaphoreTake(Dnq4v30.data_analysis,portMAX_DELAY);
		if(Dnq4v30.DataAnalysis==pdTRUE)
		{
			//printf("��ʼ����\r\n");
			uint8_t frame[VIS_FRAME_MAX_LENGTH];

			memcpy(frame, Dnq4v30.rx_buffer, Dnq4v30.rx_index);
		
			if(VIS_ParseFrame(frame, &dnq4v30_data) == DEVICE_OK) 
			{
				xSemaphoreGive(Dnq4v30.data_processing);
			}
			
			Dnq4v30.rx_index = 0;
			//memset(Dnq4v30.rx_buffer,0x00,sizeof(Dnq4v30.rx_buffer));//����������������
		}
	}
}


