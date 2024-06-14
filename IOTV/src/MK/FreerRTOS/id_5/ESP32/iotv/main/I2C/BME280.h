#ifndef MAIN_BME280_H_
#define MAIN_BME280_H_

#include "string.h"
#include "esp_log.h"
#include "driver/i2c.h"
#include "math.h"

#include "Global_def.h"

struct THP {
	float temperature,
		   humidity,
		   pressure;
	bool err;
};
typedef struct THP thp_t;

void BME280_init(void);
void BME280_deinit(void);

struct THP BME280_readValues(void);

#endif /* MAIN_BME280_H_ */
