#include "headlibs.h"

#define COMMUNICATION_EXEC_CYCLE_ADDRESS 0x08000000  // Flash存储地址（根据实际情况调整）

/*设备初始化*/
DEVICE_Status_t Debug_Driver_Init(DEVICE_Handle_t *Debug, UART_HandleTypeDef *huart) 
{
	printf("初始化：调试口\r\n");
	if(Debug == NULL || huart == NULL) return DEVICE_FRAME_ERR;
	
	/*申请一块内存*/
  memset(Debug, 0, sizeof(DEVICE_Handle_t));
  Debug->huart = huart;
	
	/*1.打开中断接收*/
	HAL_UART_Receive_IT(Debug->huart, &dnq4v30_buf, 1);
	
	
	/*4. 创建信号量：数据接收完成，触发对应的命令函数*/
	Debug->data_processing = xSemaphoreCreateBinary();
	if (Debug->data_processing == NULL) {
			return DEVICE_FRAME_ERR; // 错误处理
	}
	
	return DEVICE_OK;
}

// 保存数据到Flash
void save_to_flash(uint32_t address, uint32_t data) {
    HAL_FLASH_Unlock();  // 解锁Flash

    FLASH_EraseInitTypeDef erase_init;
    uint32_t sector_error;

    erase_init.TypeErase = FLASH_TYPEERASE_PAGES;
    erase_init.PageAddress = address;
    erase_init.NbPages = 1;

    // 擦除Flash页
    if (HAL_FLASHEx_Erase(&erase_init, &sector_error) != HAL_OK) {
        // 错误处理
    }

    // 写入数据
    HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address, data);

    HAL_FLASH_Lock();  // 锁定Flash
}


// 从Flash读取数据
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
		char *token = strtok(command + 5, " \r\n");  // 提取参数
		if(token != NULL)
		{
			uint32_t value = atoi(token);  // 将字符串转换为整数
			COMMUNICATION_EXEC_CYCLE = value * 1000;
			// 保存到Flash
			save_to_flash(COMMUNICATION_EXEC_CYCLE_ADDRESS, COMMUNICATION_EXEC_CYCLE);
		}
	}
	return 1;
}

