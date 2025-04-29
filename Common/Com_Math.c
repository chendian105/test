#include "headlibs.h"

/**************************去野值*************************/
Math_Status CheckRange(int16_t data, int16_t min, int16_t max)
{
	if (data >= min && data <= max) {
			return MATH_OK;
	} 
	return MATH_ERR;
}


/**************************滑动平均***********************/
float* MovingAverage(const float* data, int dataSize, int windowSize) 
{
    // 检查输入参数是否有效
    if (windowSize <= 0 || windowSize > dataSize || data == NULL) {
        return NULL;
    }

    // 分配存储滑动平均值的数组
    float* result = (float*)malloc((dataSize - windowSize + 1) * sizeof(float));
    if (result == NULL) {
        printf("内存分配失败！\n");
        return NULL;
    }

    // 计算滑动平均值
    for (int i = 0; i <= dataSize - windowSize; i++) {
        float sum = 0.0f;
        for (int j = 0; j < windowSize; j++) {
            sum += data[i + j];
        }
        result[i] = sum / windowSize; // 计算平均值并存储
    }

    return result;
}
