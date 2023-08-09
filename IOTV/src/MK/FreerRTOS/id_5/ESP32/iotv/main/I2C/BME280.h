#ifndef MAIN_BME280_H_
#define MAIN_BME280_H_

#include "esp_log.h"
#include "driver/i2c.h"
#include "math.h"

#include "Global_def.h"

struct THP {
	double 	temperature,
			humidity,
			pressure;
	bool err;
};

void BME280_init(void);
void BME280_deinit(void);

struct THP BME280_readValues();

#endif /* MAIN_BME280_H_ */
