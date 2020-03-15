#ifndef LPS331_h
#define LPS331_h


#define LPS331AP_ADRR     0x5C*2


#include <stdbool.h>
#include <math.h>
#include "main.h"

// register addresses
// Note: Some of the register names in the datasheet are inconsistent
// between Table 14 in section 6 and the register descriptions in
// section 7. Where they differ, the names from section 7 have been
// used here.

#define LPS331_REF_P_XL       0x08
#define LPS331_REF_P_L        0x09
#define LPS331_REF_P_H        0x0A

#define LPS331_WHO_AM_I       0x0F

#define LPS331_RES_CONF       0x10

#define LPS331_CTRL_REG1      0x20
#define LPS331_CTRL_REG2      0x21
#define LPS331_CTRL_REG3      0x22
#define LPS331_INTERRUPT_CFG  0x23
#define LPS331_INT_SOURCE     0x24
#define LPS331_THS_P_L        0x25
#define LPS331_THS_P_H        0x26
#define LPS331_STATUS_REG     0x27

#define LPS331_PRESS_OUT_XL   0x28
#define LPS331_PRESS_OUT_L    0x29
#define LPS331_PRESS_OUT_H    0x2A

#define LPS331_TEMP_OUT_L     0x2B
#define LPS331_TEMP_OUT_H     0x2C

#define LPS331_AMP_CTRL       0x30

#define LPS331_DELTA_PRESS_XL 0x3C
#define LPS331_DELTA_PRESS_L  0x3D
#define LPS331_DELTA_PRESS_H  0x3E

#define alt_mbar 1013.25
#define alt_inHg 29.9213

// Some physical constants
#define LPS331_CELSIUS_TO_KELVIN_OFFSET 273.15

	void LPS331_begin();

	void writeReg(uint8_t reg, uint8_t value);
	uint8_t readReg(uint8_t reg);

	float readPressurePascals(void);
	float readPressureMillibars(void);
	float readPressureInchesHg(void);
	float readPressureMillimetersHg(void);
	int32_t readPressureRaw(void);
	float readTemperatureK(void);
	float readTemperatureC(void);
	float readTemperatureF(void);
	int16_t readTemperatureRaw(void);

	float pressureToAltitudeMeters(float pressure_mbar, float altimeter_setting_mbar);
	float pressureToAltitudeFeet(float pressure_inHg, float altimeter_setting_inHg);

	bool autoDetectAddress(void);
	bool testWhoAmI(void);


#endif
