/*
 * sd_spi.h
 *
 *  Created on: 9 дек. 2023 г.
 *      Author: user
 */

#ifndef MAIN_SD_SD_MMC_H_
#define MAIN_SD_SD_MMC_H_

#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "driver/sdmmc_host.h"

#include "Global_def.h"
#include "Local_Lib/local_lib.h"

esp_err_t sd_mmc_init(void);
//void sd_spi_deinit(void);

#endif /* MAIN_SD_SD_MMC_H_ */
