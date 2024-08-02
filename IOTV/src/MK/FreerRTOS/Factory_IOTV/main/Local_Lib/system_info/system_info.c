/*
 * system_info.c
 *
 *  Created on: 1 авг. 2024 г.
 *      Author: user
 */

#include "system_info.h"

#include <inttypes.h>
#include "esp_app_desc.h"
#include "esp_partition.h"

#include "esp_heap_caps.h"
#include "soc/rtc.h"	//real time clock

#include "Local_Lib/local_lib.h"

static const char *dash = "------------------------------------------------------------\n";

char *get_project_info(void)
{
	char *result = calloc(1, 1);
	char buf[BUFSIZ];

	const esp_app_desc_t *app = esp_app_get_description();

	strcat_dynamic(&result, "Project:\n");
	strcat_dynamic(&result, dash);

	snprintf(buf, sizeof(buf), "Build date: %s %s\n", app->date, app->time);
	strcat_dynamic(&result, buf);

	snprintf(buf, sizeof(buf), "Name: %s\n", app->project_name);
	strcat_dynamic(&result, buf);

	snprintf(buf, sizeof(buf), "Version: %s\n", app->version);
	strcat_dynamic(&result, buf);

	snprintf(buf, sizeof(buf), "IDF: %s\n", app->idf_ver);
	strcat_dynamic(&result, buf);
	strcat_dynamic(&result, dash);

	return result;
}

char *get_chip_info(void)
{
	char *result = calloc(1, 1);
	char buf[BUFSIZ];

	esp_chip_info_t chip_info;
	esp_chip_info(&chip_info);

	strcat_dynamic(&result, "\nChip Info:\n");
	strcat_dynamic(&result, dash);

	snprintf(buf, sizeof(buf), "Model: %s\n", get_chip_model(chip_info.model));
	strcat_dynamic(&result, buf);

	snprintf(buf, sizeof(buf), "Arch: %s\n", CONFIG_IDF_TARGET_ARCH);
	strcat_dynamic(&result, buf);

	snprintf(buf, sizeof(buf), "Cores: %d\n", chip_info.cores);
	strcat_dynamic(&result, buf);

	snprintf(buf, sizeof(buf), "Revision number: %d\n", chip_info.revision);
	strcat_dynamic(&result, buf);

	uint32_t features = chip_info.features;

	char binary_str[33]; // 32 bits + null terminator
	for (int i = 31; i >= 0; i--) {
		binary_str[31 - i] = (features & (1U << i)) ? '1' : '0';
	}
	binary_str[32] = '\0'; // Null terminate the string
	snprintf(buf, sizeof(buf), "Features Bitmap: %s\n", binary_str);
	strcat_dynamic(&result, buf);

	snprintf(buf, sizeof(buf), "Embedded Flash: %s\n", (features & CHIP_FEATURE_EMB_FLASH) ? "Yes" : "No");
	strcat_dynamic(&result, buf);

	snprintf(buf, sizeof(buf), "Embedded PSRAM: %s\n", (features & CHIP_FEATURE_EMB_PSRAM) ? "Yes" : "No");
	strcat_dynamic(&result, buf);

	snprintf(buf, sizeof(buf), "Wi-Fi 2.4GHz support: %s\n", (features & CHIP_FEATURE_WIFI_BGN) ? "Yes" : "No");
	strcat_dynamic(&result, buf);

	snprintf(buf, sizeof(buf), "IEEE 802.15.4 support: %s\n", (features & CHIP_FEATURE_IEEE802154) ? "Yes" : "No");
	strcat_dynamic(&result, buf);

	snprintf(buf, sizeof(buf), "Bluetooth Classic support: %s\n", (features & CHIP_FEATURE_BT) ? "Yes" : "No");
	strcat_dynamic(&result, buf);

	snprintf(buf, sizeof(buf), "Bluetooth LE (BLE) support: %s\n", (features & CHIP_FEATURE_BLE) ? "Yes" : "No");
	strcat_dynamic(&result, buf);

	return result;
}

char *get_freq_info(void)
{
	char *result = calloc(1, 1);
	char buf[BUFSIZ];

	rtc_cpu_freq_config_t freq_config;
	rtc_clk_cpu_freq_get_config(&freq_config);

	// Reporting the CPU clock source
	const char* clk_source_str;
	switch (freq_config.source) {
	case SOC_CPU_CLK_SRC_PLL:
		clk_source_str = "PLL";
		break;
	case SOC_CPU_CLK_SRC_APLL:
		clk_source_str = "APLL";
		break;
	case SOC_CPU_CLK_SRC_XTAL:
		clk_source_str = "XTAL";
		break;
	default:
		clk_source_str = "Unknown";
		break;
	}
	snprintf(buf, sizeof(buf), "CPU Clock Source: %s\n", clk_source_str);
	strcat_dynamic(&result, buf);

	snprintf(buf, sizeof(buf), "Source Clock Frequency: %" PRIu32 " MHz\n", freq_config.source_freq_mhz);
	strcat_dynamic(&result, buf);

	snprintf(buf, sizeof(buf), "Divider: %" PRIu32 "\n", freq_config.div);
	strcat_dynamic(&result, buf);

	snprintf(buf, sizeof(buf), "Effective CPU Frequency: %" PRIu32 " MHz\n", freq_config.freq_mhz);
	strcat_dynamic(&result, buf);
	strcat_dynamic(&result, dash);

	return result;
}

char *get_memory_info(void)
{
	char *result = calloc(1, BUFSIZ);
	char buf[BUFSIZ];

	strcat_dynamic(&result, "\nMemory Info:\n");
	strcat_dynamic(&result, dash);

	// Total SPIRAM (PSRAM) Size
	size_t spiram_size = heap_caps_get_total_size(MALLOC_CAP_SPIRAM);
	if (spiram_size)
	{
		snprintf(buf, sizeof(buf), "PSRAM Size: %zu bytes\n", spiram_size);
		strcat_dynamic(&result, buf);
	}
	else
		strcat_dynamic(&result, "No PSRAM detected\n");

	uint32_t total_internal_memory = heap_caps_get_total_size(MALLOC_CAP_INTERNAL);
	uint32_t free_internal_memory = heap_caps_get_free_size(MALLOC_CAP_INTERNAL);
	uint32_t largest_contig_internal_block = heap_caps_get_largest_free_block(MALLOC_CAP_INTERNAL);

	snprintf(buf, sizeof(buf), "Total DRAM (internal memory): %"PRIu32" bytes\n", total_internal_memory);
	strcat_dynamic(&result, buf);

	snprintf(buf, sizeof(buf), "Free DRAM (internal memory): %"PRIu32" bytes\n", free_internal_memory);
	strcat_dynamic(&result, buf);

	snprintf(buf, sizeof(buf), "Largest free contiguous DRAM block: %"PRIu32" bytes\n", largest_contig_internal_block);
	strcat_dynamic(&result, buf);

	strcat_dynamic(&result, dash);

	return result;
}

char *get_partition_tabel(void)
{
	char *result = calloc(1, 1);
	char buf[BUFSIZ];

	strcat_dynamic(&result, "\nPartition table:\n");
	strcat_dynamic(&result, dash);

	snprintf(buf, sizeof(buf), "%-17s Type ST %10s %10s\n", "Label", "Offset", "Length");
	strcat_dynamic(&result, buf);

	esp_partition_iterator_t it;

	it = esp_partition_find(ESP_PARTITION_TYPE_ANY, ESP_PARTITION_SUBTYPE_ANY, NULL);

	// Loop through all matching partitions, in this case, all with the type 'data' until partition with desired
	// label is found. Verify if its the same instance as the one found before.
	for (; it != NULL; it = esp_partition_next(it))
	{
		const esp_partition_t *part = esp_partition_get(it);
		snprintf(buf, sizeof(buf), "%-17s  %02d  %02d %#010x %#010x\n", part->label, part->type, part->subtype, (int)part->address, (int)part->size);
		strcat_dynamic(&result, buf);
	}
	// Release the partition iterator to release memory allocated for it
	esp_partition_iterator_release(it);
	strcat_dynamic(&result, dash);

	return result;
}

char *get_system_info(void)
{
	char *result = NULL;
	char *str = get_project_info();
	strcat_dynamic(&result, str);
	free(str);

	str = get_chip_info();
	strcat_dynamic(&result, str);
	free(str);

	str = get_freq_info();
	strcat_dynamic(&result, str);
	free(str);

	str = get_memory_info();
	strcat_dynamic(&result, str);
	free(str);

	str = get_partition_tabel();
	strcat_dynamic(&result, str);
	free(str);

	return result;
}

const char* get_chip_model(esp_chip_model_t model)
{
	switch (model) {
	case CHIP_ESP32:
		return "ESP32";
	case CHIP_ESP32S2:
		return "ESP32-S2";
	case CHIP_ESP32S3:
		return "ESP32-S3";
	case CHIP_ESP32C3:
		return "ESP32-C3";
	case CHIP_ESP32C2:
		return "ESP32-C2";
	case CHIP_ESP32C6:
		return "ESP32-C6";
	case CHIP_ESP32H2:
		return "ESP32-H2";
		//        case CHIP_ESP32P4:
		//            return "ESP32-P4";
	case CHIP_POSIX_LINUX:
		return "POSIX/Linux Simulator";
	default:
		return "Unknown Model";
	}
}
