#include "headlibs.h"

/**************************ȥҰֵ*************************/
Math_Status CheckRange(int16_t data, int16_t min, int16_t max)
{
	if (data >= min && data <= max) {
			return MATH_OK;
	} 
	return MATH_ERR;
}


/**************************����ƽ��***********************/
float* MovingAverage(const float* data, int dataSize, int windowSize) 
{
    // �����������Ƿ���Ч
    if (windowSize <= 0 || windowSize > dataSize || data == NULL) {
        return NULL;
    }

    // ����洢����ƽ��ֵ������
    float* result = (float*)malloc((dataSize - windowSize + 1) * sizeof(float));
    if (result == NULL) {
        printf("�ڴ����ʧ�ܣ�\n");
        return NULL;
    }

    // ���㻬��ƽ��ֵ
    for (int i = 0; i <= dataSize - windowSize; i++) {
        float sum = 0.0f;
        for (int j = 0; j < windowSize; j++) {
            sum += data[i + j];
        }
        result[i] = sum / windowSize; // ����ƽ��ֵ���洢
    }

    return result;
}
