#include "headlibs.h"

#define COMMUNICATION_EXEC_CYCLE_ADDRESS 0x08000000  // Flash�洢��ַ������ʵ�����������

/*�豸��ʼ��*/
DEVICE_Status_t Debug_Driver_Init(DEVICE_Handle_t *Debug, UART_HandleTypeDef *huart) 
{
	printf("��ʼ�������Կ�\r\n");
	if(Debug == NULL || huart == NULL) return DEVICE_FRAME_ERR;
	
	/*����һ���ڴ�*/
  memset(Debug, 0, sizeof(DEVICE_Handle_t));
  Debug->huart = huart;
	
	/*1.���жϽ���*/
	HAL_UART_Receive_IT(Debug->huart, &dnq4v30_buf, 1);
	
	
	/*4. �����ź��������ݽ�����ɣ�������Ӧ�������*/
	Debug->data_processing = xSemaphoreCreateBinary();
	if (Debug->data_processing == NULL) {
			return DEVICE_FRAME_ERR; // ������
	}
	
	return DEVICE_OK;
}

// �������ݵ�Flash
void save_to_flash(uint32_t address, uint32_t data) {
    HAL_FLASH_Unlock();  // ����Flash

    FLASH_EraseInitTypeDef erase_init;
    uint32_t sector_error;

    erase_init.TypeErase = FLASH_TYPEERASE_PAGES;
    erase_init.PageAddress = address;
    erase_init.NbPages = 1;

    // ����Flashҳ
    if (HAL_FLASHEx_Erase(&erase_init, &sector_error) != HAL_OK) {
        // ������
    }

    // д������
    HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address, data);

    HAL_FLASH_Lock();  // ����Flash
}


// ��Flash��ȡ����
uint32_t read_from_flash(uint32_t address) {
    return *(__IO uint32_t *)address;
}

int8_t Debug_Receive_Process(char *command)
{

	if(command == NULL)
	{
		return -1;
	}

	if(strncmp((char *)command, "$ 01 ", 5)==0)
	{
		char *token = strtok(command + 5, " \r\n");  // ��ȡ����
		if(token != NULL)
		{
			uint32_t value = atoi(token);  // ���ַ���ת��Ϊ����
			COMMUNICATION_EXEC_CYCLE = value * 1000;
			// ���浽Flash
			save_to_flash(COMMUNICATION_EXEC_CYCLE_ADDRESS, COMMUNICATION_EXEC_CYCLE);
		}
	}
	return 1;
}

