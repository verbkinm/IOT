/*
 * local_lib.h
 *
 *  Created on: 10 мая 2023 г.
 *      Author: user
 */

#ifndef MAIN_LOCAL_LIB_LOCAL_LIB_H
#define MAIN_LOCAL_LIB_LOCAL_LIB_H

#include "stdint.h"
#include "stdio.h"
#include "stddef.h"
#include "stdbool.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "global/global_def.h"

#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define AVG(a,b) (((a) + (b)) / 2)

uint8_t bcdToDec(uint8_t val);
uint8_t decToBcd(uint8_t val);
void setBitInByte(uint8_t *byte, bool bitValue, uint8_t bitNumber);

double inRange(double val, double min, double max);

void strmac_to_arr(const char *str, uint8_t mac[6]);
char *str_replace(char const *original, char const *pattern, char const *replacement);
char *url_encode(const char *s);
char *url_decode(const char *str);

double convert_range(double value, double From1, double From2, double To1, double To2);

void strcat_dynamic(char **str1, const char *str2);

void delete_ptr(void **ptr);

#endif /* MAIN_LOCAL_LIB_LOCAL_LIB_H */
