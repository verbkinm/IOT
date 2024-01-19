/*
 * sd_spi.c
 *
 *  Created on: 9 дек. 2023 г.
 *      Author: user
 */

#include "sd_mmc.h"

#define MOUNT_POINT "/sdcard"
#define EXAMPLE_MAX_CHAR_SIZE    64

static char *TAG = "SD MMC";

static esp_err_t wrap_sdmmc_host_do_transaction(int slot, sdmmc_command_t *cmdinfo);

static esp_err_t wrap_sdmmc_host_do_transaction(int slot, sdmmc_command_t *cmdinfo)
{
	esp_err_t ret = sdmmc_host_do_transaction(slot, cmdinfo);

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
		//		spi_bus_free(slot);
	}

	return ret;
}

esp_err_t sd_mmc_init(void)
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
	ESP_LOGI(TAG, "Using SDMMC peripheral");

	sdmmc_host_t host = SDMMC_HOST_DEFAULT();
	host.do_transaction = wrap_sdmmc_host_do_transaction;

	sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
	slot_config.width = 4;

	// Enable internal pullups on enabled pins. The internal pullups
	// are insufficient however, please make sure 10k external pullups are
	// connected on the bus. This is for debug / example purpose only.
//	slot_config.flags |= SDMMC_SLOT_FLAG_INTERNAL_PULLUP;

	ESP_LOGI(TAG, "Mounting filesystem");
	ret = esp_vfs_fat_sdmmc_mount(mount_point, &host, &slot_config, &mount_config, &card);

	if (ret != ESP_OK)
	{
		glob_set_bits_status_err(STATUS_SD_ERROR);
		if (ret == ESP_FAIL)
			ESP_LOGE(TAG, "Failed to mount filesystem. If you want the card to be formatted, set the EXAMPLE_FORMAT_IF_MOUNT_FAILED menuconfig option.");
		else
			ESP_LOGE(TAG, "Failed to initialize the card (%s). Make sure SD card lines have pull-up resistors in place.", esp_err_to_name(ret));

		return ret;
	}
	ESP_LOGI(TAG, "Filesystem mounted");

	// Card has been initialized, print its properties
	sdmmc_card_print_info(stdout, card);

	return ret;
}
