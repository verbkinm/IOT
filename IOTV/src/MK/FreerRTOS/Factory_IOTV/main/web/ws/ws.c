/*
 * ws.c
 *
 *  Created on: 7 авг. 2024 г.
 *      Author: user
 */

#include "ws.h"
#include "esp_log.h"
#include "web/json/json.h"

static const char *TAG = "WS";

//static void ws_async_send(void *arg);
//static esp_err_t trigger_async_send(httpd_handle_t handle, httpd_req_t *req);
//
//static esp_err_t trigger_async_send(httpd_handle_t handle, httpd_req_t *req)
//{
//	struct async_resp_arg *resp_arg = malloc(sizeof(struct async_resp_arg));
//	resp_arg->hd = req->handle;
//	resp_arg->fd = httpd_req_to_sockfd(req);
//	resp_arg->dataSize = req->content_len;
//	resp_arg->data
//	return httpd_queue_work(handle, ws_async_send, resp_arg);
//}
//
//static void ws_async_send(void *arg)
//{
//	static const char * data = "Async data";
//	struct async_resp_arg *resp_arg = arg;
//	httpd_handle_t hd = resp_arg->hd;
//	int fd = resp_arg->fd;
//	httpd_ws_frame_t ws_pkt;
//	memset(&ws_pkt, 0, sizeof(httpd_ws_frame_t));
//	ws_pkt.payload = (uint8_t*)data;
//	ws_pkt.len = strlen(data);
//	ws_pkt.type = HTTPD_WS_TYPE_TEXT;
//
//	httpd_ws_send_frame_async(hd, fd, &ws_pkt);
//	free(resp_arg);
//}

esp_err_t ws_handler(httpd_req_t *req)
{
	//	printf("ws_handler uri - %s, method - %d, content_len %d\n", req->uri, req->method, req->content_len);

	if (req->method == HTTP_GET) {
		ESP_LOGI(TAG, "Handshake done, the new connection was opened");
		return ESP_OK;
	}

	httpd_ws_frame_t ws_pkt;
	uint8_t *buf = NULL;

	memset(&ws_pkt, 0, sizeof(httpd_ws_frame_t));

	ws_pkt.type = HTTPD_WS_TYPE_TEXT;
	/* Set max_len = 0 to get the frame len */
	esp_err_t ret = httpd_ws_recv_frame(req, &ws_pkt, 0);
	if (ret != ESP_OK) {
		ESP_LOGE(TAG, "httpd_ws_recv_frame failed to get frame len with %d", ret);
		return ret;
	}
	ESP_LOGI(TAG, "frame len is %d", ws_pkt.len);
	if (ws_pkt.len)
	{
		/* ws_pkt.len + 1 is for NULL termination as we are expecting a string */
		buf = calloc(1, ws_pkt.len + 1);
		//		free(ws_pkt.payload);
		if (buf == NULL) {
			ESP_LOGE(TAG, "Failed to calloc memory for buf");
			return ESP_ERR_NO_MEM;
		}
		ws_pkt.payload = buf;
		/* Set max_len = ws_pkt.len to get the frame payload */
		ret = httpd_ws_recv_frame(req, &ws_pkt, ws_pkt.len);
		if (ret != ESP_OK) {
			ESP_LOGE(TAG, "httpd_ws_recv_frame failed with %d", ret);
			free(buf);
			return ret;
		}
//		ESP_LOGI(TAG, "Got packet with message: %s", ws_pkt.payload);
	}
//	ESP_LOGI(TAG, "Packet type: %d, data: %s", ws_pkt.type, ws_pkt.payload);
	char *reply = parse_request((const char *)(ws_pkt.payload));
	if (reply != NULL)
	{
		ws_pkt.payload = (uint8_t *)reply;
		ws_pkt.len = strlen(reply);

		ret = httpd_ws_send_frame(req, &ws_pkt);
		free(reply);
		if (ret != ESP_OK) {
			ESP_LOGE(TAG, "httpd_ws_send_frame failed with %d", ret);
		}
	}
//	else
//	{
//		ret = httpd_ws_send_frame(req, &ws_pkt);
//		if (ret != ESP_OK) {
//			ESP_LOGE(TAG, "httpd_ws_send_frame failed with %d", ret);
//		}
//	}
	free(buf);
	return ret;
}
