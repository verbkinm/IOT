/*
 * web.c
 *
 *  Created on: 27 сент. 2023 г.
 *      Author: user
 */
#include "esp_log.h"

#include "web.h"
#include "wifi/wifi.h"

#include <sys/param.h>

#include "esp_event.h"
#include "esp_log.h"
#include "esp_mac.h"

// Глобальные объекты
//extern bool global_wifi_sta_connect;
extern iotv_wifi_status_t global_wifi_sta_status;

extern const unsigned char index_start[] asm("_binary_index_html_start");
extern const unsigned char index_end[] asm("_binary_index_html_end");

extern const unsigned char connecting_start[] asm("_binary_connecting_html_start");
extern const unsigned char connecting_end[] asm("_binary_connecting_html_end");

extern const unsigned char error_connect_start[] asm("_binary_error_html_start");
extern const unsigned char error_connect_end[]   asm("_binary_error_html_end");

extern const unsigned char ota_start[] asm("_binary_ota_html_start");
extern const unsigned char iota_end[] asm("_binary_ota_html_end");

extern const unsigned char favicon_start[] asm("_binary_favicon_ico_start");
extern const unsigned char favicon_end[] asm("_binary_favicon_ico_end");

// Приватные свойства
static const char *TAG = "web";

// Приватные функцие
static esp_err_t root_get_handler(httpd_req_t *req);
static esp_err_t cmd_get_handler(httpd_req_t *req);
static esp_err_t favicon_get_handler(httpd_req_t *req);
static esp_err_t http_404_error_handler(httpd_req_t *req, httpd_err_code_t err);
static char *parseUri(const char* uri, size_t *resultSize, const char *searchStr);

// Приватные объекты использующие функции описанные выше
static const httpd_uri_t root = {
		.uri = "/",
		.method = HTTP_GET,
		.handler = root_get_handler
};

static const httpd_uri_t cmd = {
		.uri = "/cmd",
		.method = HTTP_GET,
		.handler = cmd_get_handler
};

static const httpd_uri_t favicon = {
		.uri = "/favicon.ico",
		.method = HTTP_GET,
		.handler = favicon_get_handler
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

	char *index = NULL;
	size_t indexSize = 0;

	if (global_wifi_sta_status == IOTV_WIFI_STA_NULL)
	{
		index = (char *)index_start;
		indexSize = index_end - index_start;
	}
	else if (global_wifi_sta_status & IOTV_WIFI_STA_START_CONNECTING)
	{
		index = (char *)connecting_start;
		indexSize = connecting_end - connecting_start;
	}
	else if (global_wifi_sta_status & IOTV_WIFI_FAIL_CONNECT)
	{
		global_wifi_sta_status = IOTV_WIFI_STA_NULL;

		index = (char *)error_connect_start;
		indexSize = error_connect_end - error_connect_start;
	}
	else if (global_wifi_sta_status == IOTV_WIFI_STA_CONNECTED)
	{
		index = (char *)ota_start;
		indexSize = iota_end - ota_start;
	}

	httpd_resp_set_type(req, "text/html");
	httpd_resp_send(req, index, indexSize);

	//	httpd_resp_set_type(req, "image/x-icon");
	//	index = favicon_start;
	//	indexSize = favicon_end - favicon_start;
	//	httpd_resp_send(req, index, indexSize);

	return ESP_OK;
}

static esp_err_t cmd_get_handler(httpd_req_t *req)
{
	printf("uri - %s\n", req->uri);

	bool errArgs = false;
	size_t resultSize;
	size_t indexSize;
	char *result = parseUri(req->uri, &resultSize, "ssid=");

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

static esp_err_t favicon_get_handler(httpd_req_t *req)
{
	printf("uri - %s\n", req->uri);

	char *index = (char *)favicon_start;
	size_t indexSize = favicon_end - favicon_start;

	httpd_resp_set_type(req, "image/x-icon");
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
		httpd_register_uri_handler(server, &cmd);
		httpd_register_uri_handler(server, &favicon);

		httpd_register_err_handler(server, HTTPD_404_NOT_FOUND, http_404_error_handler);
	}
	return server;
}
