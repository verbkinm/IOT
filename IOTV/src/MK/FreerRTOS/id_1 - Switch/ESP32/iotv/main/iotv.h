/*
 * iotv.h
 *
 *  Created on: 13 апр. 2023 г.
 *      Author: user
 */

#ifndef MAIN_IOTV_H_
#define MAIN_IOTV_H_

#include "Defines.h"

void iotvTask(void *pvParameters);
void Vl6180X_Task(void *pvParameters);
void BME280_Task(void *pvParameters);

#endif /* MAIN_IOTV_H_ */
