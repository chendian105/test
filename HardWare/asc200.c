/********************************����********************************/

//BG,123456,04,RWIS,001,20231010120000,001,03,02,0A,01000,1,1A,02000,1,2A,03000,1,0B,0080,0,1B,0050,0,2B,0030,0,ZD1A,1,ZD2A,0,1234,ED

#include "headlibs.h"
//#include "timers.h"


// ���崮�ھ��
extern UART_HandleTypeDef huart1;


// ��ʱ���ص�����
void vTimerCallback(TimerHandle_t xTimer) {
    //DataFrame dataFrame;

    // ���Ͷ�ȡ��������
    ASC_SendCommand("READDATA");

    // ��������
    //ASC_ReceiveData(&asc200_data);

    // ��������
    //ASC_ProcessData(&dataFrame);
}

// ��ʼ������
DEVICE_Status_t ASC_Init(PASVDEVICE_Handle_t *ASC200,UART_HandleTypeDef *huart) {
	
		printf("������״��ʼ��\r\n");
	
		if(ASC200 == NULL || huart == NULL) return DEVICE_FRAME_ERR;
		
		/*1. ����һ���ڴ�*/
		memset(ASC200, 0, sizeof(PASVDEVICE_Handle_t));
		ASC200->huart = huart;
	
	  /*2.��������,��ʱ�������ݲɼ�*/
		ASC200->TimerHandle_t = xTimerCreate(
															"HourlyTimer",              // ��ʱ������
															pdMS_TO_TICKS(5 * 1000),    // ��ʱ�����ڣ�1Сʱ��
															pdTRUE,                     // �Զ�����
															(void*)0,                   // ��ʱ��ID
															vTimerCallback              // ��ʱ���ص�����
    );
		// ������ʱ��
    if (ASC200->TimerHandle_t != NULL) {
        xTimerStart(ASC200->TimerHandle_t, 0);
    }
		
		/*3. �����ź��������ݽ�����ɣ���������ת��*/
		ASC200->data_processing = xSemaphoreCreateBinary();
		if (ASC200->data_processing == NULL) {
				return DEVICE_FRAME_ERR; // ������
		}
		return DEVICE_OK;
}

// ���������
void ASC_SendCommand(const char* command) {
    // ����Ϊ����ģʽ
    //HAL_GPIO_WritePin(DE_RE_GPIO_PORT, DE_RE_PIN, GPIO_PIN_SET);

    // ��������
    HAL_UART_Transmit(&huart2, (uint8_t*)command, strlen(command), HAL_MAX_DELAY);

    // ����Ϊ����ģʽ
    //HAL_GPIO_WritePin(DE_RE_GPIO_PORT, DE_RE_PIN, GPIO_PIN_RESET);
}

// �������ݺ���
void ASC_ReceiveData(ASC200_DataFrame* dataFrame) {
    char buffer[MAX_FRAME_LENGTH];
    //uint16_t length = 0;

    // ��������
		printf("��ʼ���յ�������״����\r\n");
		HAL_Delay(100);
    HAL_UART_Receive(&huart2, (uint8_t*)buffer, MAX_FRAME_LENGTH, 2000);
	//printf("strlen(buffer):%d\r\n",strlen(buffer));
		printf("%s\r\n",buffer);
		
    // �������֡����ʼ�ͽ�����ʶ
    if (strncmp(buffer, FRAME_START, 2) == 0 && strncmp(buffer + strlen(buffer) - 2, FRAME_END, 2) == 0) {
				printf("���յ�������״����\r\n");
        // ��������֡
        memcpy(dataFrame->start, buffer, 2);
        memcpy(dataFrame->stationID, buffer + 2, 6);
        memcpy(dataFrame->serviceType, buffer + 8, 2);
        memcpy(dataFrame->deviceID, buffer + 10, 4);
        memcpy(dataFrame->deviceNumber, buffer + 14, 3);
        memcpy(dataFrame->timestamp, buffer + 17, 14);
        memcpy(dataFrame->frameFlag, buffer + 31, 3);
        memcpy(dataFrame->observationVariableCount, buffer + 34, 2);
        memcpy(dataFrame->statusVariableCount, buffer + 36, 2);

        // �����۲�Ҫ��
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

        // ����״̬����
        int statusCount = atoi(dataFrame->statusVariableCount);
        for (int i = 0; i < statusCount; i++) {
            memcpy(dataFrame->statusVariables[i].statusName, buffer + offset, 4);
            offset += 4;
            dataFrame->statusVariables[i].statusValue = atoi(buffer + offset);
            offset += 1;
        }

        // ����У����
        memcpy(dataFrame->checksum, buffer + offset, CHECKSUM_LENGTH);
        offset += CHECKSUM_LENGTH;

        // ����������ʶ
        memcpy(dataFrame->end, buffer + offset, 2);
				
				//�����ź������������ݴ���
				xSemaphoreGive(ASC200.data_processing);
    }
		//�������
		memset(&buffer,0x00,MAX_FRAME_LENGTH);//����������������
}

// �������ݺ���
void ASC_ProcessData(ASC200_DataFrame* dataFrame) {
    // �����ﴦ����յ�������
    // ���磺��ӡ�۲�Ҫ�غ�״̬����
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

// ����У��ͺ���
uint16_t ASC_CalculateChecksum(const char* data, uint16_t length) {
    uint16_t checksum = 0;
    for (uint16_t i = 0; i < length; i++) {
        checksum += data[i];
    }
    return checksum & 0xFFFF;
}




