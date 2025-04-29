#ifndef __COM_MATH_H
#define __COM_MATH_H

#include "Com_Config.h"

/*���ݴ�����ö��*/
typedef enum {
   MATH_OK          = 0x00,
	 MATH_ERR         = 0x01,
} Math_Status;

Math_Status CheckRange(int16_t data, int16_t min, int16_t max);
#endif
