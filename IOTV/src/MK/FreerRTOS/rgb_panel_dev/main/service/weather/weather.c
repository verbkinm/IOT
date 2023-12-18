/*
 * wearther.c
 *
 *  Created on: 18 дек. 2023 г.
 *      Author: user
 */

#include "weather.h"

#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define MIN(a,b) ((a) < (b) ? (a) : (b))

#define MAX_HTTP_OUTPUT_BUFFER 2048

extern uint32_t glob_status_reg;

static const char *TAG = "weather";

static esp_err_t _http_event_handler(esp_http_client_event_t *evt)
{
	//	switch (evt->event_id) {
	//	case HTTP_EVENT_ERROR:
	//		ESP_LOGD(TAG, "HTTP_EVENT_ERROR");
	//		break;
	//	case HTTP_EVENT_ON_CONNECTED:
	//		ESP_LOGD(TAG, "HTTP_EVENT_ON_CONNECTED");
	//		break;
	//	case HTTP_EVENT_HEADER_SENT:
	//		ESP_LOGD(TAG, "HTTP_EVENT_HEADER_SENT");
	//		break;
	//	case HTTP_EVENT_ON_HEADER:
	//		ESP_LOGD(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
	//		break;
	//	case HTTP_EVENT_ON_DATA:
	//		ESP_LOGD(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
	//		break;
	//	case HTTP_EVENT_ON_FINISH:
	//		ESP_LOGD(TAG, "HTTP_EVENT_ON_FINISH");
	//		break;
	//	case HTTP_EVENT_DISCONNECTED:
	//		ESP_LOGD(TAG, "HTTP_EVENT_DISCONNECTED");
	//		break;
	//	case HTTP_EVENT_REDIRECT:
	//		ESP_LOGD(TAG, "HTTP_EVENT_REDIRECT");
	//		break;
	//	}
	//	return ESP_OK;
	static char *output_buffer;  // Buffer to store response of http request from event handler
	static int output_len;       // Stores number of bytes read
	switch(evt->event_id) {
	case HTTP_EVENT_ERROR:
		ESP_LOGD(TAG, "HTTP_EVENT_ERROR");
		break;
	case HTTP_EVENT_ON_CONNECTED:
		ESP_LOGD(TAG, "HTTP_EVENT_ON_CONNECTED");
		break;
	case HTTP_EVENT_HEADER_SENT:
		ESP_LOGD(TAG, "HTTP_EVENT_HEADER_SENT");
		break;
	case HTTP_EVENT_ON_HEADER:
		ESP_LOGD(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
		break;
	case HTTP_EVENT_ON_DATA:
		ESP_LOGD(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
		/*
		 *  Check for chunked encoding is added as the URL for chunked encoding used in this example returns binary data.
		 *  However, event handler can also be used in case chunked encoding is used.
		 */
		if (!esp_http_client_is_chunked_response(evt->client)) {
			// If user_data buffer is configured, copy the response into the buffer
			int copy_len = 0;
			if (evt->user_data) {
				copy_len = MIN(evt->data_len, (MAX_HTTP_OUTPUT_BUFFER - output_len));
				if (copy_len) {
					memcpy(evt->user_data + output_len, evt->data, copy_len);
				}
			} else {
				const int buffer_len = esp_http_client_get_content_length(evt->client);
				if (output_buffer == NULL) {
					output_buffer = (char *) malloc(buffer_len);
					output_len = 0;
					if (output_buffer == NULL) {
						ESP_LOGE(TAG, "Failed to allocate memory for output buffer");
						return ESP_FAIL;
					}
				}
				copy_len = MIN(evt->data_len, (buffer_len - output_len));
				if (copy_len) {
					memcpy(output_buffer + output_len, evt->data, copy_len);
				}
			}
			output_len += copy_len;
		}

		break;
	case HTTP_EVENT_ON_FINISH:
		ESP_LOGD(TAG, "HTTP_EVENT_ON_FINISH");
		if (output_buffer != NULL) {
			// Response is accumulated in output_buffer. Uncomment the below line to print the accumulated response
			// ESP_LOG_BUFFER_HEX(TAG, output_buffer, output_len);
			free(output_buffer);
			output_buffer = NULL;
		}
		output_len = 0;
		break;
	case HTTP_EVENT_DISCONNECTED:
		ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
		int mbedtls_err = 0;
		esp_err_t err = esp_tls_get_and_clear_last_error((esp_tls_error_handle_t)evt->data, &mbedtls_err, NULL);
		if (err != 0) {
			ESP_LOGI(TAG, "Last esp error code: 0x%x", err);
			ESP_LOGI(TAG, "Last mbedtls failure: 0x%x", mbedtls_err);
		}
		if (output_buffer != NULL) {
			free(output_buffer);
			output_buffer = NULL;
		}
		output_len = 0;
		break;
	case HTTP_EVENT_REDIRECT:
		ESP_LOGD(TAG, "HTTP_EVENT_REDIRECT");
		esp_http_client_set_header(evt->client, "From", "user@example.com");
		esp_http_client_set_header(evt->client, "Accept", "text/html");
		esp_http_client_set_redirection(evt->client);
		break;
	}
	return ESP_OK;
}

void weather_service_task(void *pvParameters)
{
//	char *url = "https://geocoding-api.open-meteo.com/v1/search?name=krasnodar&count=1&language=ru&format=json";
	char *url = "https://api.open-meteo.com/v1/forecast?latitude=45.044842&longitude=38.976025&current=temperature_2m,relative_humidity_2m,apparent_temperature,is_day,precipitation,rain,showers,snowfall,cloud_cover,surface_pressure,wind_speed_10m,wind_direction_10m,wind_gusts_10m&wind_speed_unit=ms&timezone=auto&forecast_days=1";

	for( ;; )
	{
		if ( !(glob_status_reg & STATUS_WIFI_STA_CONNECTED) )
			goto for_end;

		char local_response_buffer[MAX_HTTP_OUTPUT_BUFFER] = {0};

		esp_http_client_config_t config = {
				.url = url,
				.crt_bundle_attach = esp_crt_bundle_attach,
				.event_handler = _http_event_handler,
				.keep_alive_enable = true,
				.user_data = local_response_buffer
		};

		esp_http_client_handle_t client = esp_http_client_init(&config);

		esp_err_t err = esp_http_client_perform(client);
		if (err == ESP_OK)
		{
			ESP_LOGI(TAG, "HTTP GET Status = %d, content_length = %"PRIu64", data:\n %s",
					esp_http_client_get_status_code(client),
					esp_http_client_get_content_length(client),
					local_response_buffer);

			//			ESP_LOG_BUFFER_HEX(TAG, local_response_buffer, strlen(local_response_buffer));
		}
//		else
//		{
//			ESP_LOGE(TAG, "HTTP GET request failed: %s", esp_err_to_name(err));
//		}

//		ESP_LOG_BUFFER_HEX(TAG, local_response_buffer, strlen(local_response_buffer));

//		esp_http_client_cleanup(client);
//		esp_http_client_close(client);

		vTaskDelay(10000 / portTICK_PERIOD_MS);
		esp_http_client_cleanup(client);

		for_end:
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}
