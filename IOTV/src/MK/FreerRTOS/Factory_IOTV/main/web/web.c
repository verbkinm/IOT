/*
 * web.c
 *
 *  Created on: 27 сент. 2023 г.
 *      Author: user
 */
#include "esp_log.h"
#include <sys/param.h>
#include "esp_event.h"
#include "esp_log.h"
#include "esp_mac.h"
#include "esp_wifi.h"

#include "global/global_def.h"
#include "global/registers.h"
#include "web.h"
#include "ota/ota.h"
#include "Local_Lib/system_info/system_info.h"
#include "Local_Lib/local_lib.h"
#include "nvs/local_nvs.h"
#include "service/wifi/wifi.h"
#include "web/ws/ws.h"

// HTML страницы
extern const unsigned char index_start[] asm("_binary_index_html_start");
extern const unsigned char index_end[] asm("_binary_index_html_end");

extern const unsigned char net_start[] asm("_binary_net_html_start");
extern const unsigned char net_end[] asm("_binary_net_html_end");

extern const unsigned char ota_start[] asm("_binary_ota_html_start");
extern const unsigned char ota_end[] asm("_binary_ota_html_end");

// FAVICON
extern const unsigned char favicon_start[] asm("_binary_favicon_ico_start");
extern const unsigned char favicon_end[] asm("_binary_favicon_ico_end");

// CSS
extern const unsigned char css_start[] asm("_binary_style_css_start");
extern const unsigned char css_end[] asm("_binary_style_css_end");

// JS
extern const unsigned char js_start[] asm("_binary_js_js_start");
extern const unsigned char js_end[] asm("_binary_js_js_end");

// Приватные свойства
static const char *TAG = "web";

// Приватные функцие
static esp_err_t root_get_handler(httpd_req_t *req);
static esp_err_t net_get_handler(httpd_req_t *req);
static esp_err_t favicon_get_handler(httpd_req_t *req);
static esp_err_t css_get_handler(httpd_req_t *req);
static esp_err_t js_get_handler(httpd_req_t *req);
static esp_err_t ota_get_handler(httpd_req_t *req);
//static esp_err_t cap_handler(httpd_req_t *req);
static esp_err_t http_404_error_handler(httpd_req_t *req, httpd_err_code_t err);

static char *parseUri(const char* uri, size_t *resultSize, const char *searchStr);

// Приватные объекты использующие функции описанные выше
static const httpd_uri_t root = {
		.uri = "/",
		.method = HTTP_GET,
		.handler = root_get_handler
};

static const httpd_uri_t net_get = {
		.uri = "/net.html",
		.method = HTTP_GET,
		.handler = net_get_handler
};

static const httpd_uri_t favicon = {
		.uri = "/favicon.ico",
		.method = HTTP_GET,
		.handler = favicon_get_handler
};

static const httpd_uri_t css = {
		.uri = "/style.css",
		.method = HTTP_GET,
		.handler = css_get_handler
};

static const httpd_uri_t js_get = {
		.uri = "/js/js.js",
		.method = HTTP_GET,
		.handler = js_get_handler
};

static const httpd_uri_t ota_get = {
		.uri = "/ota.html",
		.method = HTTP_GET,
		.handler = ota_get_handler
};

static const httpd_uri_t ws = {
		.uri        = "/ws",
		.method     = HTTP_GET,
		.handler    = ws_handler,
		.user_ctx   = NULL,
		.is_websocket = true
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
	//	printf("uri - %s, method - %d, content_len %d\n", req->uri, req->method, req->content_len);

	httpd_resp_set_type(req, "text/html");
	httpd_resp_send(req, (char *)index_start, index_end - index_start);

	//	uint32_t free_internal_memory = heap_caps_get_free_size(MALLOC_CAP_INTERNAL);
	//	printf("Free DRAM (internal memory): %"PRIu32" bytes\n", free_internal_memory);

	return ESP_OK;
}

static esp_err_t net_get_handler(httpd_req_t *req)
{
	//	printf("uri - %s, method - %d, content_len %d\n", req->uri, req->method, req->content_len);

	httpd_resp_set_type(req, "text/html");
	httpd_resp_send(req, (char *)net_start, net_end - net_start);

	return ESP_OK;
}

static esp_err_t favicon_get_handler(httpd_req_t *req)
{
	//	printf("uri - %s\n", req->uri);

	httpd_resp_set_type(req, "image/x-icon");
	httpd_resp_send(req, (char *)favicon_start, favicon_end - favicon_start);

	return ESP_OK;
}

static esp_err_t css_get_handler(httpd_req_t *req)
{
	//	printf("uri - %s\n", req->uri);

	httpd_resp_set_type(req, "text/css");
	httpd_resp_send(req, (char *)css_start, css_end - css_start);

	return ESP_OK;
}

static esp_err_t js_get_handler(httpd_req_t *req)
{
	//	printf("uri - %s\n", req->uri);

	httpd_resp_set_type(req, "text/javascript");
	httpd_resp_send(req, (char *)js_start, js_end - js_start);

	return ESP_OK;
}

static esp_err_t ota_get_handler(httpd_req_t *req)
{
	//	printf("uri - %s\n", req->uri);

	httpd_resp_set_type(req, "text/html");
	httpd_resp_send(req, (char *)ota_start, ota_end - ota_start);

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

//static esp_err_t cap_handler(httpd_req_t *req)
//{
//	return http_404_error_handler(req, 0);
//}

//Реализация публичных функций
httpd_handle_t start_webserver(void)
{
	while(!(glob_get_status_reg() & STATUS_WIFI_AP_START))
		vTaskDelay(100 / portTICK_PERIOD_MS);

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
		httpd_register_uri_handler(server, &net_get);
		httpd_register_uri_handler(server, &ota_get);

		httpd_register_uri_handler(server, &favicon);
		httpd_register_uri_handler(server, &css);
		httpd_register_uri_handler(server, &js_get);

		httpd_register_uri_handler(server, &ws);

		httpd_register_err_handler(server, HTTPD_404_NOT_FOUND, http_404_error_handler);
	}
	else
	{
		ESP_LOGE(TAG, "Error start server on port: '%d'", config.server_port);
	}

	return server;
}
