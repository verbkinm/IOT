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

void setBitInByte(uint8_t *byte, bool bitValue, uint8_t bitNumber)
{
	if (byte == NULL)
		return;

	if (bitValue)
		*byte |= (1 << bitNumber);
	else
		*byte &= ~(1 << bitNumber);
}

double inRange(double val, double min, double max)
{
	if (val < min)
		return min;
	else if (val > max)
		return max;

	return val;
}

