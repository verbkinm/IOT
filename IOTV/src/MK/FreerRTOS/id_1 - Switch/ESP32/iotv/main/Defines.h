/*
 * Defines.h
 *
 *  Created on: 13 апр. 2023 г.
 *      Author: user
 */

#ifndef MAIN_DEFINES_H_
#define MAIN_DEFINES_H_

#include <stdint.h>

#define BUFSIZE 1024  // по умолчанию, после компиляции, BUFSIZ = 128

struct DataPkg
{
	uint8_t *data;
	uint64_t size;
};


#endif /* MAIN_DEFINES_H_ */
