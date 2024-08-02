/*
 * nvs.c
 *
 *  Created on: 7 мар. 2024 г.
 *      Author: user
 */

#include "local_nvs.h"

static const char *TAG = "local_nvs";

static esp_err_t nvs_read_u8(const char *namespace, const char *key, uint8_t *out);
static esp_err_t nvs_write_u8(const char *namespace, const char *key, uint8_t value);

static esp_err_t nvs_read_string(const char *namespace, const char *key, char *out);
static esp_err_t nvs_write_string(const char *namespace, const char *key, const char *value);

static esp_err_t nvs_write_u8(const char *namespace, const char *key, uint8_t value)
{
	if (namespace == NULL || key == NULL)
		return ESP_ERR_INVALID_ARG;

	nvs_handle_t my_handle;
	if (nvs_open(namespace, NVS_READWRITE, &my_handle) != ESP_OK)
	{
		ESP_LOGE(TAG, "%s open nvs failed", namespace);
		return ESP_FAIL;
	}

	if (nvs_set_u8(my_handle, key, value) != ESP_OK)
	{
		ESP_LOGE(TAG, "nvs_set_u8 failed");
		nvs_close(my_handle);
		return ESP_FAIL;
	}

	if (nvs_commit(my_handle) != ESP_OK)
	{
		ESP_LOGE(TAG, "nvs commit %s %s failed", namespace, key);
		nvs_close(my_handle);
		return ESP_FAIL;
	}

	nvs_close(my_handle);
	return ESP_OK;
}

static esp_err_t nvs_read_u8(const char *namespace, const char *key, uint8_t *out)
{
	if (namespace == NULL || key == NULL || out == NULL)
		return ESP_ERR_INVALID_ARG;

	nvs_handle_t my_handle;
	if (nvs_open(namespace, NVS_READONLY, &my_handle) != ESP_OK)
	{
		ESP_LOGE(TAG, "%s open nvs failed", namespace);
		return ESP_FAIL;
	}

	if (nvs_get_u8(my_handle, key, out) != ESP_OK)
	{
		ESP_LOGE(TAG, "nvs_get_u8 %s failed", key);
		nvs_close(my_handle);
		return ESP_FAIL;
	}

	nvs_close(my_handle);
	return ESP_OK;
}

static esp_err_t nvs_read_string(const char *namespace, const char *key, char *out)
{
	if (namespace == NULL || key == NULL)
		return ESP_ERR_INVALID_ARG;

	out = calloc(1, BUFSIZE);
	if (out == NULL)
	{
		ESP_LOGE(TAG, "calloc failed");
		return ESP_ERR_NO_MEM;
	}

	nvs_handle_t my_handle;
	if (nvs_open(namespace, NVS_READONLY, &my_handle) != ESP_OK)
	{
		ESP_LOGE(TAG, "%s open nvs failed", namespace);
		free(out);
		return ESP_FAIL;
	}

	size_t size = BUFSIZE - 1;
	if (nvs_get_str(my_handle, key, out, &size) != ESP_OK)
	{
		ESP_LOGE(TAG, "nvs_get_str %s failed", key);
		free(out);
		nvs_close(my_handle);
		return ESP_FAIL;
	}

	nvs_close(my_handle);
	return ESP_OK;
}

static esp_err_t nvs_write_string(const char *namespace, const char *key, const char *value)
{
	if (namespace == NULL || key == NULL || value)
		return ESP_ERR_INVALID_ARG;

	nvs_handle_t my_handle;
	if (nvs_open(namespace, NVS_READWRITE, &my_handle) != ESP_OK)
	{
		ESP_LOGE(TAG, "%s open nvs failed", namespace);
		return ESP_FAIL;
	}

	if (nvs_set_str(my_handle, key, value) != ESP_OK)
	{
		ESP_LOGE(TAG, "nvs_set_str failed");
		nvs_close(my_handle);
		return ESP_FAIL;
	}

	if (nvs_commit(my_handle) != ESP_OK)
	{
		ESP_LOGE(TAG, "nvs commit %s %s failed", namespace, key);
		nvs_close(my_handle);
		return ESP_FAIL;
	}

	nvs_close(my_handle);
	return ESP_OK;
}

esp_err_t nvs_read_update_flag(uint8_t *out)
{
	return nvs_read_u8(NVS_NAMESPACE_UPDATE, NVS_KEY_UPDATE_FLAG, out);
}

esp_err_t nvs_read_update_url(char *out)
{
	return nvs_read_string(NVS_NAMESPACE_UPDATE, NVS_KEY_UPDATE_URL, out);
}

esp_err_t nvs_write_update_flag(uint8_t value)
{
	return nvs_write_u8(NVS_NAMESPACE_UPDATE, NVS_KEY_UPDATE_FLAG, value);
}

esp_err_t nvs_write_update_url(const char *value)
{
	return nvs_write_string(NVS_NAMESPACE_UPDATE, NVS_KEY_UPDATE_URL, value);
}

esp_err_t nvs_read_wifi_sta_ssid(char *out)
{
	return nvs_read_string(NVS_NAMESPACE_WIFI, NVS_KEY_WIFI_STA_SSID, out);
}

esp_err_t nvs_write_wifi_sta_ssid(const char *value)
{
	return nvs_write_string(NVS_NAMESPACE_WIFI, NVS_KEY_WIFI_STA_SSID, value);
}

esp_err_t nvs_read_wifi_sta_pwd(char *out)
{
	return nvs_read_string(NVS_NAMESPACE_WIFI, NVS_KEY_WIFI_STA_PWD, out);
}

esp_err_t nvs_write_wifi_sta_pwd(const char *value)
{
	return nvs_write_string(NVS_NAMESPACE_WIFI, NVS_KEY_WIFI_STA_PWD, value);
}
