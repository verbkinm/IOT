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

#define TYPE_STR						"type"
	#define TYPE_VALUE_STR				"sysinf"
	#define TYPE_STA_STATE_VALUE_STR	"sta_state"
	#define TYPE_SCAN_VALUE_STR			"scan"
	#define TYPE_SCAN_RESULT_VALUE_STR	"scan_result"
	#define TYPE_CONN_VALUE_STR			"conn"
	#define TYPE_OTA_VALUE_STR			"ota"

#define TEXT_STR						"text"
#define STA_SSID_STR					"ssid"
#define STA_PWD_STR						"pwd"
#define URL_STR							"url"
#define RESULT_STR						"result"

// CONN
#define STA_STATE_STR					"state"
	#define STA_STATE_ONLINE_VALUE_STR	"online"
	#define STA_STATE_OFFLINE_VALUE_STR	"offline"
	#define STA_STATE_CONNECTING_VALUE_STR	"connecting"

enum type {
	SYSINFO,
	STA_STATE,
	SCAN,
	SCAN_RESULT,
	CONN,
	OTA,
	UNKNOW
};

static int type_str_to_int(const char *str);
static char *sysinf_reply(void);
static char *sta_state_reply(void);
static char *conn_reply(const cJSON *root);
static char *ota_reply(const cJSON *root);
static char *scan_result_reply(void);

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

		cJSON *reply_root = cJSON_CreateObject();
		if (reply_root == NULL)
		{
			delete_ptr((void *)&url);
			return NULL;
		}

		cJSON *reply_type_text = cJSON_CreateString(TYPE_OTA_VALUE_STR);
		if (reply_type_text == NULL)
			goto reply_end;
		cJSON_AddItemToObject(reply_root, TYPE_STR, reply_type_text);

		cJSON *reply_url = cJSON_CreateString(url);
		if (reply_url == NULL)
			goto reply_end;
		cJSON_AddItemToObject(reply_root, URL_STR, reply_url);

		result = cJSON_Print(reply_root);

		reply_end:
		cJSON_Delete(reply_root);
		delete_ptr((void *)&url);
	}
	else
	{
		nvs_write_update_url(cJSON_GetStringValue(jurl));
	}

	return  result;
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
	char *result = NULL;

	cJSON *root = cJSON_CreateObject();
	if (root == NULL)
		return result;

	cJSON *type_text = cJSON_CreateString(TYPE_STA_STATE_VALUE_STR);
	if (type_text == NULL)
		goto end;
	cJSON_AddItemToObject(root, TYPE_STR, type_text);

	cJSON *state_text = cJSON_CreateString((glob_get_status_reg() & STATUS_WIFI_STA_CONNECTED) ? STA_STATE_ONLINE_VALUE_STR :
			(glob_get_status_reg() & STATUS_WIFI_STA_CONNECTING) ? STA_STATE_CONNECTING_VALUE_STR :STA_STATE_OFFLINE_VALUE_STR);

	if (state_text == NULL)
		goto end;
	cJSON_AddItemToObject(root, STA_STATE_STR, state_text);

	char *ssid = NULL;
	nvs_read_wifi_sta_ssid(&ssid);
	char *tmp_ssid = str_replace(ssid, "\"", "\\\"");
	delete_ptr((void *)&ssid);
	ssid = tmp_ssid;

	cJSON *ssid_text = cJSON_CreateString(ssid);
	if (ssid_text == NULL)
		goto end;
	cJSON_AddItemToObject(root, STA_SSID_STR, ssid_text);

	char *pwd = NULL;
	nvs_read_wifi_sta_pwd(&pwd);
	char *tmp_pwd = str_replace(pwd, "\"", "\\\"");
	delete_ptr((void *)&pwd);
	pwd = tmp_pwd;

	cJSON *pwd_text = cJSON_CreateString(pwd);
	if (pwd_text == NULL)
		goto end;
	cJSON_AddItemToObject(root, STA_PWD_STR, pwd_text);

	result = cJSON_Print(root);

	end:
	cJSON_Delete(root);
	delete_ptr((void *)&ssid);
	delete_ptr((void *)&pwd);
	return result;
}

static char *sysinf_reply(void)
{
	char *result = NULL;

	char *tmp_str = get_system_info();
	if (tmp_str == NULL)
		goto bad_end;

	char *tmp_str_repl = str_replace(tmp_str, "\"", "\\\"");
	if (tmp_str_repl == NULL)
		goto bad_end;

	cJSON *root = cJSON_CreateObject();
	if (root == NULL)
		goto bad_end;

	cJSON *type_text = cJSON_CreateString(TYPE_VALUE_STR);
	if (type_text == NULL)
		goto end;
	cJSON_AddItemToObject(root, TYPE_STR, type_text);

	cJSON *text_text = cJSON_CreateString(tmp_str_repl);
	if (text_text == NULL)
		goto end;
	cJSON_AddItemToObject(root, TEXT_STR, text_text);

	result = cJSON_Print(root);

	end:
	cJSON_Delete(root);

	bad_end:
	delete_ptr((void *)&tmp_str);
	delete_ptr((void *)&tmp_str_repl);

	return result;
}

static int type_str_to_int(const char *str)
{
	if(str == NULL)
		return UNKNOW;

	if(strcmp(str, TYPE_VALUE_STR) == 0)
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
	case UNKNOW:
	default:
		goto end;
	}

	end:
	cJSON_Delete(root);

	return result;
}
