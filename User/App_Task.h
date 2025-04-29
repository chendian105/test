#ifndef __APP_TASK_H
#define __APP_TASK_H

#include "dnq4v30.h"
#include "LaserC12.h"
#include "asc200.h"


void App_Task_FreeRTOSStart(void);

extern DEVICE_Handle_t Debug;
extern DEVICE_Handle_t Dnq4v30;
extern DEVICE_Handle_t LaserC12;
extern PASVDEVICE_Handle_t ASC200;


extern VIS_Data_t dnq4v30_data;
extern MLD_Data_t laserc12_data;
extern ASC200_DataFrame asc200_data; //云量云状数据结构体

#endif
