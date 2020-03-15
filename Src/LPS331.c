#include <LPS331.h>

extern I2C_HandleTypeDef hi2c1;

void LPS331_begin()
{
	uint8_t cfg = 0xE0;
	HAL_I2C_Mem_Write(&hi2c1,LPS331AP_ADRR, LPS331_CTRL_REG1, I2C_MEMADD_SIZE_8BIT, &cfg, 1, 100); 
	cfg = 0x00;
	HAL_I2C_Mem_Write(&hi2c1,LPS331AP_ADRR, LPS331_REF_P_XL, I2C_MEMADD_SIZE_8BIT, &cfg, 1, 100); 
	cfg = 0x00;
	HAL_I2C_Mem_Write(&hi2c1,LPS331AP_ADRR, LPS331_REF_P_L, I2C_MEMADD_SIZE_8BIT, &cfg, 1, 100); 
	cfg = 0x00;
	HAL_I2C_Mem_Write(&hi2c1,LPS331AP_ADRR, LPS331_REF_P_H, I2C_MEMADD_SIZE_8BIT, &cfg, 1, 100); 
  HAL_Delay(100);
}


// read pressure in pascals
float readPressurePascals(void)
{
  return (float)readPressureRaw() / 40.96;
}

// reads pressure in millibars (mbar)/hectopascals (hPa)
float readPressureMillibars(void)
{
  return (float)readPressureRaw() / 4096;
}

// reads pressure in inches of mercury (inHg)
float readPressureInchesHg(void)
{
  return (float)readPressureRaw() / 138706.5;
}

// reads pressure in millimeters of mercury (mmHg)
float readPressureMillimetersHg(void)
{
  // 1 mbar * 0,75006168270417 = 1 mmHg
  return (float)(readPressureRaw()) * 0.75006375541921 / 4096.0;
}

// reads pressure and returns raw 24-bit sensor output
int32_t readPressureRaw(void)
{
	uint8_t pres[3];
	HAL_I2C_Mem_Read(&hi2c1,LPS331AP_ADRR, LPS331_PRESS_OUT_XL, I2C_MEMADD_SIZE_8BIT, &pres[0], 1, 100);
	HAL_I2C_Mem_Read(&hi2c1,LPS331AP_ADRR, LPS331_PRESS_OUT_L, I2C_MEMADD_SIZE_8BIT, &pres[1], 1, 100);
	HAL_I2C_Mem_Read(&hi2c1,LPS331AP_ADRR, LPS331_PRESS_OUT_H, I2C_MEMADD_SIZE_8BIT, &pres[2], 1, 100);
  // combine uint8_ts
  return (int32_t)(int8_t)pres[2] << 16 | (uint16_t)pres[1] << 8 | pres[0];
	//return(int32_t)(pres[2] | pres[1] | pres[0]);
}

// reads temperature in degrees K
float readTemperatureK(){
  return readTemperatureC() + LPS331_CELSIUS_TO_KELVIN_OFFSET;
}

// reads temperature in degrees C
float readTemperatureC(void)
{
  return 42.5 + (float)readTemperatureRaw() / 480;
}

// reads temperature in degrees F
float readTemperatureF(void)
{
  return 108.5 + (float)readTemperatureRaw() / 480 * 1.8;
}

// reads temperature and returns raw 16-bit sensor output
int16_t readTemperatureRaw(void)
{
	uint8_t temp[2];
	HAL_I2C_Mem_Read(&hi2c1,LPS331AP_ADRR, LPS331_TEMP_OUT_H, I2C_MEMADD_SIZE_8BIT, &temp[0], 1, 100);
	HAL_I2C_Mem_Read(&hi2c1,LPS331AP_ADRR, LPS331_TEMP_OUT_L, I2C_MEMADD_SIZE_8BIT, &temp[1], 1, 100);
  // combine uint8_ts
  return (int16_t)(temp[0] << 8 | temp[1]);
}

// converts pressure in mbar to altitude in meters, using 1976 US
// Standard Atmosphere model (note that this formula only applies to a
// height of 11 km, or about 36000 ft)
//  If altimeter setting (QNH, barometric pressure adjusted to sea
//  level) is given, this function returns an indicated altitude
//  compensated for actual regional pressure; otherwise, it returns
//  the pressure altitude above the standard pressure level of 1013.25
//  mbar or 29.9213 inHg
float pressureToAltitudeMeters(float pressure_mbar, float altimeter_setting_mbar)
{
  return (1 - pow(pressure_mbar / altimeter_setting_mbar, 0.190263)) * 44330.8;
}

// converts pressure in inHg to altitude in feet; see notes above
float pressureToAltitudeFeet(float pressure_inHg, float altimeter_setting_inHg)
{
  return (1 - pow(pressure_inHg / altimeter_setting_inHg, 0.190263)) * 145442;
}

