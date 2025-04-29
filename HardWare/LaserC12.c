#include "headlibs.h"

/* ���ݽ������� */
void C12_ProcessRawData( void * arg);
#define C12_DATA_PROCESS_TASK_NAME "DNQ_DATA_PROCESS"
#define C12_DATA_PROCESS_TASK_STACK_SIZE 256
#define C12_DATA_PROCESS_TASK_PRIORITY 10

// �����Ƹ��ǵ�ͨѶЭ��ģʽ
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

/*1. �豸��ʼ��*/
DEVICE_Status_t LaserC12_Driver_Init(DEVICE_Handle_t *LaserC12, UART_HandleTypeDef *huart) 
{
	printf("�Ƹ��ǳ�ʼ��\r\n");
	
	if(LaserC12 == NULL || huart == NULL) return DEVICE_FRAME_ERR;
	
	/*1. ����һ���ڴ�*/
  memset(LaserC12, 0, sizeof(DEVICE_Handle_t));
  LaserC12->huart = huart;
	
	/*2. ���жϽ���*/
	HAL_UART_Receive_IT(LaserC12->huart, &c12_buf, 1);
	
	
	/*3. �����ź��������ݽ�����ɣ��������ݽ���*/
	LaserC12->data_processing = xSemaphoreCreateBinary();
	if (LaserC12->data_processing == NULL) {
			return DEVICE_FRAME_ERR; // ������
	}
	
	/*4. �����ź��������ݽ�����ɣ���������ת��*/
	LaserC12->data_analysis = xSemaphoreCreateBinary();
	if (LaserC12->data_analysis == NULL) {
			return DEVICE_FRAME_ERR; // ������
	}
	
	/*5. �����Ƹ���Ϊ��������ģʽ*/
	//��ǰ���ú�
	
	/*6. �������ݴ�����*/
	printf("�������ݴ�������\r\n");
	
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

/* Э�����ʵ�� */
static DEVICE_Status_t MLD_ParseFrame(uint8_t *frame, MLD_Data_t *data) {
    char *token;
    uint16_t crc_cal, crc_recv;
    
    /* ��֤��ʼ��ʶ */
    if(strstr((char*)frame, "MLD") == NULL) return DEVICE_FRAME_ERR;
    
     // �ָ��ֶ�
    token = strtok((char*)frame, " ");
    for (uint8_t i = 0; token != NULL && i < 21; i++) 
		{
        switch (i) {
            case 0:  // ����
                strncpy(data->date, token, 10);
                data->date[10] = '\0';  // ȷ���ַ�����NULL��β
                break;
            case 1:  // ʱ��
                strncpy(data->time, token, 8);
                data->time[8] = '\0';  // ȷ���ַ�����NULL��β
                break;
            case 2:  // �豸ID
                data->deviceID = (uint16_t)atoi(token);
                break;
            case 3:  // ���״̬
                data->skyStatus = (uint8_t)atoi(token);
                break;
            case 4:  // ��һ���Ƹ�
                data->cloudHeight[0] = (uint16_t)atoi(token);
                break;
            case 5:  // ��һ���ƺ�
                data->cloudThickness[0] = (uint16_t)atoi(token);
                break;
            case 6:  // �ڶ����Ƹ�
                data->cloudHeight[1] = (uint16_t)atoi(token);
                break;
            case 7:  // �ڶ����ƺ�
                data->cloudThickness[1] = (uint16_t)atoi(token);
                break;
            case 8:  // �������Ƹ�
                data->cloudHeight[2] = (uint16_t)atoi(token);
                break;
            case 9:  // �������ƺ�
                data->cloudThickness[2] = (uint16_t)atoi(token);
                break;
            case 10: // ���Ĳ��Ƹ�
                data->cloudHeight[3] = (uint16_t)atoi(token);
                break;
            case 11: // ���Ĳ��ƺ�
                data->cloudThickness[3] = (uint16_t)atoi(token);
                break;
            case 12: // ������Ƹ�
                data->cloudHeight[4] = (uint16_t)atoi(token);
                break;
            case 13: // ������ƺ�
                data->cloudThickness[4] = (uint16_t)atoi(token);
                break;
            case 14: // ��ֱ�ܼ���
                data->verticalVisibility = (uint16_t)atoi(token);
                break;
            case 15: // ��������
                data->lowCloudCover = (uint8_t)atoi(token);
                break;
            case 16: // ��������
                data->midCloudCover = (uint8_t)atoi(token);
                break;
            case 17: // ��������
                data->highCloudCover = (uint8_t)atoi(token);
                break;
            case 18: // ������
                data->totalCloudCover = (uint8_t)atoi(token);
                break;
            case 19: // �豸״̬��
                data->statusCode = (uint16_t)strtoul(token, NULL, 2);  // ������תʮ����
                break;
            case 20: // ��У��
                data->checksum = (uint16_t)atoi(token);
                break;
            default:
                break;
        }
        token = strtok(NULL, " ");
    }
 
    return DEVICE_OK;
}

/* ���ݽ������� */
static void C12_ProcessRawData(void *argument) 
{
	printf("���ݽ�����������\r\n");
	for(;;)
	{
		LaserC12.DataAnalysis = xSemaphoreTake(LaserC12.data_analysis,portMAX_DELAY);
		
		if(LaserC12.DataAnalysis==pdTRUE)
		{
			uint8_t frame[C12_FRAME_MAX_LENGTH];

			memcpy(&frame, LaserC12.rx_buffer, LaserC12.rx_index);
		  printf("���ݽ��������յ��ź���\r\n");
			if(MLD_ParseFrame(frame, &laserc12_data) == DEVICE_OK) 
			{
				
				xSemaphoreGive(LaserC12.data_processing);
			}
			
			LaserC12.rx_index = 0;
			memset(LaserC12.rx_buffer,0x00,sizeof(LaserC12.rx_buffer));//����������������
		}
	}
}

