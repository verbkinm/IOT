/*
 * local_lib.c
 *
 *  Created on: 10 мая 2023 г.
 *      Author: user
 */

#include "local_lib.h"

uint8_t bcdToDec(uint8_t val)
{
	return( (val / 16 * 10) + (val % 16) );
}

uint8_t decToBcd(uint8_t val)
{
	return( (val / 10 * 16) + (val % 10) );
}

double inRange(double val, double min, double max)
{
	if (val < min)
		return min;
	else if (val > max)
		return max;

	return val;
}

double inc(double val, double min, double max)
{
	++val;
	if (val > max)
		return min;
	if (val < min)
		return min;

	return val;
}
