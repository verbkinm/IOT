#ifndef MAIN_VL6180X_SIMPLE_H_
#define MAIN_VL6180X_SIMPLE_H_

#include "esp_log.h"
#include "driver/i2c.h"

#include "Global_def.h"

void VL6180X_init(void);
void VL6180X_deinit(void);

uint8_t VL6180X_simpleRange(void);
uint16_t VL6180X_simpleALS(uint8_t gain);

#endif /* MAIN_VL6180X_SIMPLE_H_ */
