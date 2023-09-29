/*
 * web.c
 *
 *  Created on: 27 сент. 2023 г.
 *      Author: user
 */
#include "esp_log.h"

#include "web.h"
#include "wifi/wifi.h"
#include "ota/ota.h"
#include "lib/urldecode.h"

#include <sys/param.h>

#include "esp_event.h"
#include "esp_log.h"
#include "esp_mac.h"

// Глобальные объекты
extern iotv_wifi_status_t global_wifi_sta_status;
extern iotv_ota_status_t global_ota_status;

// HTML страницы
extern const unsigned char index_start[] asm("_binary_index_html_start");
extern const unsigned char index_end[] asm("_binary_index_html_end");

extern const unsigned char connecting_start[] asm("_binary_connecting_html_start");
extern const unsigned char connecting_end[] asm("_binary_connecting_html_end");

extern const unsigned char error_connect_start[] asm("_binary_error_html_start");
extern const unsigned char error_connect_end[]   asm("_binary_error_html_end");

// OTA
extern const unsigned char ota_start[] asm("_binary_ota_html_start");
extern const unsigned char ota_end[] asm("_binary_ota_html_end");

extern const unsigned char ota_downloaded_start[] asm("_binary_ota_downloaded_html_start");
extern const unsigned char ota_downloaded_end[] asm("_binary_ota_downloaded_html_end");

extern const unsigned char ota_downloading_start[] asm("_binary_ota_downloading_html_start");
extern const unsigned char ota_downloading_end[] asm("_binary_ota_downloading_html_end");

extern const unsigned char ota_fail_start[] asm("_binary_ota_fail_html_start");
extern const unsigned char ota_fail_end[] asm("_binary_ota_fail_html_end");

// FAVICON
extern const unsigned char favicon_start[] asm("_binary_favicon_ico_start");
extern const unsigned char favicon_end[] asm("_binary_favicon_ico_end");

// Приватные свойства
static const char *TAG = "web";
static char *otaDecodeUrl = NULL;

// Приватные функцие
static esp_err_t root_get_handler(httpd_req_t *req);
static esp_err_t favicon_get_handler(httpd_req_t *req);
static esp_err_t ota_get_handler(httpd_req_t *req);
static esp_err_t cap_handler(httpd_req_t *req);
static esp_err_t http_404_error_handler(httpd_req_t *req, httpd_err_code_t err);

static esp_err_t cmd_post_handler(httpd_req_t *req);
static esp_err_t ota_post_handler(httpd_req_t *req);

static char *parseUri(const char* uri, size_t *resultSize, const char *searchStr);

// Приватные объекты использующие функции описанные выше
static const httpd_uri_t root = {
		.uri = "/",
		.method = HTTP_GET,
		.handler = root_get_handler
};

static const httpd_uri_t cmd_get = {
		.uri = "/cmd",
		.method = HTTP_GET,
		.handler = cap_handler
};

static const httpd_uri_t favicon = {
		.uri = "/favicon.ico",
		.method = HTTP_GET,
		.handler = favicon_get_handler
};

static const httpd_uri_t ota_get = {
		.uri = "/ota",
		.method = HTTP_GET,
		.handler = ota_get_handler
};

static const httpd_uri_t cmd_post = {
		.uri = "/cmd",
		.method = HTTP_POST,
		.handler = cmd_post_handler
};

static const httpd_uri_t ota_post = {
		.uri = "/ota",
		.method = HTTP_POST,
		.handler = ota_post_handler
};

//Реализация приватных функций
static char *parseUri(const char* uri, size_t *resultSize, const char *searchStr)
{
	if (uri == NULL || searchStr == NULL || resultSize == NULL)
		return NULL;

	char *result = NULL;
	*resultSize = 0;

	char *start = strstr(uri, searchStr);
	if (start == NULL)
		return NULL;

	start += strlen(searchStr);
	char *stop = strstr(start, "&");
	if (stop == NULL)
		stop = (char *)uri + strlen(uri);

	result = start;
	*resultSize = stop - start;

	return result;
}

// HTTP GET Handler
static esp_err_t root_get_handler(httpd_req_t *req)
{
	printf("uri - %s\n", req->uri);

	char *index = (char *)index_start;
	size_t indexSize = index_end - index_start;

	if (global_ota_status != IOTV_OTA_NULL || global_wifi_sta_status == IOTV_WIFI_STA_CONNECTED)
	{
		httpd_resp_set_status(req, "302 Temporary Redirect");
		httpd_resp_set_hdr(req, "Location", "/ota");
		httpd_resp_send(req, "Redirect to the captive portal", HTTPD_RESP_USE_STRLEN);

		ESP_LOGI(TAG, "Redirecting to /ota");
		return ESP_OK;
	}
	else if (global_wifi_sta_status == IOTV_WIFI_STA_START_CONNECTING)
	{
		index = (char *)connecting_start;
		indexSize = connecting_end - connecting_start;
	}
	else if (global_wifi_sta_status == IOTV_WIFI_FAIL_CONNECT)
	{
		global_wifi_sta_status = IOTV_WIFI_STA_NULL;

		index = (char *)error_connect_start;
		indexSize = error_connect_end - error_connect_start;
	}

	httpd_resp_set_type(req, "text/html");
	httpd_resp_send(req, index, indexSize);

	return ESP_OK;
}

static esp_err_t cmd_post_handler(httpd_req_t *req)
{
	printf("uri - %s\n", req->uri);

	char content[100] = {};
	size_t recv_size = MIN(req->content_len, sizeof(content));

	if (httpd_req_recv(req, content, recv_size) <= 0)
		return cap_handler(req);

	char *decodeText = urlDecode(content);
	memcpy(content, decodeText, strlen(decodeText));
	free(decodeText);

	ESP_LOGE(TAG, "content: %s", content);

	bool errArgs = false;
	size_t resultSize;
	size_t indexSize;
	char *result = parseUri(content, &resultSize, "ssid=");

	if (result == NULL || resultSize > WIFI_SSID_MAX_LENGH)
	{
		errArgs = true;
		resultSize = WIFI_SSID_MAX_LENGH;
		goto ARGS_ERROR;
	}

	char ssid[WIFI_SSID_MAX_LENGH] = {};
	memcpy(ssid, result, resultSize);

	result = parseUri(result, &resultSize, "pass=");
	if (result == NULL || resultSize > WIFI_PASS_MAX_LENGH)
	{
		errArgs = true;
		resultSize = WIFI_PASS_MAX_LENGH;
		goto ARGS_ERROR;
	}

	char pass[WIFI_PASS_MAX_LENGH] = {};
	memcpy(pass, result, resultSize);

	if (strlen(ssid) == 0 || strlen(pass) == 0)
		errArgs = true;

	if (!errArgs)
	{
		printf("ssid = %s\n", ssid);
		printf("pass = %s\n", pass);

		if (global_wifi_sta_status == IOTV_WIFI_STA_NULL)
			wifi_sta_connect(ssid, pass);

		char *index = (char *)connecting_start;
		indexSize = connecting_end - connecting_start;

		httpd_resp_set_type(req, "text/html");
		httpd_resp_send(req, index, indexSize);

		return ESP_OK;
	}

	ARGS_ERROR:
	printf("cmd handler args error\n");

	char *index = (char *)error_connect_start;
	indexSize = error_connect_end - error_connect_start;

	httpd_resp_set_type(req, "text/html");
	httpd_resp_send(req, index, indexSize);

	return ESP_OK;
}

static esp_err_t ota_post_handler(httpd_req_t *req)
{
	printf("uri - %s\n", req->uri);

	if (global_wifi_sta_status != IOTV_WIFI_STA_CONNECTED && global_ota_status != IOTV_OTA_NULL)
		return ESP_OK;

	char content[100] = {};
	size_t recv_size = MIN(req->content_len, sizeof(content));

	if (httpd_req_recv(req, content, recv_size) <= 0)
		return cap_handler(req);

	ESP_LOGE(TAG, "content: %s", content);

	size_t resultSize;
	char *result = parseUri(content, &resultSize, "firmware=");

	if (result == NULL)
		return cap_handler(req);

	otaDecodeUrl = urlDecode(result);

	global_ota_status = IOTV_OTA_DOWNLOADING;
	cap_handler(req);


	return ESP_OK;
}

static esp_err_t favicon_get_handler(httpd_req_t *req)
{
	printf("uri - %s\n", req->uri);

	char *index = (char *)favicon_start;
	size_t indexSize = favicon_end - favicon_start;

	httpd_resp_set_type(req, "image/x-icon");
	httpd_resp_send(req, index, indexSize);

	return ESP_OK;
}

static esp_err_t ota_get_handler(httpd_req_t *req)
{
	printf("uri - %s\n", req->uri);

	char *index = (char *)index_start;
	size_t indexSize = index_end - index_start;

	if (global_ota_status == IOTV_OTA_NULL && global_wifi_sta_status == IOTV_WIFI_STA_CONNECTED)
	{
		index = (char *)ota_start;
		indexSize = ota_end - ota_start;
	}
	else if (global_ota_status == IOTV_OTA_DOWNLOADING)
	{
		index = (char *)ota_downloading_start;
		indexSize = ota_downloading_end - ota_downloading_start;

		httpd_resp_set_type(req, "text/html");
		httpd_resp_send(req, index, indexSize);

		if (otaDecodeUrl != NULL)
		{
			ota_firmware(otaDecodeUrl);
			free(otaDecodeUrl);
		}

		return ESP_OK;
	}
	else if (global_ota_status == IOTV_OTA_DOWNLOADED)
	{
		index = (char *)ota_downloaded_start;
		indexSize = ota_downloaded_end - ota_downloaded_start;
		global_ota_status = IOTV_OTA_REBOOTING;
	}
	else if (global_ota_status == IOTV_OTA_REBOOTING)
		esp_restart();
	else if (global_ota_status == IOTV_OTA_FAIL)
	{
		index = (char *)ota_fail_start;
		indexSize = ota_fail_end - ota_fail_start;

		global_ota_status = IOTV_OTA_NULL;
	}
	else
		return cap_handler(req);

	httpd_resp_set_type(req, "text/html");
	httpd_resp_send(req, index, indexSize);

	return ESP_OK;
}

// HTTP Error (404) Handler - Redirects all requests to the root page
static esp_err_t http_404_error_handler(httpd_req_t *req, httpd_err_code_t err)
{
	// Set status
	httpd_resp_set_status(req, "302 Temporary Redirect");
	// Redirect to the "/" root directory
	httpd_resp_set_hdr(req, "Location", "/");
	// iOS requires content in the response to detect a captive portal, simply redirecting is not sufficient.
	httpd_resp_send(req, "Redirect to the captive portal", HTTPD_RESP_USE_STRLEN);

	ESP_LOGI(TAG, "Redirecting to root");
	return ESP_OK;
}

static esp_err_t cap_handler(httpd_req_t *req)
{
	return http_404_error_handler(req, 0);
}

//Реализация публичных функций
httpd_handle_t start_webserver(void)
{
	httpd_handle_t server = NULL;
	httpd_config_t config = HTTPD_DEFAULT_CONFIG();
	config.max_open_sockets = 13;
	config.lru_purge_enable = true;

	// Start the httpd server
	ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
	if (httpd_start(&server, &config) == ESP_OK)
	{
		// Set URI handlers
		ESP_LOGI(TAG, "Registering URI handlers");
		httpd_register_uri_handler(server, &root);
		httpd_register_uri_handler(server, &cmd_get);
		httpd_register_uri_handler(server, &favicon);
		httpd_register_uri_handler(server, &ota_get);

		httpd_register_uri_handler(server, &cmd_post);
		httpd_register_uri_handler(server, &ota_post);

		httpd_register_err_handler(server, HTTPD_404_NOT_FOUND, http_404_error_handler);
	}
	return server;
}
