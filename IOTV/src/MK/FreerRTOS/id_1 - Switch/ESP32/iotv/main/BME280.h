/*
 * MAIN_BME280_H_.h
 *
 *  Created on: 11 апр. 2023 г.
 *      Author: user
 */

#ifndef MAIN_BME280_H_
#define MAIN_BME280_H_

#include "esp_log.h"
#include "driver/i2c.h"

#include "errorBlink.h"

enum
{
	HUM_LSB			= 0xFE,
	HUM_MSB			= 0xFD,

	TEMP_XLSB		= 0xFC,
	TEMP_LSB		= 0xFB,
	TEMP_MSB		= 0xFA,

	PRESS_XLSB		= 0xF9,
	PRESS_LSB_		= 0xF8,
	PRESS_MSB		= 0xF7,

	CONFIG			= 0xF5,
	CTRL_MEAS		= 0xF4,
	_STATUS			= 0xF3,
	CTRL_HUM		= 0xF2,

	RESET			= 0xE0,
	ID				= 0xD0,

	/* CALIB26..CALIB41 = 0xE1..0xF0 */
	/* CALIB00..CALIB25 = 0x88..0xA1 */

	dig_T1_LSB 		= 0x88,
	dig_T1_MSB 		= 0x89,
	dig_T2_LSB 		= 0x8A,
	dig_T2_MSB 		= 0x8B,
	dig_T3_LSB 		= 0x8C,
	dig_T3_MSB 		= 0x8D,

	dig_P1_LSB 		= 0x8E,
	dig_P1_MSB 		= 0x8F,
	dig_P2_LSB 		= 0x90,
	dig_P2_MSB 		= 0x91,
	dig_P3_LSB 		= 0x92,
	dig_P3_MSB 		= 0x93,
	dig_P4_LSB 		= 0x94,
	dig_P4_MSB 		= 0x95,
	dig_P5_LSB 		= 0x96,
	dig_P5_MSB 		= 0x97,
	dig_P6_LSB 		= 0x98,
	dig_P6_MSB 		= 0x99,
	dig_P7_LSB 		= 0x9A,
	dig_P7_MSB 		= 0x9B,
	dig_P8_LSB 		= 0x9C,
	dig_P8_MSB 		= 0x9D,
	dig_P9_LSB 		= 0x9E,
	dig_P9_MSB 		= 0x9F,

	_dig_H1	 		= 0xA1,
	dig_H2_LSB 		= 0xE1,
	dig_H2_MSB 		= 0xE2,
	_dig_H3	 		= 0xE3,

//	0xE4 / 0xE5[3:0] 	dig_H4 [11:4] / [3:0]
//	0xE5[7:4] / 0xE6	dig_H5 [3:0] / [11:4]
	dig_H4_LSB 		= 0xE4,
	dig_H4_MSB 		= 0xE5,
	dig_H5_LSB 		= 0xE5,
	dig_H5_MSB 		= 0xE6,
	_dig_H6	 		= 0xE7,

};

void BME280_init(void);
void BME280_deinit(void);

void BME280_readValues(double *temp, double *press, double *hum);

#endif /* MAIN_BME280_H_ */
