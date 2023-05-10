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
