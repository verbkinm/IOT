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

void strmac_to_arr(const char *str, uint8_t mac[6])
{
    int arr[6];
    sscanf(str, "%x:%x:%x:%x:%x:%x*c",
           &arr[0], &arr[1], &arr[2],
           &arr[3], &arr[4], &arr[5]);

    for (uint8_t i = 0; i < 6; ++i)
        mac[i] = arr[i];
}
