#ifndef MAIN_BME280_H_
#define MAIN_BME280_H_

#include "esp_log.h"
#include "driver/i2c.h"
#include "math.h"

#include "Global_def.h"

struct THP {
	double temperature,
		   humidity,
		   pressure;
	bool err;
};

void BME280_init(void);
void BME280_deinit(void);

struct THP BME280_readValues();
struct THP BME280_readValues_without_calibration();

int8_t BME280_get_calib_temperature();
int8_t BME280_get_calib_humidity();
int8_t BME280_get_calib_pressure();

void BME280_set_calib_temperature(int8_t val);
void BME280_set_calib_humidity(int8_t val);
void BME280_set_calib_pressure(int8_t val);

#endif /* MAIN_BME280_H_ */
