#include "headlibs.h"

/************实例化设备控制结构体****************/
DEVICE_Handle_t Debug;     //实例化调试口控制结构体
DEVICE_Handle_t Dnq4v30;   //实例化能见度仪驱动控制结构体
DEVICE_Handle_t LaserC12;  //实例化云高仪驱动控制结构体
PASVDEVICE_Handle_t ASC200; //实例化云量云状驱动控制结构体


/***************设备数据结构体*******************/
VIS_Data_t dnq4v30_data;   //能见度仪数据结构体
MLD_Data_t laserc12_data;  //云高仪数据结构体
uint16_t   tbq2rad485_data;//太阳辐射计
ASC200_DataFrame asc200_data; //云量云状数据结构体



/*******************任务************************/
/*启动任务*/
void StartTaskFunction( void * arg);
#define START_TASK_NAME "StartTask"
#define START_TASK_STACK 128
#define START_TASK_PRIORITY 10
TaskHandle_t StartTaskHandle;


/*0. 调试任务*/
void DebugTaskFunction( void * arg);
#define DEBUG_TASK_NAME "DebugTask"
#define DEBUG_TASK_STACK 128
#define DEBUG_TASK_PRIORITY 10
TaskHandle_t DebugTaskHandle;
SemaphoreHandle_t DebugSemaphore;        //调试口指令接受信号量
BaseType_t DebugBaseType;                //

/*1. dnq4v30任务*/
void Dnq4v30Task( void * arg);
#define DNQ4V30_TASK_NAME "Dnq4v30Task"
#define DNQ4V30_TASK_STACK_SIZE 256
#define DNQ4V30_TASK_PRIORITY 9
TaskHandle_t Dnq4v30TaskHandle;

/*2. 云高仪任务*/
void LaserC12Task( void * arg);
#define LASERC12_TASK_NAME "LaserC12Task"
#define LASERC12_TASK_STACK_SIZE 256
#define LASERC12_TASK_PRIORITY 9
TaskHandle_t LaserC12TaskHandle;

/*3. 太阳辐射计任务*/
void TBQ2485Task( void * arg);
#define TBQ2485_TASK_NAME "Tbq2485Task"
#define TBQ2485_TASK_STACK_SIZE 256
#define TBQ2485_TASK_PRIORITY 9
TaskHandle_t Tbq2485TaskHandle;

/*4. 云量云状传感器任务*/
void ASC200Task( void * arg);
#define ASC200_TASK_NAME "Asc200Task"
#define ASC200_TASK_STACK_SIZE 256
#define ASC200_TASK_PRIORITY 9
TaskHandle_t Asc200TaskHandle;

/*数据发送任务*/
void SendDataTask( void * arg);
#define SENDDATA_TASK_NAME "DataSendTask"
#define SENDDATA_TASK_STACK_SIZE 256
#define SENDDATA_TASK_PRIORITY 10
TaskHandle_t SendDataHandle;


void App_Task_FreeRTOSStart(void)
{
	/*1. 初始化*/

	printf("==========start===========\r\n");

	/*2. 创建一个启动任务：在启动任务中创建和业务相关的任务*/
	BaseType_t r = xTaskCreate(StartTaskFunction,  /*启动任务函数*/
														START_TASK_NAME,     /*任务名*/
														START_TASK_STACK,    /*栈大小：单位是字（不是自己）字的大小：32位架构是4 START_TASK_STACK*4=字节数*/
														NULL,                /*指定给任务函数传递参数*/
														START_TASK_PRIORITY, /*任务优先级*/
														&StartTaskHandle);   /*任务句柄*/
	
	if(r==pdPASS)
	{
		printf("启动任务创建成功\r\n");
	}
	else
	{
		printf("启动任务创建失败\r\n");	
	}
	
	/*3. 启动调度器*/
	vTaskStartScheduler();
}

/*启动任务*/
void StartTaskFunction( void * arg)
{
	printf("开始调度\r\n");

//	//创建一个定时器
//	    xTimerHandle = xTimerCreate(
//        "HourlyTimer",              // 定时器名称
//        pdMS_TO_TICKS(3600 * 1000), // 定时器周期（1小时）
//        pdTRUE,                    // 自动重载
//        (void*)0,                   // 定时器ID
//        vTimerCallback              // 定时器回调函数
//    );
//	
	
	/*创建事件集*/
	
	/*调试任务*/
	xTaskCreate(DebugTaskFunction,
							DEBUG_TASK_NAME,
							DEBUG_TASK_STACK,
							NULL,
							DEBUG_TASK_PRIORITY,
							&DebugTaskHandle);	

	/*1.DNQ4V30（能见度）设备任务*/
	xTaskCreate(Dnq4v30Task,
							DNQ4V30_TASK_NAME,
							DNQ4V30_TASK_STACK_SIZE,
							NULL,
							DNQ4V30_TASK_PRIORITY,
							&Dnq4v30TaskHandle);
	
//	/*2.C12（云高仪）设备任务*/
//	xTaskCreate(LaserC12Task,
//							LASERC12_TASK_NAME,
//							LASERC12_TASK_STACK_SIZE,
//							NULL,
//							LASERC12_TASK_PRIORITY,
//							&LaserC12TaskHandle);
	
//	/*3.太阳辐射计*/
//	xTaskCreate(TBQ2485Task,
//							TBQ2485_TASK_NAME,
//							TBQ2485_TASK_STACK_SIZE,
//							NULL,
//							TBQ2485_TASK_PRIORITY,
//							&Tbq2485TaskHandle);
							
//	/*4.云量云状*/
//	xTaskCreate(ASC200Task,
//							ASC200_TASK_NAME,
//							ASC200_TASK_STACK_SIZE,
//							NULL,
//							ASC200_TASK_PRIORITY,
//							&Asc200TaskHandle);
	
	/*数据发送*/
	xTaskCreate(SendDataTask,
							SENDDATA_TASK_NAME,
							SENDDATA_TASK_STACK_SIZE,
							NULL,
							SENDDATA_TASK_PRIORITY,
							&SendDataHandle);	
	
	/*删除自己*/
	vTaskDelete(NULL);
	printf("不会执行\r\n");
}

//-----------------------------------------------------------------------------
// 指令接收任务函数
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
// DNQ4V30任务函数
//-----------------------------------------------------------------------------
static void Dnq4v30Task(void *argument) 
{
	uint8_t cnt = 0;
	float visibility_1min = 0.0;
	float visibility_10min = 0.0;
	/*1. 初始化设备*/
	VIS_Driver_Init(&Dnq4v30,&DNQ4V30_UART);

	/*2. 处理设备数据*/
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
					memset(&dnq4v30_data,0x00,sizeof(dnq4v30_data));//对数组进行清零操作
					cnt++;
				}
			}
			else
			{
				/*将处理好的值存入数据结构体*/
				ComDataStruct.visibility_1min = visibility_1min/TRANSFOR_TIME_MAX;
				ComDataStruct.visibility_10min = visibility_10min/TRANSFOR_TIME_MAX;
				
				/*将能见度标志（低7位）位置为1*/
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
// LASERC12云高仪任务函数
//-----------------------------------------------------------------------------
static void LaserC12Task(void *argument) 
{
	/*1. 初始化设备*/
	LaserC12_Driver_Init(&LaserC12,&LASERC12_UART);

	/*2. 处理设备数据*/
	while(1)
	{
		LaserC12.DataProcess = xSemaphoreTake(LaserC12.data_processing,portMAX_DELAY);
		if(LaserC12.DataProcess==pdTRUE)
		{
			printf("cloudHeight[1]: %d \r\n", laserc12_data.cloudHeight[1]);
			memset(&laserc12_data,0x00,sizeof(laserc12_data));//对数组进行清零操作
		}
	}
}

//-----------------------------------------------------------------------------
// 太阳辐射计任务函数
//-----------------------------------------------------------------------------
static void TBQ2485Task(void *argument) 
{
	/*1. 初始化设备*/
	RAD485_Init(&DSG9RAD485_UART);
	
	/*2. 读取读取辐射瞬时值*/
	RAD485_ReadRadiationInstantValue(&tbq2rad485_data);
	printf("dsg9rad485_data: %d \r\n", tbq2rad485_data);

}

//-----------------------------------------------------------------------------
// 云量传感器任务函数
//-----------------------------------------------------------------------------
static void ASC200Task(void *argument) 
{
	/*1. 初始化设备*/
	ASC_Init(&ASC200,&ASC200_UART);
	
	/*2. 读取读取辐射瞬时值*/
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
			memset(&asc200_data,0x00,sizeof(asc200_data));//对数组进行清零操作
		}
	}
}

//-----------------------------------------------------------------------------
// 数据发送任务函数:1分钟/5分钟发送一次数据
//-----------------------------------------------------------------------------
static void SendDataTask(void *argument) 
{
	uint32_t preTime = xTaskGetTickCount();
	while(1)
	{
		if(send_data_flag == 0x0040)
		{
			printf("发送数据\r\n");
			/*将数据进行封装*/
			pack_data();
			/*发送数据*/
			HAL_UART_Transmit(&DEBUG_UART,(uint8_t *)&ComDataStruct,sizeof(Com_Data_Status),HAL_MAX_DELAY);
			
		}
		send_data_flag = 0x0000;
		vTaskDelayUntil(&preTime, COMMUNICATION_EXEC_CYCLE);
	}
}
