/*
 * sd_spi.h
 *
 *  Created on: 9 дек. 2023 г.
 *      Author: user
 */

#ifndef MAIN_SD_SD_SPI_H_
#define MAIN_SD_SD_SPI_H_

#include "esp_check.h"

#include "Global_def.h"
#include "Local_Lib/local_lvgl_lib.h"

esp_err_t sd_spi_init(void);
void sd_spi_space_info(uint32_t *totalKB, uint32_t *availableKB);
//void sd_spi_deinit(void);

//esp_err_t s_write_file(const char *path, char *data);

#endif /* MAIN_SD_SD_SPI_H_ */
