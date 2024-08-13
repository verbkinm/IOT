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
#include "esp_ota_ops.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "esp_wifi_types.h"
#include "esp_mac.h"

#include "esp_heap_caps.h"
#include "soc/rtc.h"	//real time clock

#include "Local_Lib/local_lib.h"

static const char *dash = "------------------------------------------------------------\n";

static char *network_info(const char *ifkey, esp_mac_type_t mac_type, const char *title);

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

static char *network_info(const char *ifkey, esp_mac_type_t mac_type, const char *title)
{
	esp_netif_t *netif = esp_netif_get_handle_from_ifkey(ifkey);

	char ip[16] = {0};
	char netmask[16] = {0};
	char dns_server[16] = {0};
	char gw[16] = {0};

	esp_netif_ip_info_t ip_info;
	esp_netif_get_ip_info(netif, &ip_info);

	esp_netif_dns_info_t dns;
	esp_netif_get_dns_info(netif, ESP_NETIF_DNS_MAIN, &dns);
	esp_ip_addr_t ipaddr;
	memcpy(&ipaddr.u_addr.ip4,  &dns.ip, 4);

	esp_ip4addr_ntoa(&ip_info.ip, ip, 15);
	esp_ip4addr_ntoa(&ip_info.netmask, netmask, 15);
	esp_ip4addr_ntoa(&ipaddr.u_addr.ip4, dns_server, 15);
	esp_ip4addr_ntoa(&ip_info.gw, gw, 15);

	uint8_t mac[6];
	esp_read_mac(mac, mac_type);

	char mac_str[19] = {0};
	snprintf(mac_str, sizeof(mac_str) - 1, MACSTR, MAC2STR(mac));

	char *result = calloc(1, 1);
	char buf[BUFSIZ];

	strcat_dynamic(&result, title);
	strcat_dynamic(&result, dash);

	snprintf(buf, sizeof(buf),
			"mac: %s\n"
			"ip: %s\n"
			"netmask: %s\n"
			"dns: %s\n"
			"gateway: %s\n",
			mac_str, ip, netmask, dns_server, gw);

	strcat_dynamic(&result, buf);

	if (mac_type == ESP_MAC_WIFI_SOFTAP)
	{
		wifi_sta_list_t sta;
		esp_wifi_ap_get_sta_list(&sta);

		esp_netif_pair_mac_ip_t pair[sta.num];
		for (int i = 0; i < sta.num; ++i)
			memcpy(pair[i].mac, sta.sta[i].mac, 6);

		strcat_dynamic(&result, "\nClients:\n");
		for (int i = 0; i < sta.num; ++i)
		{
			esp_netif_dhcps_get_clients_by_mac(netif, sta.num, pair);

			esp_ip4addr_ntoa(&(pair[i].ip), ip, 15);
			snprintf(buf, sizeof(buf), "%d: "MACSTR" - %s\n", i + 1,
					MAC2STR(sta.sta[i].mac), ip);
			strcat_dynamic(&result, buf);
		}
	}
	else if (mac_type == ESP_MAC_WIFI_STA)
	{
		wifi_config_t wifi_config;
		esp_wifi_get_config(WIFI_IF_STA, &wifi_config);

		char mac_str[18] = {0};
		char ssid_str[32] = {0};

		sprintf(mac_str, MACSTR, MAC2STR(wifi_config.sta.bssid));
		sprintf(ssid_str, "%s", wifi_config.sta.ssid);

		strcat_dynamic(&result, "\nConnected to \"");
		strcat_dynamic(&result, ssid_str);
		strcat_dynamic(&result, "\" (");
		strcat_dynamic(&result, mac_str);
		strcat_dynamic(&result, ")\n");
	}

	strcat_dynamic(&result, dash);

	return result;
}

char *get_network_info(void)
{
	char *result = calloc(1, 1);

	char *netinf = network_info("WIFI_AP_DEF", ESP_MAC_WIFI_SOFTAP, "\nWIFI AP:\n");
	strcat_dynamic(&result, netinf);
	free(netinf);

	netinf = network_info("WIFI_STA_DEF", ESP_MAC_WIFI_STA, "\nWIFI STA:\n");
	strcat_dynamic(&result, netinf);
	free(netinf);

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
	char buf[BUFSIZE];

	strcat_dynamic(&result, "\nPartition table:\n");
	strcat_dynamic(&result, dash);

	snprintf(buf, sizeof(buf), "%-18s Type ST %10s %10s\n", "Label", "Offset", "Length");
	strcat_dynamic(&result, buf);

	esp_partition_iterator_t it;

	it = esp_partition_find(ESP_PARTITION_TYPE_ANY, ESP_PARTITION_SUBTYPE_ANY, NULL);

	// Loop through all matching partitions, in this case, all with the type 'data' until partition with desired
	// label is found. Verify if its the same instance as the one found before.
	for (; it != NULL; it = esp_partition_next(it))
	{
		char boot_label = ' ';
		const esp_partition_t *part = esp_partition_get(it);
		if (part == esp_ota_get_boot_partition())
			boot_label = '*';
		snprintf(buf, sizeof(buf), "%c%-17s  %02d  %02d %#010x %#010x\n", boot_label, part->label, part->type, part->subtype, (int)part->address, (int)part->size);
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

	str = get_network_info();
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
