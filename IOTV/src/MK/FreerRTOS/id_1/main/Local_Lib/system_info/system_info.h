/*
 * system_info.h
 *
 *  Created on: 1 авг. 2024 г.
 *      Author: user
 */

#ifndef MAIN_LOCAL_LIB_SYSTEM_INFO_SYSTEM_INFO_H_
#define MAIN_LOCAL_LIB_SYSTEM_INFO_SYSTEM_INFO_H_

#include "esp_chip_info.h"

const char* get_chip_model(esp_chip_model_t model);
char *get_chip_info(void);
char *get_freq_info(void);
char *get_memory_info(void);
char *get_partition_tabel(void);
char *get_project_info(void);

char *get_system_info(void);

#endif /* MAIN_LOCAL_LIB_SYSTEM_INFO_SYSTEM_INFO_H_ */
