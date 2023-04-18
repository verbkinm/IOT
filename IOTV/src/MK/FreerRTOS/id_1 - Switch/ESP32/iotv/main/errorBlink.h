/*
 * errorBlink.h
 *
 *  Created on: 18 апр. 2023 г.
 *      Author: user
 */

#ifndef MAIN_ERRORBLINK_H_
#define MAIN_ERRORBLINK_H_

#define I2C_INIT_FAIL						1
#define I2C_DEINIT_FAIL						2
#define I2C_WRITE_FAIL						3
#define I2C_READ_FAIL						4
#define RESULT__INTERRUPT_STATUS_GPIO_LOOP 	5
#define RESULT__RANGE_STATUS_LOOP			6
#define SYSTEM__FRESH_OUT_OF_RESET_LOOP		7

void errorLoopBlink(const char* TAG, uint8_t value);

#endif /* MAIN_ERRORBLINK_H_ */
