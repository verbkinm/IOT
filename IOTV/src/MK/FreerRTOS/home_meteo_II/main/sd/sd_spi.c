/*
 * sd_spi.c
 *
 *  Created on: 9 дек. 2023 г.
 *      Author: user
 */

#include "sd_spi.h"

#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "stdbool.h"
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>

#define MOUNT_POINT "/sdcard"
#define EXAMPLE_MAX_CHAR_SIZE    64

static char *TAG = "SD SPI";

static esp_err_t wrap_sdspi_host_do_transaction(int slot, sdmmc_command_t *cmdinfo);

static esp_err_t wrap_sdspi_host_do_transaction(int slot, sdmmc_command_t *cmdinfo)
{
	esp_err_t ret = sdspi_host_do_transaction(slot, cmdinfo);

	if (ret == ESP_ERR_TIMEOUT)
	{
		static time_t old_t = 0;
		if (old_t == 0)
		{
			time(&old_t);
			return ret;
		}

		time_t cur_t;
		time(&cur_t);

		if ((cur_t - old_t) == 0)
		{
			glob_set_bits_status_err(STATUS_SD_ERROR);
			ESP_LOGE(TAG, "STATUS_SD_ERROR - %d", ret);
			return ret;
		}
		time(&old_t);
	}

	if (ret != ESP_OK && ret != ESP_ERR_NOT_SUPPORTED)
	{
		ESP_LOGE(TAG, "STATUS_SD_ERROR - %d", ret);
		glob_set_bits_status_err(STATUS_SD_ERROR);
	}

	return ret;
}

esp_err_t sd_spi_init(void)
{
	esp_err_t ret;

	esp_vfs_fat_sdmmc_mount_config_t mount_config = {
			.format_if_mount_failed = true,
			.max_files = 10,
			.allocation_unit_size = 32 * 1024
	};
	sdmmc_card_t *card;
	const char mount_point[] = MOUNT_POINT;
	ESP_LOGI(TAG, "Initializing SD card");
	ESP_LOGI(TAG, "Using SPI peripheral");

	sdmmc_host_t host = SDSPI_HOST_DEFAULT();
	host.do_transaction = wrap_sdspi_host_do_transaction;

	spi_bus_config_t bus_cfg = {
			.mosi_io_num = SPI_NUM_MOSI,
			.miso_io_num = SPI_NUM_MISO,
			.sclk_io_num = SPI_NUM_SCK,
			.quadwp_io_num = -1,
			.quadhd_io_num = -1,
			.max_transfer_sz = 4000,
	};

	ret = spi_bus_initialize(host.slot, &bus_cfg, SDSPI_DEFAULT_DMA);
	if (ret != ESP_OK)
	{
		glob_set_bits_status_err(STATUS_SD_ERROR);
		ESP_LOGE(TAG, "Failed to initialize bus.");
		return ret;
	}

	sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
	slot_config.gpio_cs = SPI_NUM_CS;
	slot_config.host_id = host.slot;

	ESP_LOGI(TAG, "Mounting filesystem");
	ret = esp_vfs_fat_sdspi_mount(mount_point, &host, &slot_config, &mount_config, &card);

	if (ret != ESP_OK)
	{
		glob_set_bits_status_err(STATUS_SD_ERROR);
		if (ret == ESP_FAIL)
		{
			ESP_LOGE(TAG, "Failed to mount filesystem. "
					"If you want the card to be formatted, set the CONFIG_EXAMPLE_FORMAT_IF_MOUNT_FAILED menuconfig option.");
		}
		else
		{
			ESP_LOGE(TAG, "Failed to initialize the card (%s). "
					"Make sure SD card lines have pull-up resistors in place.", esp_err_to_name(ret));
		}
		return ret;
	}
	ESP_LOGI(TAG, "Filesystem mounted");

	// Card has been initialized, print its properties
	sdmmc_card_print_info(stdout, card);

	return ret;
}

void sd_spi_space_info(uint32_t *totalKB, uint32_t *availableKB)
{
	if (totalKB == NULL || availableKB == NULL)
		return;

	FATFS *fs;
	uint32_t fre_clust, fre_sect, tot_sect;

	/* Get volume information and free clusters of drive 0 */
	f_getfree("0:", &fre_clust, &fs);
	/* Get total sectors and free sectors */
	tot_sect = (fs->n_fatent - 2) * fs->csize;
	fre_sect = fre_clust * fs->csize;

	/* Print the free space (assuming 512 bytes/sector) */
//	printf("%10lu KiB total drive space.\n%10lu KiB available.\n", tot_sect / 2, fre_sect / 2);

	*totalKB = tot_sect / 2;
	*availableKB = fre_sect / 2;
}

//void sd_spi_deinit(void)
//{
//	const char mount_point[] = MOUNT_POINT;
//
//	// All done, unmount partition and disable SPI peripheral
//	esp_vfs_fat_sdcard_unmount(mount_point, card);
//	ESP_LOGI(TAG, "Card unmounted");
//
//	//deinitialize the bus after all devices are removed
//	spi_bus_free(host.slot);
//}
