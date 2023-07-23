/*
 * local_lib.h
 *
 *  Created on: 10 мая 2023 г.
 *      Author: user
 */

#ifndef MAIN_LOCAL_LIB_LOCAL_LIB_H_
#define MAIN_LOCAL_LIB_LOCAL_LIB_H_

#include "stdint.h"
#include "stddef.h"

uint8_t bcdToDec(uint8_t val);
uint8_t decToBcd(uint8_t val);

double inRange(double val, double min, double max);
double inc(double val, double min, double max);


#endif /* MAIN_LOCAL_LIB_LOCAL_LIB_H_ */
