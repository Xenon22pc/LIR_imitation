
#include "lis331dlh.h"

extern I2C_HandleTypeDef hi2c1;

float _mult;

void LIS331DLH_begin() {
    // включаем координаты x, y, z
		uint8_t cfg = 0x27;
		HAL_I2C_Mem_Write(&hi2c1,LIS331DLH_ADDR, CTRL_REG1, I2C_MEMADD_SIZE_8BIT, &cfg, 1, 100); 
		// устанавливаем максимальное измеряемое ускорение в G
		setRange(RANGE_2G);
}

void setRange(uint8_t range) {
	uint8_t cfg=0;
    switch (range) {
        case RANGE_2G: {
            cfg = ADR_FS_2;
            _mult = RANGE_2G / 32767.0;
            break;
        }
        case RANGE_4G: {
            cfg = ADR_FS_4;
            _mult = RANGE_4G / 32767.0;
            break;
        }
        case RANGE_8G: {
            cfg = ADR_FS_8;
            _mult = RANGE_8G / 32767.0;
            break;
        }
        default: {
            _mult = RANGE_2G / 32767.0;    
        }
        break;
    }
    HAL_I2C_Mem_Write(&hi2c1,LIS331DLH_ADDR, CTRL_REG4, I2C_MEMADD_SIZE_8BIT, &cfg, 1, 100); 
}

void sleep(bool enable) {
	uint8_t cfg=0;
    if (enable)
        cfg &= ~(1 << 5);
    else
        cfg |= (1 << 5);

     HAL_I2C_Mem_Write(&hi2c1,LIS331DLH_ADDR, CTRL_REG1, I2C_MEMADD_SIZE_8BIT, &cfg, 1, 100); 
}

int16_t readAxis(uint8_t reg) {
		uint8_t tmp[2];
		HAL_I2C_Mem_Read(&hi2c1,LIS331DLH_ADDR, reg, I2C_MEMADD_SIZE_8BIT, &tmp[0], 1, 100);
		HAL_I2C_Mem_Read(&hi2c1,LIS331DLH_ADDR, reg+1, I2C_MEMADD_SIZE_8BIT, &tmp[1], 1, 100);
    return ((int16_t)tmp[1] << 8) | tmp[0];
}

int16_t readX() {
    return readAxis(OUT_X_L);
}

int16_t readY() {
    return readAxis(OUT_Y_L);
}

int16_t readZ() {
    return readAxis(OUT_Z_L);
}

float readGX() {
    return readX()*_mult;
}

float readGY() {
    return readY()*_mult;
}

float readGZ() {
    return readZ()*_mult;
}

float readAX() {
    return readGX() * G;
}

float readAY() {
    return readGY() * G;
}

float readAZ() {
    return readGZ() * G;
}

void readXYZ(int16_t *x, int16_t *y, int16_t *z) {
		uint8_t values[6];
    HAL_I2C_Mem_Read(&hi2c1,LIS331DLH_ADDR, OUT_X_L, I2C_MEMADD_SIZE_8BIT, &values[0], 1, 100);
		HAL_I2C_Mem_Read(&hi2c1,LIS331DLH_ADDR, OUT_X_H, I2C_MEMADD_SIZE_8BIT, &values[1], 1, 100);
		HAL_I2C_Mem_Read(&hi2c1,LIS331DLH_ADDR, OUT_Y_L, I2C_MEMADD_SIZE_8BIT, &values[2], 1, 100);
		HAL_I2C_Mem_Read(&hi2c1,LIS331DLH_ADDR, OUT_Y_H, I2C_MEMADD_SIZE_8BIT, &values[3], 1, 100);
		HAL_I2C_Mem_Read(&hi2c1,LIS331DLH_ADDR, OUT_Z_L, I2C_MEMADD_SIZE_8BIT, &values[4], 1, 100);
		HAL_I2C_Mem_Read(&hi2c1,LIS331DLH_ADDR, OUT_Z_H, I2C_MEMADD_SIZE_8BIT, &values[5], 1, 100);
    
    *x = *((int16_t*)&values[0]);
    *y = *((int16_t*)&values[2]);
    *z = *((int16_t*)&values[4]);
}

void readGXYZ(float *gx, float *gy, float *gz) {
    int16_t x, y, z;
    readXYZ(&x, &y, &z);
    *gx = x * _mult;
    *gy = y * _mult;
    *gz = z * _mult;
}

void readAXYZ(float *ax, float *ay, float *az) {
    readGXYZ(ax, ay, az);
    (*ax) *= G;
    (*ay) *= G;
    (*az) *= G;
}
