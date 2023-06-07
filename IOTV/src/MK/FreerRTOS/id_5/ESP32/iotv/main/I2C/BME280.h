#ifndef MAIN_BME280_H_
#define MAIN_BME280_H_

#include "esp_log.h"
#include "driver/i2c.h"
#include "math.h"

#include "Global_def.h"

void BME280_init(void);
void BME280_deinit(void);

void BME280_readValues(double *temp, double *press, double *hum);

#endif /* MAIN_BME280_H_ */
