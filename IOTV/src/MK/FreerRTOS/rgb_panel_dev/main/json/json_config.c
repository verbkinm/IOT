/*
 * json_config.c
 *
 *  Created on: 11 дек. 2023 г.
 *      Author: user
 */

#include "json_config.h"

#define WIFI_CONF_PATH	"/sdcard/wifi.jsn"
#define SNTP_CONF_PATH	"/sdcard/sntp.jsn"

static bool set_config_value(const char* file_path, const char *group, const char *key, const char *value);
static bool get_config_value(const char* file_path, const char *group, const char *key, char **value);

static bool set_config_value(const char* file_path, const char *group, const char *key, const char *value)
{
	FILE *file = fopen(file_path, "r");
	if (!file)
	{
		fprintf(stderr, "File %s not open\n", file_path);
		return false;
	}

	char *content = calloc(1, BUFSIZ);
	if (content == NULL)
		return false;

	fread(content, BUFSIZ, 1, file);
	fclose(file);

	cJSON *monitor = cJSON_Parse(content);
	free(content);

	cJSON *obj = cJSON_GetObjectItemCaseSensitive(monitor, group);
	if (obj == NULL)
		goto bad_end;

	cJSON *jkey = cJSON_GetObjectItemCaseSensitive(obj, key);
	if (jkey == NULL)
		goto bad_end;

	cJSON_SetValuestring(jkey, value);

	size_t len = strlen(cJSON_GetStringValue(jkey));
	if (len == 0)
		goto bad_end;

	char *print = NULL;
	print = cJSON_Print(monitor);

	if (print == NULL)
		goto bad_end;

	file = fopen(file_path, "w");
	if (file == NULL)
		goto bad_end;

	fprintf(file, "%s", print);
	fclose(file);

	cJSON_Delete(monitor);
	free(print);
	return true;

	bad_end:
	cJSON_Delete(monitor);
	return false;
}

static bool get_config_value(const char* file_path, const char *group, const char *key, char **value)
{
	if (key == NULL)
		return false;

	FILE *file = fopen(file_path, "r");
	if (!file)
	{
		fprintf(stderr, "File %s not open\n", file_path);
		return false;
	}

	char *content = calloc(1, BUFSIZ);
	if (content == NULL)
		return false;

	fread(content, BUFSIZ, 1, file);
	fclose(file);

	cJSON *monitor = cJSON_Parse(content);
	free(content);

	cJSON *obj = cJSON_GetObjectItemCaseSensitive(monitor, group);
	if (obj == NULL)
		goto bad_end;

	cJSON *jkey = cJSON_GetObjectItemCaseSensitive(obj, key);
	if (jkey == NULL)
		goto bad_end;

	size_t len = strlen(cJSON_GetStringValue(jkey));
	if (len == 0)
		goto bad_end;

	*value = calloc(1, len + 1);
	if (*value == NULL)
		goto bad_end;

	memcpy(*value, cJSON_GetStringValue(jkey), len);

	cJSON_Delete(monitor);
	return true;

	bad_end:
	cJSON_Delete(monitor);
	return false;
}

bool set_wifi_config_value(const char *key, const char *value)
{
	return set_config_value(WIFI_CONF_PATH, "wifi", key, value);
}

bool get_wifi_config_value(const char *key, char **value)
{
	return get_config_value(WIFI_CONF_PATH, "wifi", key, value);
}

bool set_sntp_config_value(const char *key, const char *value)
{
	return set_config_value(SNTP_CONF_PATH, "sntp", key, value);
}

bool get_sntp_config_value(const char *key, char **value)
{
	return get_config_value(SNTP_CONF_PATH, "sntp", key, value);
}

