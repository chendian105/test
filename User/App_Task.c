#include "headlibs.h"

/************ʵ�����豸���ƽṹ��****************/
DEVICE_Handle_t Debug;     //ʵ�������Կڿ��ƽṹ��
DEVICE_Handle_t Dnq4v30;   //ʵ�����ܼ������������ƽṹ��
DEVICE_Handle_t LaserC12;  //ʵ�����Ƹ����������ƽṹ��
PASVDEVICE_Handle_t ASC200; //ʵ����������״�������ƽṹ��


/***************�豸���ݽṹ��*******************/
VIS_Data_t dnq4v30_data;   //�ܼ��������ݽṹ��
MLD_Data_t laserc12_data;  //�Ƹ������ݽṹ��
uint16_t   tbq2rad485_data;//̫�������
ASC200_DataFrame asc200_data; //������״���ݽṹ��



/*******************����************************/
/*��������*/
void StartTaskFunction( void * arg);
#define START_TASK_NAME "StartTask"
#define START_TASK_STACK 128
#define START_TASK_PRIORITY 10
TaskHandle_t StartTaskHandle;


/*0. ��������*/
void DebugTaskFunction( void * arg);
#define DEBUG_TASK_NAME "DebugTask"
#define DEBUG_TASK_STACK 128
#define DEBUG_TASK_PRIORITY 10
TaskHandle_t DebugTaskHandle;
SemaphoreHandle_t DebugSemaphore;        //���Կ�ָ������ź���
BaseType_t DebugBaseType;                //

/*1. dnq4v30����*/
void Dnq4v30Task( void * arg);
#define DNQ4V30_TASK_NAME "Dnq4v30Task"
#define DNQ4V30_TASK_STACK_SIZE 256
#define DNQ4V30_TASK_PRIORITY 9
TaskHandle_t Dnq4v30TaskHandle;

/*2. �Ƹ�������*/
void LaserC12Task( void * arg);
#define LASERC12_TASK_NAME "LaserC12Task"
#define LASERC12_TASK_STACK_SIZE 256
#define LASERC12_TASK_PRIORITY 9
TaskHandle_t LaserC12TaskHandle;

/*3. ̫�����������*/
void TBQ2485Task( void * arg);
#define TBQ2485_TASK_NAME "Tbq2485Task"
#define TBQ2485_TASK_STACK_SIZE 256
#define TBQ2485_TASK_PRIORITY 9
TaskHandle_t Tbq2485TaskHandle;

/*4. ������״����������*/
void ASC200Task( void * arg);
#define ASC200_TASK_NAME "Asc200Task"
#define ASC200_TASK_STACK_SIZE 256
#define ASC200_TASK_PRIORITY 9
TaskHandle_t Asc200TaskHandle;

/*���ݷ�������*/
void SendDataTask( void * arg);
#define SENDDATA_TASK_NAME "DataSendTask"
#define SENDDATA_TASK_STACK_SIZE 256
#define SENDDATA_TASK_PRIORITY 10
TaskHandle_t SendDataHandle;


void App_Task_FreeRTOSStart(void)
{
	/*1. ��ʼ��*/

	printf("==========start===========\r\n");

	/*2. ����һ���������������������д�����ҵ����ص�����*/
	BaseType_t r = xTaskCreate(StartTaskFunction,  /*����������*/
														START_TASK_NAME,     /*������*/
														START_TASK_STACK,    /*ջ��С����λ���֣������Լ����ֵĴ�С��32λ�ܹ���4 START_TASK_STACK*4=�ֽ���*/
														NULL,                /*ָ�������������ݲ���*/
														START_TASK_PRIORITY, /*�������ȼ�*/
														&StartTaskHandle);   /*������*/
	
	if(r==pdPASS)
	{
		printf("�������񴴽��ɹ�\r\n");
	}
	else
	{
		printf("�������񴴽�ʧ��\r\n");	
	}
	
	/*3. ����������*/
	vTaskStartScheduler();
}

/*��������*/
void StartTaskFunction( void * arg)
{
	printf("��ʼ����\r\n");

//	//����һ����ʱ��
//	    xTimerHandle = xTimerCreate(
//        "HourlyTimer",              // ��ʱ������
//        pdMS_TO_TICKS(3600 * 1000), // ��ʱ�����ڣ�1Сʱ��
//        pdTRUE,                    // �Զ�����
//        (void*)0,                   // ��ʱ��ID
//        vTimerCallback              // ��ʱ���ص�����
//    );
//	
	
	/*�����¼���*/
	
	/*��������*/
	xTaskCreate(DebugTaskFunction,
							DEBUG_TASK_NAME,
							DEBUG_TASK_STACK,
							NULL,
							DEBUG_TASK_PRIORITY,
							&DebugTaskHandle);	

	/*1.DNQ4V30���ܼ��ȣ��豸����*/
	xTaskCreate(Dnq4v30Task,
							DNQ4V30_TASK_NAME,
							DNQ4V30_TASK_STACK_SIZE,
							NULL,
							DNQ4V30_TASK_PRIORITY,
							&Dnq4v30TaskHandle);
	
//	/*2.C12���Ƹ��ǣ��豸����*/
//	xTaskCreate(LaserC12Task,
//							LASERC12_TASK_NAME,
//							LASERC12_TASK_STACK_SIZE,
//							NULL,
//							LASERC12_TASK_PRIORITY,
//							&LaserC12TaskHandle);
	
//	/*3.̫�������*/
//	xTaskCreate(TBQ2485Task,
//							TBQ2485_TASK_NAME,
//							TBQ2485_TASK_STACK_SIZE,
//							NULL,
//							TBQ2485_TASK_PRIORITY,
//							&Tbq2485TaskHandle);
							
//	/*4.������״*/
//	xTaskCreate(ASC200Task,
//							ASC200_TASK_NAME,
//							ASC200_TASK_STACK_SIZE,
//							NULL,
//							ASC200_TASK_PRIORITY,
//							&Asc200TaskHandle);
	
	/*���ݷ���*/
	xTaskCreate(SendDataTask,
							SENDDATA_TASK_NAME,
							SENDDATA_TASK_STACK_SIZE,
							NULL,
							SENDDATA_TASK_PRIORITY,
							&SendDataHandle);	
	
	/*ɾ���Լ�*/
	vTaskDelete(NULL);
	printf("����ִ��\r\n");
}

//-----------------------------------------------------------------------------
// ָ�����������
//-----------------------------------------------------------------------------
static void DebugTaskFunction(void *argument) 
{
	Debug_Driver_Init(&Debug,&DEBUG_UART);

	while(1)
	{
		Debug.DataProcess = xSemaphoreTake(Debug.data_processing,portMAX_DELAY);
		if(Debug.DataProcess==pdTRUE)
		{
			Debug_Receive_Process((char *)Debug.rx_buffer);

		}
	}
}

//-----------------------------------------------------------------------------
// DNQ4V30������
//-----------------------------------------------------------------------------
static void Dnq4v30Task(void *argument) 
{
	uint8_t cnt = 0;
	float visibility_1min = 0.0;
	float visibility_10min = 0.0;
	/*1. ��ʼ���豸*/
	VIS_Driver_Init(&Dnq4v30,&DNQ4V30_UART);

	/*2. �����豸����*/
	while(1)
	{
		Dnq4v30.DataProcess = xSemaphoreTake(Dnq4v30.data_processing,portMAX_DELAY);
		if(Dnq4v30.DataProcess==pdTRUE)
		{
			
			if(cnt < TRANSFOR_TIME_MAX && (send_data_flag & 0x0040)==0)
			{
				if((CheckRange(dnq4v30_data.visibility_1min,10,10000)==MATH_OK) && (CheckRange(dnq4v30_data.visibility_10min,10,10000)==MATH_OK))
				{
					
					visibility_1min += dnq4v30_data.visibility_1min;
					visibility_10min += dnq4v30_data.visibility_10min;
					printf("Visibility 1min: %.1f m\r\n", dnq4v30_data.visibility_1min);
					memset(&dnq4v30_data,0x00,sizeof(dnq4v30_data));//����������������
					cnt++;
				}
			}
			else
			{
				/*������õ�ֵ�������ݽṹ��*/
				ComDataStruct.visibility_1min = visibility_1min/TRANSFOR_TIME_MAX;
				ComDataStruct.visibility_10min = visibility_10min/TRANSFOR_TIME_MAX;
				
				/*���ܼ��ȱ�־����7λ��λ��Ϊ1*/
				taskENTER_CRITICAL();
				send_data_flag = send_data_flag | 0x0040;
				taskEXIT_CRITICAL();
				cnt = 0;
			}
			printf("cnt = %d\r\n",cnt);
		}
	}
}

//-----------------------------------------------------------------------------
// LASERC12�Ƹ���������
//-----------------------------------------------------------------------------
static void LaserC12Task(void *argument) 
{
	/*1. ��ʼ���豸*/
	LaserC12_Driver_Init(&LaserC12,&LASERC12_UART);

	/*2. �����豸����*/
	while(1)
	{
		LaserC12.DataProcess = xSemaphoreTake(LaserC12.data_processing,portMAX_DELAY);
		if(LaserC12.DataProcess==pdTRUE)
		{
			printf("cloudHeight[1]: %d \r\n", laserc12_data.cloudHeight[1]);
			memset(&laserc12_data,0x00,sizeof(laserc12_data));//����������������
		}
	}
}

//-----------------------------------------------------------------------------
// ̫�������������
//-----------------------------------------------------------------------------
static void TBQ2485Task(void *argument) 
{
	/*1. ��ʼ���豸*/
	RAD485_Init(&DSG9RAD485_UART);
	
	/*2. ��ȡ��ȡ����˲ʱֵ*/
	RAD485_ReadRadiationInstantValue(&tbq2rad485_data);
	printf("dsg9rad485_data: %d \r\n", tbq2rad485_data);

}

//-----------------------------------------------------------------------------
// ����������������
//-----------------------------------------------------------------------------
static void ASC200Task(void *argument) 
{
	/*1. ��ʼ���豸*/
	ASC_Init(&ASC200,&ASC200_UART);
	
	/*2. ��ȡ��ȡ����˲ʱֵ*/
	while(1)
	{
		ASC200.DataProcess = xSemaphoreTake(ASC200.data_processing,portMAX_DELAY);
		if(ASC200.DataProcess==pdTRUE)
		{
			 for (int i = 0; i < atoi(asc200_data.observationVariableCount); i++) {
        printf("Observation Variable %s: %d, Quality Control: %d\n",
               asc200_data.observationVariables[i].variableName,
               asc200_data.observationVariables[i].variableValue,
               asc200_data.observationVariables[i].qualityControl);
			}

			for (int i = 0; i < atoi(asc200_data.statusVariableCount); i++) {
					printf("Status Variable %s: %d\n",
								 asc200_data.statusVariables[i].statusName,
								 asc200_data.statusVariables[i].statusValue);
			}
			memset(&asc200_data,0x00,sizeof(asc200_data));//����������������
		}
	}
}

//-----------------------------------------------------------------------------
// ���ݷ���������:1����/5���ӷ���һ������
//-----------------------------------------------------------------------------
static void SendDataTask(void *argument) 
{
	uint32_t preTime = xTaskGetTickCount();
	while(1)
	{
		if(send_data_flag == 0x0040)
		{
			printf("��������\r\n");
			/*�����ݽ��з�װ*/
			pack_data();
			/*��������*/
			HAL_UART_Transmit(&DEBUG_UART,(uint8_t *)&ComDataStruct,sizeof(Com_Data_Status),HAL_MAX_DELAY);
			
		}
		send_data_flag = 0x0000;
		vTaskDelayUntil(&preTime, COMMUNICATION_EXEC_CYCLE);
	}
}
