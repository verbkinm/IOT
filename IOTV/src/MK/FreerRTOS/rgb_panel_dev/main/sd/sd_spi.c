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


esp_err_t s_write_file(const char *path, char *data)
{
	ESP_LOGI(TAG, "Opening file %s", path);
	FILE *f = fopen(path, "w");
	if (f == NULL) {
		ESP_LOGE(TAG, "Failed to open file for writing");
		return ESP_FAIL;
	}
	fprintf(f, data);
	fclose(f);
	ESP_LOGI(TAG, "File written");

	return ESP_OK;
}

static esp_err_t s_read_file(const char *path)
{
	ESP_LOGI(TAG, "Reading file %s", path);
	FILE *f = fopen(path, "r");
	if (f == NULL) {
		ESP_LOGE(TAG, "Failed to open file for reading");
		return ESP_FAIL;
	}
	char line[EXAMPLE_MAX_CHAR_SIZE];
	fgets(line, sizeof(line), f);
	fclose(f);

	// strip newline
	char *pos = strchr(line, '\n');
	if (pos) {
		*pos = '\0';
	}
	ESP_LOGI(TAG, "Read from file: '%s'", line);

	return ESP_OK;
}


esp_err_t sd_spi_init(void)
{
	esp_err_t ret;

	esp_vfs_fat_sdmmc_mount_config_t mount_config = {
			.format_if_mount_failed = false,
			.max_files = 5,
			.allocation_unit_size = 16 * 1024
	};
	sdmmc_card_t *card;
	const char mount_point[] = MOUNT_POINT;
	ESP_LOGI(TAG, "Initializing SD card");
	ESP_LOGI(TAG, "Using SPI peripheral");

	sdmmc_host_t host = SDSPI_HOST_DEFAULT();
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

//	const char *file_hello = MOUNT_POINT"/conf.jsn";
//	char data[EXAMPLE_MAX_CHAR_SIZE];
//	snprintf(data, EXAMPLE_MAX_CHAR_SIZE, "%s %s!\n", "Hello", card->cid.name);
//	ret = s_write_file(file_hello, data);
//	if (ret != ESP_OK) {
//		return ret;
//	}
	//	struct stat st;
	//	const char *f = "/sdcard/panel/sd_on.png";
	//	if (stat(f, &st) == 0)
	//	{
	//		printf("File 1 size: %d\n", (int)st.st_size);
	//	}
	//	else
	//		printf("file not exist\n");
	//
	//	const char *f2 = "/sdcard/img/panel/sd_on.png";
	//	if (stat(f2, &st) == 0)
	//	{
	//		printf("File 2 size: %d\n", (int)st.st_size);
	//	}
	//	else
	//		printf("file not exist\n");


	//	lv_img_set_src(icon, "S:/sdcard/status_panel/sd_on.png");

	// Use POSIX and C standard library functions to work with files.

	// First create a file.
	//	const char *file_hello = MOUNT_POINT"/hello.txt";
	//	char data[EXAMPLE_MAX_CHAR_SIZE];
	//	snprintf(data, EXAMPLE_MAX_CHAR_SIZE, "%s %s!\n", "Hello", card->cid.name);
	//	ret = s_example_write_file(file_hello, data);
	//	if (ret != ESP_OK) {
	//		return ret;
	//	}
	//
	//	const char *file_foo = MOUNT_POINT"/foo.txt";
	//
	//	// Check if destination file exists before renaming
	//	struct stat st;
	//	if (stat(file_foo, &st) == 0) {
	//		// Delete it if it exists
	//		unlink(file_foo);
	//	}
	//
	//	// Rename original file
	//	ESP_LOGI(TAG, "Renaming file %s to %s", file_hello, file_foo);
	//	if (rename(file_hello, file_foo) != 0) {
	//		ESP_LOGE(TAG, "Rename failed");
	//		return ret;
	//	}
	//
	//	ret = s_example_read_file(file_foo);
	//	if (ret != ESP_OK) {
	//		return ret;
	//	}
	//
	//	// Format FATFS
	//	ret = esp_vfs_fat_sdcard_format(mount_point, card);
	//	if (ret != ESP_OK) {
	//		ESP_LOGE(TAG, "Failed to format FATFS (%s)", esp_err_to_name(ret));
	//		return ret;
	//	}
	//
	//	if (stat(file_foo, &st) == 0) {
	//		ESP_LOGI(TAG, "file still exists");
	//		return ret;
	//	} else {
	//		ESP_LOGI(TAG, "file doesnt exist, format done");
	//	}
	//
	//	const char *file_nihao = MOUNT_POINT"/nihao.txt";
	//	memset(data, 0, EXAMPLE_MAX_CHAR_SIZE);
	//	snprintf(data, EXAMPLE_MAX_CHAR_SIZE, "%s %s!\n", "Nihao", card->cid.name);
	//	ret = s_example_write_file(file_nihao, data);
	//	if (ret != ESP_OK) {
	//		return ret;
	//	}
	//
	//	//Open file for reading
	//	ret = s_example_read_file(file_nihao);
	//	if (ret != ESP_OK) {
	//		return ret;
	//	}

	return ret;
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
