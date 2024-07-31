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

#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define AVG(a,b) (((a) + (b)) / 2)

uint8_t bcdToDec(uint8_t val);
uint8_t decToBcd(uint8_t val);
void setBitInByte(uint8_t *byte, bool bitValue, uint8_t bitNumber);

double inRange(double val, double min, double max);

void strmac_to_arr(const char *str, uint8_t mac[6]);
char *url_encode(const char *s);
char *url_decode(const char *str);

double convert_range(double value, double From1, double From2, double To1, double To2);

uint32_t glob_get_status_reg(void);
void glob_set_status_reg(uint32_t reg);
void glob_set_bits_status_reg(uint32_t bits);
void glob_clear_bits_status_reg(uint32_t bits);

uint32_t glob_get_status_err(void);
void glob_set_status_err(uint32_t reg);
void glob_set_bits_status_err(uint32_t bits);
void glob_clear_bits_status_err(uint32_t bits);

uint32_t glob_get_update_reg(void);
void glob_set_update_reg(uint32_t reg);
void glob_set_bits_update_reg(uint32_t bits);
void glob_clear_bits_update_reg(uint32_t bits);

#endif /* MAIN_LOCAL_LIB_LOCAL_LIB_H */
