#ifndef MAIN_BME280_H_
#define MAIN_BME280_H_

#include "string.h"
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
typedef struct THP thp_t;

void BME280_init(void);
void BME280_deinit(void);

void BME280_readValues(struct THP *thp, struct THP *thp_without_calibration);

int BME280_get_calib_temperature();
int BME280_get_calib_humidity();
int BME280_get_calib_pressure();

void BME280_set_calib_temperature(int val);
void BME280_set_calib_humidity(int val);
void BME280_set_calib_pressure(int val);

#endif /* MAIN_BME280_H_ */
