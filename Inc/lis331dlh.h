#ifndef LIS331DLH_H
#define LIS331DLH_H

#include "main.h"
#include <stdbool.h>

#define LIS331DLH_ADDR    0x18*2
#define LIS331DLH_ADDR_V2  0x19*2

#define ADR_FS_2        0x00
#define ADR_FS_4        0x10
#define ADR_FS_8        0x30

#define CTRL_REG1       0x20
#define CTRL_REG2       0x21
#define CTRL_REG3       0x22
#define CTRL_REG4       0x23
#define CTRL_REG5       0x24

#define OUT_X_L           0x28
#define OUT_X_H           0x29
#define OUT_Y_L           0x2A
#define OUT_Y_H           0x2B
#define OUT_Z_L           0x2C
#define OUT_Z_H           0x2D

#define RANGE_2G        2
#define RANGE_4G        4
#define RANGE_8G        8

#define G               9.8

    void LIS331DLH_begin();
    void sleep(bool enable);
    void setRange(uint8_t range);
		int16_t readAxis(uint8_t reg);
		int16_t readX();
		int16_t readY();
		int16_t readZ();
    float readGX();
    float readGY();
    float readGZ();
    float readAX();
    float readAY();
    float readAZ();
		void readXYZ(int16_t *x, int16_t *y, int16_t *z);
    void readGXYZ(float *ax, float *ay, float *az);
    void readAXYZ(float *gx, float *gy, float *gz);

#endif
