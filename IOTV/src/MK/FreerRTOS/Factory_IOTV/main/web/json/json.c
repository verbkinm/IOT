/*
 * json.c
 *
 *  Created on: 7 авг. 2024 г.
 *      Author: user
 */

#include "json.h"

#include "cJSON.h"
#include "esp_mac.h"
#include "Local_Lib/system_info/system_info.h"
#include "Local_Lib/local_lib.h"
#include "global/registers.h"
#include "nvs/local_nvs.h"
#include "service/wifi/wifi.h"
#include "service/update/update.h"
#include "ota/ota.h"

#define TYPE_STR						"type"
	#define TYPE_SYSINF_VALUE_STR		"sysinf"
	#define TYPE_STA_STATE_VALUE_STR	"sta_state"
	#define TYPE_SCAN_VALUE_STR			"scan"
	#define TYPE_SCAN_RESULT_VALUE_STR	"scan_result"
	#define TYPE_CONN_VALUE_STR			"conn"
	#define TYPE_OTA_VALUE_STR			"ota"
	#define TYPE_FIRMWARE_VALUE_STR		"firmware"

#define TEXT_STR						"text"
#define STA_SSID_STR					"ssid"
#define STA_PWD_STR						"pwd"
#define URL_STR							"url"
#define RESULT_STR						"result"
#define OK_STR							"ok"
#define ERR_STR							"err"
#define PER_STR							"per"

// CONN
#define STA_STATE_STR					"state"
	#define STA_STATE_ONLINE_VALUE_STR	"online"
	#define STA_STATE_OFFLINE_VALUE_STR	"offline"
	#define STA_STATE_CONNECTING_VALUE_STR	"connecting"

//FIRMWARE
#define DATA_STR						"data"
#define SIZE_STR						"size"
#define PKG_STR							"pkg"
#define PKGS_STR						"pkgs"

enum type {
	SYSINFO,
	STA_STATE,
	SCAN,
	SCAN_RESULT,
	CONN,
	OTA,
	FIRMWARE,
	UNKNOW
};

struct pair {
	char *first;
	char *second;
}typedef pair_t;

static const char *TAG = "JSON";

static char *parsePairs(const pair_t *arr, size_t size);

static int type_str_to_int(const char *str);
static char *sysinf_reply(void);
static char *sta_state_reply(void);
static char *conn_reply(const cJSON *root);
static char *ota_reply(const cJSON *root);
static char *firmware_reply(const cJSON *root);
static char *scan_result_reply(void);

static char *parsePairs(const pair_t *arr, size_t size)
{
	if (arr == NULL || size == 0)
		return NULL;

	char *result = NULL;

	cJSON *root = cJSON_CreateObject();
	if (root == NULL)
		return result;


	for (size_t i = 0; i < size; ++i)
	{
		cJSON *obj = cJSON_CreateString(arr[i].second);
		if (obj == NULL)
			goto end;
		cJSON_AddItemToObject(root, arr[i].first, obj);
	}

	result = cJSON_Print(root);

	end:
	cJSON_Delete(root);
	return result;
}

static char *scan_result_reply(void)
{
	char *result = NULL;

	cJSON *root = cJSON_CreateObject();
	if (root == NULL)
		return result;

	cJSON *type_text = cJSON_CreateString(TYPE_SCAN_RESULT_VALUE_STR);
	if (type_text == NULL)
		goto end;
	cJSON_AddItemToObject(root, TYPE_STR, type_text);

	cJSON *arr = cJSON_CreateArray();
	if (root == NULL)
		goto end;
	cJSON_AddItemToObject(root, RESULT_STR, arr);

	const wifi_ap_record_t *ap_record = wifi_service_last_scan();

	for (int i = 0; i < AP_INFO_ARR_SIZE; i++)
	{
		if (ap_record[i].bssid[0] == 0)
			continue;

		char ssid[sizeof(ap_record[i].ssid) + 1] = {0};
		memcpy(ssid, ap_record[i].ssid, sizeof(ap_record[i].ssid));

		char bssid[19] = {0};
		snprintf(bssid, 18, MACSTR, MAC2STR(ap_record[i].bssid));

		cJSON *pair = cJSON_CreateArray();
		cJSON *jssid = cJSON_CreateString(ssid);
		cJSON *jbssid = cJSON_CreateString(bssid);

		cJSON_AddItemToArray(pair, jssid);
		cJSON_AddItemToArray(pair, jbssid);

		cJSON_AddItemToArray(arr, pair);
	}

	result = cJSON_Print(root);

	end:
	cJSON_Delete(root);
	return result;
}

static char *ota_reply(const cJSON *root)
{
	char *result = NULL;

	if (root == NULL)
		return NULL;

	cJSON *jurl = cJSON_GetObjectItemCaseSensitive(root, URL_STR);

	if (jurl == NULL)
	{
		char *url = NULL;
		nvs_read_update_url(&url);
		if (url == NULL)
			strcat_dynamic(&url, " ");

		pair_t pairs[] = {
				{TYPE_STR, TYPE_OTA_VALUE_STR},
				{URL_STR, url}
		};

		result = parsePairs(pairs, 2);
		delete_ptr((void *)&url);
	}
	else
	{
		char *url = cJSON_GetStringValue(jurl);
		nvs_write_update_url(url);
		pair_t pairs[] = {
				{TYPE_STR, TYPE_OTA_VALUE_STR},
				{RESULT_STR, ERR_STR}
		};
		if (ota_firmware(url) == ESP_OK)
			pairs[1].second = OK_STR;

		result = parsePairs(pairs, 2);
	}

	return  result;
}

static char *firmware_reply(const cJSON *root)
{
	char *result = NULL;

	if (root == NULL)
		return result;

	cJSON *j_data = cJSON_GetObjectItemCaseSensitive(root, DATA_STR);
	cJSON *j_size = cJSON_GetObjectItemCaseSensitive(root, SIZE_STR);
	cJSON *j_pkg = cJSON_GetObjectItemCaseSensitive(root, PKG_STR);
	cJSON *j_pkgs = cJSON_GetObjectItemCaseSensitive(root, PKGS_STR);

	if (j_data == NULL || j_size == NULL || j_pkg == NULL || j_pkgs == NULL)
		return result;

	size_t dataSize = cJSON_GetNumberValue(j_size);
	char *data = malloc(dataSize);
	size_t pkg = cJSON_GetNumberValue(j_pkg);
	size_t pkgs = cJSON_GetNumberValue(j_pkgs);

	size_t i = 0;
	cJSON *element;
	cJSON_ArrayForEach(element, j_data)
	{
		data[i++] = cJSON_GetNumberValue(element);
	}

	if (update_service_firmware(data, dataSize, pkg, pkgs) != ESP_OK)
	{
		pair_t pairs[] = {
				{TYPE_STR, TYPE_OTA_VALUE_STR},
				{RESULT_STR, ERR_STR}
		};
		result = parsePairs(pairs, 2);
	}
	else
	{
		pair_t pairs[] = {
				{TYPE_STR, TYPE_OTA_VALUE_STR},
				{PER_STR, "0"}
		};

        int totalPer = 100.0 * pkg / pkgs;
        char digit[4] = {0};
        snprintf(digit, sizeof(digit) - 1, "%d", totalPer);

        if (pkg == pkgs)
        	pairs[1].second = "100";
        else
        	pairs[1].second = digit;

        result = parsePairs(pairs, 2);
	}

	free(data);
	return result;
}

static char *conn_reply(const cJSON *root)
{
	if (root == NULL)
		return NULL;

	cJSON *ssid = cJSON_GetObjectItemCaseSensitive(root, STA_SSID_STR);
	cJSON *pwd = cJSON_GetObjectItemCaseSensitive(root, STA_PWD_STR);

	if (ssid == NULL || pwd == NULL)
		return NULL;

	char *ssid_str = cJSON_GetStringValue(ssid);
	char *pwd_str = cJSON_GetStringValue(pwd);

	if (ssid_str == NULL || pwd_str == NULL)
		return NULL;

	nvs_write_wifi_sta_ssid(ssid_str);
	nvs_write_wifi_sta_pwd(pwd_str);

	wifi_service_conntect_to_AP(ssid_str, pwd_str);

	return  NULL;
}
static char *sta_state_reply(void)
{
	char *ssid = NULL;
	nvs_read_wifi_sta_ssid(&ssid);
	char *tmp_ssid = str_replace(ssid, "\"", "\\\"");
	delete_ptr((void *)&ssid);
	ssid = tmp_ssid;

	char *pwd = NULL;
	nvs_read_wifi_sta_pwd(&pwd);
	char *tmp_pwd = str_replace(pwd, "\"", "\\\"");
	delete_ptr((void *)&pwd);
	pwd = tmp_pwd;

	char *state = (glob_get_status_reg() & STATUS_WIFI_STA_CONNECTED) ? STA_STATE_ONLINE_VALUE_STR :
			(glob_get_status_reg() & STATUS_WIFI_STA_CONNECTING) ? STA_STATE_CONNECTING_VALUE_STR : STA_STATE_OFFLINE_VALUE_STR;

	pair_t pairs[] = {
			{TYPE_STR, TYPE_STA_STATE_VALUE_STR},
			{STA_STATE_STR, state},
			{STA_SSID_STR, ssid},
			{STA_PWD_STR, pwd}
	};

	char *result = NULL;
	result = parsePairs(pairs, 4);

	delete_ptr((void *)&ssid);
	delete_ptr((void *)&pwd);

	return result;
}

static char *sysinf_reply(void)
{
	char *tmp_str = get_system_info();
	char *tmp_str_repl = str_replace(tmp_str, "\"", "\\\"");
	delete_ptr((void *)&tmp_str);

	pair_t pairs[] = {
			{TYPE_STR, TYPE_SYSINF_VALUE_STR},
			{TEXT_STR, tmp_str_repl}
	};

	char *result = NULL;
	result = parsePairs(pairs, 2);

	delete_ptr((void *)&tmp_str_repl);

	return result;
}

static int type_str_to_int(const char *str)
{
	if(str == NULL)
		return UNKNOW;

	if(strcmp(str, TYPE_SYSINF_VALUE_STR) == 0)
		return SYSINFO;
	if(strcmp(str, TYPE_STA_STATE_VALUE_STR) == 0)
		return STA_STATE;
	if(strcmp(str, TYPE_SCAN_VALUE_STR) == 0)
		return SCAN;
	if(strcmp(str, TYPE_SCAN_RESULT_VALUE_STR) == 0)
		return SCAN_RESULT;
	if(strcmp(str, TYPE_CONN_VALUE_STR) == 0)
		return CONN;
	if(strcmp(str, TYPE_OTA_VALUE_STR) == 0)
		return OTA;
	if(strcmp(str, TYPE_FIRMWARE_VALUE_STR) == 0)
		return FIRMWARE;

	return UNKNOW;
}

char *parse_request(const char *content)
{
	char *result = NULL;

	cJSON *root = cJSON_Parse(content);
	if (root == NULL)
		return NULL;

	cJSON *type = cJSON_GetObjectItemCaseSensitive(root, TYPE_STR);
	if (type == NULL)
		goto end;

	char *type_str = cJSON_GetStringValue(type);

	switch(type_str_to_int(type_str))
	{
	case SYSINFO:
		result = sysinf_reply();
		break;
	case STA_STATE:
		result = sta_state_reply();
		break;
	case SCAN:
		wifi_service_scan_wifi();
		break;
	case SCAN_RESULT:
		result = scan_result_reply();
		break;
	case CONN:
		result = conn_reply(root);
		break;
	case OTA:
		result = ota_reply(root);
		break;
	case FIRMWARE:
		result = firmware_reply(root);
		break;
	case UNKNOW:
	default:
		printf("%s unknow request", TAG);
		goto end;
	}

	end:
	cJSON_Delete(root);

	return result;
}
