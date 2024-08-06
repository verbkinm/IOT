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

// HTML страницы
extern const unsigned char index_start[] asm("_binary_index_html_start");
extern const unsigned char index_end[] asm("_binary_index_html_end");

extern const unsigned char net_1_start[] asm("_binary_net_1_start");
extern const unsigned char net_1_end[] asm("_binary_net_1_end");
extern const unsigned char net_2_start[] asm("_binary_net_2_start");
extern const unsigned char net_2_end[] asm("_binary_net_2_end");
extern const unsigned char net_3_start[] asm("_binary_net_3_start");
extern const unsigned char net_3_end[] asm("_binary_net_3_end");
extern const unsigned char net_4_start[] asm("_binary_net_4_start");
extern const unsigned char net_4_end[] asm("_binary_net_4_end");

extern const unsigned char scanning_start[] asm("_binary_scanning_html_start");
extern const unsigned char scanning_end[] asm("_binary_scanning_html_end");

extern const unsigned char msg_1_start[] asm("_binary_msg_1_start");
extern const unsigned char msg_1_end[] asm("_binary_msg_1_end");
extern const unsigned char msg_2_start[] asm("_binary_msg_2_start");
extern const unsigned char msg_2_end[] asm("_binary_msg_2_end");
extern const unsigned char msg_3_start[] asm("_binary_msg_3_start");
extern const unsigned char msg_3_end[] asm("_binary_msg_3_end");

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

// CSS
extern const unsigned char css_start[] asm("_binary_style_css_start");
extern const unsigned char css_end[] asm("_binary_style_css_end");

// Приватные свойства
static const char *TAG = "web";
static char *otaDecodeUrl = NULL;

// Приватные функцие
static esp_err_t root_get_handler(httpd_req_t *req);
static esp_err_t net_get_handler(httpd_req_t *req);
static esp_err_t net_post_handler(httpd_req_t *req);
static esp_err_t scanning_get_handler(httpd_req_t *req);
static esp_err_t msg_post_handler(httpd_req_t *req);
static esp_err_t favicon_get_handler(httpd_req_t *req);
static esp_err_t css_get_handler(httpd_req_t *req);
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

static const httpd_uri_t net_get = {
		.uri = "/net.html",
		.method = HTTP_GET,
		.handler = net_get_handler
};

static const httpd_uri_t net_post = {
		.uri = "/net.html",
		.method = HTTP_POST,
		.handler = net_post_handler
};

static const httpd_uri_t scanning_get = {
		.uri = "/scanning.html",
		.method = HTTP_GET,
		.handler = scanning_get_handler
};

//static const httpd_uri_t msg_post = {
//		.uri = "/msg",
//		.method = HTTP_POST,
//		.handler = msg_post_handler
//};

//static const httpd_uri_t cmd_get = {
//		.uri = "/cmd",
//		.method = HTTP_GET,
//		.handler = cap_handler
//};

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

//static const httpd_uri_t ota_get = {
//		.uri = "/ota",
//		.method = HTTP_GET,
//		.handler = ota_get_handler
//};
//
//static const httpd_uri_t cmd_post = {
//		.uri = "/cmd",
//		.method = HTTP_POST,
//		.handler = cmd_post_handler
//};
//
//static const httpd_uri_t ota_post = {
//		.uri = "/ota",
//		.method = HTTP_POST,
//		.handler = ota_post_handler
//};

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
	printf("uri - %s, method - %d, content_len %d\n", req->uri, req->method, req->content_len);

	char *index = (char *)index_start;

	char *xmp_open = strstr(index, "<xmp>");
	char *xmp_close = strstr(index, "</xmp>");

	size_t part_1 = xmp_open - index + 5;
	size_t part_2 = (char *)index_end - xmp_close;

	char *sys_inf = get_system_info();
	size_t sys_inf_size = strlen(sys_inf);

	char *sendData = calloc(1, part_1 + sys_inf_size + part_2 + 1);

	memcpy(sendData, index, part_1);
	memcpy(&sendData[part_1], sys_inf, sys_inf_size);
	memcpy(&sendData[part_1 + sys_inf_size], xmp_close, part_2);

	size_t sendSize = index_end - index_start + sys_inf_size - 3;

	httpd_resp_set_type(req, "text/html");
	httpd_resp_send(req, sendData, sendSize);

	free(sys_inf);
	free(sendData);

	//	uint32_t free_internal_memory = heap_caps_get_free_size(MALLOC_CAP_INTERNAL);
	//	printf("Free DRAM (internal memory): %"PRIu32" bytes\n", free_internal_memory);

	return ESP_OK;
}

static esp_err_t scanning_get_handler(httpd_req_t *req)
{
	printf("uri - %s, method - %d, content_len %d\n", req->uri, req->method, req->content_len);

	if (glob_get_status_reg() & STATUS_WIFI_SCAN_DONE)
	{
		glob_clear_bits_status_reg(STATUS_WIFI_SCAN_DONE);
		req->method = HTTP_GET;
		net_get_handler(req);
	}
	else
	{
		httpd_resp_set_type(req, "text/html");
		httpd_resp_send(req, (char *)scanning_start, scanning_end - scanning_start);
		wifi_service_scan_wifi();
	}
	uint32_t free_internal_memory = heap_caps_get_free_size(MALLOC_CAP_INTERNAL);
	printf("Free DRAM (internal memory): %"PRIu32" bytes\n", free_internal_memory);

	return ESP_OK;
}
static esp_err_t net_get_handler(httpd_req_t *req)
{
	printf("uri - %s, method - %d, content_len %d\n", req->uri, req->method, req->content_len);

	if (glob_get_status_reg() & STATUS_WIFI_SCANNING)
	{
		//		printf("DEBUG %lu\n", (glob_get_status_reg() & STATUS_WIFI_SCANNING));
		scanning_get_handler(req);
		return ESP_OK;
	}

	char *buf = NULL;
	char *pwd = NULL;
	char *ssid = NULL;

	nvs_read_wifi_sta_ssid(&buf);

	strcat_dynamic(&ssid, " value=\"");
	strcat_dynamic(&ssid, buf);
	strcat_dynamic(&ssid, "\"");
	free(buf);

	nvs_read_wifi_sta_pwd(&buf);

	strcat_dynamic(&pwd, " value=\"");
	strcat_dynamic(&pwd, buf);
	strcat_dynamic(&pwd, "\"");
	free(buf);

	size_t ssid_size = strlen(ssid);
	size_t pwd_size = strlen(pwd);
	size_t ap_list_size = 0;
	size_t page_size = net_1_end - net_1_start + net_2_end - net_2_start + net_3_end - net_3_start + net_4_end - net_4_start;

	wifi_config_t wifi_config;
	esp_wifi_get_config(WIFI_IF_STA, &wifi_config);

	const wifi_ap_record_t *ap_info = wifi_service_last_scan();
	char *ap_list_html = NULL;

	for (int i = 0; i < AP_INFO_ARR_SIZE; ++i)
	{
		if(ap_info[i].bssid[0] == 0)
			continue;

		char buf[sizeof(ap_info[i].ssid) + 1] = {0};
		memcpy(buf, ap_info[i].ssid, sizeof(ap_info[i].ssid));

		strcat_dynamic(&ap_list_html, "<option value=\"");
		strcat_dynamic(&ap_list_html, buf);

		if (memcmp(ap_info[i].bssid, wifi_config.sta.bssid, 6) == 0)
			strcat_dynamic(&ap_list_html, "\" selected>");
		else
			strcat_dynamic(&ap_list_html, "\">");

		strcat_dynamic(&ap_list_html, "\"");
		strcat_dynamic(&ap_list_html, buf);
		strcat_dynamic(&ap_list_html, "\" (");

		memset(buf, 0, sizeof(buf));
		sprintf(buf, MACSTR, MAC2STR(ap_info[i].bssid));

		strcat_dynamic(&ap_list_html, buf);
		strcat_dynamic(&ap_list_html, ")");

		strcat_dynamic(&ap_list_html, "</option>");
	}

	if (ap_list_html != NULL)
		ap_list_size = strlen(ap_list_html);

	char *sendData = calloc(1, page_size + ssid_size + pwd_size + ap_list_size + 1);
	if (sendData == NULL)
	{
		printf("sendData = NULL\n");
		return ESP_ERR_NO_MEM;
	}

	char *ptr = sendData;
	memcpy(ptr, net_1_start, net_1_end - net_1_start);
	ptr += net_1_end - net_1_start;

	memcpy(ptr, ssid, ssid_size);
	ptr += ssid_size;

	memcpy(ptr, net_2_start, net_2_end - net_2_start);
	ptr += net_2_end - net_2_start;

	memcpy(ptr, pwd, pwd_size);
	ptr += pwd_size;

	memcpy(ptr, net_3_start, net_3_end - net_3_start);
	ptr += net_3_end - net_3_start;

	if (ap_list_html != NULL)
	{
		memcpy(ptr, ap_list_html, strlen(ap_list_html));
		ptr += strlen(ap_list_html);
	}

	memcpy(ptr, net_4_start, net_4_end - net_4_start);

	httpd_resp_set_type(req, "text/html");
	httpd_resp_send(req, sendData, page_size + ssid_size + pwd_size + ap_list_size + 1);

	free(sendData);
	free(pwd);
	free(ssid);
	delete_ptr((void *)&ap_list_html);

	uint32_t free_internal_memory = heap_caps_get_free_size(MALLOC_CAP_INTERNAL);
	printf("Free DRAM (internal memory): %"PRIu32" bytes\n", free_internal_memory);

	return ESP_OK;
}

static esp_err_t net_post_handler(httpd_req_t *req)
{
	printf("uri - %s, method - %d, content_len %d\n", req->uri, req->method, req->content_len);

	char content[100] = {};
	size_t recv_size = MIN(req->content_len, sizeof(content));

	if (httpd_req_recv(req, content, recv_size) <= 0)
		return cap_handler(req);

	//	printf("raw content = %s\n", content);

	char *decodeText = url_decode(content);
	memset(content, 0, sizeof(content));
	memcpy(content, decodeText, strlen(decodeText));
	free(decodeText);

	//	printf("content: %s\n", content);

	bool errArgs = false;
	size_t resultSize;
	char *result = parseUri(content, &resultSize, "ssid=");

	if (result == NULL || resultSize == 0 || resultSize > WIFI_SSID_MAX_LENGH)
	{
		errArgs = true;
		goto end;
	}

	char ssid[WIFI_SSID_MAX_LENGH] = {};
	memcpy(ssid, result, resultSize);

	result = parseUri(content, &resultSize, "pass=");

	if (result == NULL || resultSize == 0 || resultSize > WIFI_SSID_MAX_LENGH)
	{
		errArgs = true;
		goto end;
	}

	char pass[WIFI_PASS_MAX_LENGH + 1] = {};
	memcpy(pass, result, resultSize);

	if (strlen(ssid) == 0 || strlen(pass) == 0)
		errArgs = true;

	if (!errArgs)
	{
		nvs_write_wifi_sta_ssid(ssid);
		nvs_write_wifi_sta_pwd(pass);

		wifi_config_t wifi_config;
		esp_wifi_get_config(WIFI_IF_STA, &wifi_config);
		memcpy(wifi_config.sta.ssid, ssid, sizeof(wifi_config.sta.ssid));
		memcpy(wifi_config.sta.password, pass, MIN(sizeof(wifi_config.sta.password), sizeof(pass) - 1));
		esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config);

		glob_set_bits_status_reg(STATUS_WIFI_AUTOCONNECT);
	}

	end:
	req->method = HTTP_GET;
	net_get_handler(req);

	uint32_t free_internal_memory = heap_caps_get_free_size(MALLOC_CAP_INTERNAL);
	printf("Free DRAM (internal memory): %"PRIu32" bytes\n", free_internal_memory);

	return ESP_OK;
}

static esp_err_t msg_post_handler(httpd_req_t *req)
{
	printf("uri - %s, method - post, content_len %d\n", req->uri, req->content_len);

	char content[100] = {};
	size_t recv_size = MIN(req->content_len, sizeof(content));

	if (httpd_req_recv(req, content, recv_size) <= 0)
		return cap_handler(req);

	//	printf("raw content = %s\n", content);

	char *decodeText = url_decode(content);
	memset(content, 0, sizeof(content));
	memcpy(content, decodeText, strlen(decodeText));
	free(decodeText);

	size_t url_size;
	size_t msg_size;

	char *result = parseUri(content, &url_size, "url=");
	char *url = calloc(1, url_size + 1);
	if (url == NULL)
	{
		printf("url = NULL\n");
		return ESP_ERR_NO_MEM;
	}
	memcpy(url, result, url_size);

	result = parseUri(content, &msg_size, "msg=");
	char *msg = calloc(1, msg_size + 1);
	if (msg == NULL)
	{
		free(url);
		printf("msg = NULL\n");
		return ESP_ERR_NO_MEM;
	}
	memcpy(msg, result, msg_size);

	size_t page_size = msg_1_end - msg_1_start +  msg_2_end - msg_2_start + msg_3_end - msg_3_start;

	char *sendData = calloc(1, page_size + url_size + msg_size + 1);
	if (sendData == NULL)
	{
		printf("sendData = NULL\n");
		return ESP_ERR_NO_MEM;
	}

	char *ptr = sendData;
	memcpy(ptr, msg_1_start, msg_1_end - msg_1_start);
	ptr += msg_1_end - msg_1_start;

	memcpy(ptr, url, url_size);
	ptr += url_size;

	memcpy(ptr, msg_2_start, msg_2_end - msg_2_start);
	ptr += msg_2_end - msg_2_start;

	memcpy(ptr, msg, msg_size);
	ptr += msg_size;

	memcpy(ptr, msg_3_start, msg_3_end - msg_3_start);


	req->method = HTTP_GET;
	httpd_resp_set_type(req, "text/html");
	httpd_resp_send(req, sendData, page_size + url_size + msg_size + 1);

	free(url);
	free(msg);

	uint32_t free_internal_memory = heap_caps_get_free_size(MALLOC_CAP_INTERNAL);
	printf("Free DRAM (internal memory): %"PRIu32" bytes\n", free_internal_memory);

	return ESP_OK;
}

static esp_err_t cmd_post_handler(httpd_req_t *req)
{
	//	printf("uri - %s\n", req->uri);
	//
	//	char content[100] = {};
	//	size_t recv_size = MIN(req->content_len, sizeof(content));
	//
	//	if (httpd_req_recv(req, content, recv_size) <= 0)
	//		return cap_handler(req);
	//
	//	char *decodeText = urlDecode(content);
	//	memcpy(content, decodeText, strlen(decodeText));
	//	free(decodeText);
	//
	//	ESP_LOGE(TAG, "content: %s", content);
	//
	//	bool errArgs = false;
	//	size_t resultSize;
	//	size_t indexSize;
	//	char *result = parseUri(content, &resultSize, "ssid=");
	//
	//	if (result == NULL || resultSize > WIFI_SSID_MAX_LENGH)
	//	{
	//		errArgs = true;
	//		resultSize = WIFI_SSID_MAX_LENGH;
	//		goto ARGS_ERROR;
	//	}
	//
	//	char ssid[WIFI_SSID_MAX_LENGH] = {};
	//	memcpy(ssid, result, resultSize);
	//
	//	result = parseUri(result, &resultSize, "pass=");
	//	if (result == NULL || resultSize > WIFI_PASS_MAX_LENGH)
	//	{
	//		errArgs = true;
	//		resultSize = WIFI_PASS_MAX_LENGH;
	//		goto ARGS_ERROR;
	//	}
	//
	//	char pass[WIFI_PASS_MAX_LENGH] = {};
	//	memcpy(pass, result, resultSize);
	//
	//	if (strlen(ssid) == 0 || strlen(pass) == 0)
	//		errArgs = true;
	//
	//	if (!errArgs)
	//	{
	//		printf("ssid = %s\n", ssid);
	//		printf("pass = %s\n", pass);
	//
	//		if (global_wifi_sta_status == IOTV_WIFI_STA_NULL)
	//			wifi_sta_connect(ssid, pass);
	//
	//		char *index = (char *)connecting_start;
	//		indexSize = connecting_end - connecting_start;
	//
	//		httpd_resp_set_type(req, "text/html");
	//		httpd_resp_send(req, index, indexSize);
	//
	//		return ESP_OK;
	//	}
	//
	//	ARGS_ERROR:
	//	printf("cmd handler args error\n");
	//
	//	char *index = (char *)error_connect_start;
	//	indexSize = error_connect_end - error_connect_start;
	//
	//	httpd_resp_set_type(req, "text/html");
	//	httpd_resp_send(req, index, indexSize);

	return ESP_OK;
}

static esp_err_t ota_post_handler(httpd_req_t *req)
{
	//	printf("uri - %s\n", req->uri);
	//
	//	if (global_wifi_sta_status != IOTV_WIFI_STA_CONNECTED && global_ota_status != IOTV_OTA_NULL)
	//		return ESP_OK;
	//
	//	char content[100] = {};
	//	size_t recv_size = MIN(req->content_len, sizeof(content));
	//
	//	if (httpd_req_recv(req, content, recv_size) <= 0)
	//		return cap_handler(req);
	//
	//	ESP_LOGE(TAG, "content: %s", content);
	//
	//	size_t resultSize;
	//	char *result = parseUri(content, &resultSize, "firmware=");
	//
	//	if (result == NULL)
	//		return cap_handler(req);
	//
	//	otaDecodeUrl = urlDecode(result);
	//
	//	global_ota_status = IOTV_OTA_DOWNLOADING;
	//	cap_handler(req);


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

static esp_err_t css_get_handler(httpd_req_t *req)
{
	printf("uri - %s\n", req->uri);

	char *index = (char *)css_start;
	size_t indexSize = css_end - css_start;

	httpd_resp_set_type(req, "text/css");
	httpd_resp_send(req, index, indexSize);

	return ESP_OK;
}

static esp_err_t ota_get_handler(httpd_req_t *req)
{
	//	printf("uri - %s\n", req->uri);
	//
	//	char *index = (char *)index_start;
	//	size_t indexSize = index_end - index_start;
	//
	//	if (global_ota_status == IOTV_OTA_NULL && global_wifi_sta_status == IOTV_WIFI_STA_CONNECTED)
	//	{
	//		index = (char *)ota_start;
	//		indexSize = ota_end - ota_start;
	//	}
	//	else if (global_ota_status == IOTV_OTA_DOWNLOADING)
	//	{
	//		index = (char *)ota_downloading_start;
	//		indexSize = ota_downloading_end - ota_downloading_start;
	//
	//		httpd_resp_set_type(req, "text/html");
	//		httpd_resp_send(req, index, indexSize);
	//
	//		if (otaDecodeUrl != NULL)
	//		{
	//			ota_firmware(otaDecodeUrl);
	//			free(otaDecodeUrl);
	//		}
	//
	//		return ESP_OK;
	//	}
	//	else if (global_ota_status == IOTV_OTA_DOWNLOADED)
	//	{
	//		index = (char *)ota_downloaded_start;
	//		indexSize = ota_downloaded_end - ota_downloaded_start;
	//		global_ota_status = IOTV_OTA_REBOOTING;
	//	}
	//	else if (global_ota_status == IOTV_OTA_REBOOTING)
	//		esp_restart();
	//	else if (global_ota_status == IOTV_OTA_FAIL)
	//	{
	//		index = (char *)ota_fail_start;
	//		indexSize = ota_fail_end - ota_fail_start;
	//
	//		global_ota_status = IOTV_OTA_NULL;
	//	}
	//	else
	//		return cap_handler(req);
	//
	//	httpd_resp_set_type(req, "text/html");
	//	httpd_resp_send(req, index, indexSize);

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
		httpd_register_uri_handler(server, &net_post);
		httpd_register_uri_handler(server, &scanning_get);
		//		httpd_register_uri_handler(server, &msg_post);
		//		httpd_register_uri_handler(server, &cmd_get);
		httpd_register_uri_handler(server, &favicon);
		httpd_register_uri_handler(server, &css);
		//		httpd_register_uri_handler(server, &ota_get);
		//
		//		httpd_register_uri_handler(server, &cmd_post);
		//		httpd_register_uri_handler(server, &ota_post);

		httpd_register_err_handler(server, HTTPD_404_NOT_FOUND, http_404_error_handler);
	}
	else
	{
		ESP_LOGE(TAG, "Error start server on port: '%d'", config.server_port);
	}

	return server;
}
