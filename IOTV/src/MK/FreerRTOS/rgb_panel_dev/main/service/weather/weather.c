/*
 * wearther.c
 *
 *  Created on: 18 дек. 2023 г.
 *      Author: user
 */

#include "weather.h"

#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define MIN(a,b) ((a) < (b) ? (a) : (b))

#define MAX_HTTP_REC_BUFFER 4096

extern uint32_t glob_status_reg;
extern uint32_t glob_status_err;

extern open_meteo_data_t glob_open_meteo;
static char *city_search = NULL;

static const char *TAG = "weather";

static const char *city_url = "https://geocoding-api.open-meteo.com/v1/search?count=20&language=ru&format=json&name=";
static const char *meteo_url = "https://api.open-meteo.com/v1/forecast?current=temperature_2m,relative_humidity_2m,apparent_temperature,is_day,precipitation,rain,showers,snowfall,cloud_cover,surface_pressure,wind_speed_10m,wind_direction_10m,wind_gusts_10m&wind_speed_unit=ms&timeformat=unixtime&timezone=auto&forecast_days=1&latitude=";

static void check_meteo_conf_file(void);
static void read_meteo_conf(void);
static char *generate_url_meteo(void);
static bool parse_open_meteo_weather(const char *data, open_meteo_data_t *open_meteo);
static void http_city_search(void);
static esp_err_t _http_event_handler(esp_http_client_event_t *evt);

static esp_err_t _http_event_handler(esp_http_client_event_t *evt)
{
	switch (evt->event_id) {
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
		break;
	case HTTP_EVENT_ON_FINISH:
		ESP_LOGD(TAG, "HTTP_EVENT_ON_FINISH");
		break;
	case HTTP_EVENT_DISCONNECTED:
		ESP_LOGD(TAG, "HTTP_EVENT_DISCONNECTED");
		break;
	case HTTP_EVENT_REDIRECT:
		ESP_LOGD(TAG, "HTTP_EVENT_REDIRECT");
		break;
	}
	return ESP_OK;
	//	static char *output_buffer;  // Buffer to store response of http request from event handler
	//	static int output_len;       // Stores number of bytes read
	//	switch(evt->event_id) {
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
	//		/*
	//		 *  Check for chunked encoding is added as the URL for chunked encoding used in this example returns binary data.
	//		 *  However, event handler can also be used in case chunked encoding is used.
	//		 */
	//		if (!esp_http_client_is_chunked_response(evt->client)) {
	//			// If user_data buffer is configured, copy the response into the buffer
	//			int copy_len = 0;
	//			if (evt->user_data) {
	//				copy_len = MIN(evt->data_len, (MAX_HTTP_OUTPUT_BUFFER - output_len));
	//				if (copy_len) {
	//					memcpy(evt->user_data + output_len, evt->data, copy_len);
	//				}
	//			} else {
	//				const int buffer_len = esp_http_client_get_content_length(evt->client);
	//				if (output_buffer == NULL) {
	//					output_buffer = (char *) malloc(buffer_len);
	//					output_len = 0;
	//					if (output_buffer == NULL) {
	//						ESP_LOGE(TAG, "Failed to allocate memory for output buffer");
	//						return ESP_FAIL;
	//					}
	//				}
	//				copy_len = MIN(evt->data_len, (buffer_len - output_len));
	//				if (copy_len) {
	//					memcpy(output_buffer + output_len, evt->data, copy_len);
	//				}
	//			}
	//			output_len += copy_len;
	//		}
	//
	//		break;
	//	case HTTP_EVENT_ON_FINISH:
	//		ESP_LOGD(TAG, "HTTP_EVENT_ON_FINISH");
	//		if (output_buffer != NULL) {
	//			// Response is accumulated in output_buffer. Uncomment the below line to print the accumulated response
	//			// ESP_LOG_BUFFER_HEX(TAG, output_buffer, output_len);
	//			free(output_buffer);
	//			output_buffer = NULL;
	//		}
	//		output_len = 0;
	//		break;
	//	case HTTP_EVENT_DISCONNECTED:
	//		ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
	//		int mbedtls_err = 0;
	//		esp_err_t err = esp_tls_get_and_clear_last_error((esp_tls_error_handle_t)evt->data, &mbedtls_err, NULL);
	//		if (err != 0) {
	//			ESP_LOGI(TAG, "Last esp error code: 0x%x", err);
	//			ESP_LOGI(TAG, "Last mbedtls failure: 0x%x", mbedtls_err);
	//		}
	//		if (output_buffer != NULL) {
	//			free(output_buffer);
	//			output_buffer = NULL;
	//		}
	//		output_len = 0;
	//		break;
	//	case HTTP_EVENT_REDIRECT:
	//		ESP_LOGD(TAG, "HTTP_EVENT_REDIRECT");
	//		esp_http_client_set_header(evt->client, "From", "user@example.com");
	//		esp_http_client_set_header(evt->client, "Accept", "text/html");
	//		esp_http_client_set_redirection(evt->client);
	//		break;
	//	}
	//	return ESP_OK;
}

static void check_meteo_conf_file(void)
{
	cJSON *root = cJSON_CreateObject();

	cJSON *sntp = cJSON_CreateObject();
	cJSON_AddItemToObjectCS(root, "meteo", sntp);

	cJSON *on_obj = cJSON_CreateString("0");
	cJSON_AddItemToObject(sntp, "on", on_obj);

	cJSON *city_obj = cJSON_CreateString("Moskva");
	cJSON_AddItemToObjectCS(sntp, "city", city_obj);

	cJSON *latitude_obj = cJSON_CreateString("55.75222");
	cJSON_AddItemToObjectCS(sntp, "latitude", latitude_obj);

	cJSON *longitude_obj = cJSON_CreateString("37.61556");
	cJSON_AddItemToObjectCS(sntp, "longitude", longitude_obj);

	get_meteo_config_value("on", &on_obj->valuestring);
	get_meteo_config_value("city", &city_obj->valuestring);
	get_meteo_config_value("latitude", &latitude_obj->valuestring);
	get_meteo_config_value("longitude", &longitude_obj->valuestring);

	FILE *file = fopen(METEO_CONF_PATH, "w");
	if (file == NULL)
		printf("cant write \"%s\" file!\n", METEO_CONF_PATH);
	else
	{
		fprintf(file, "%s", cJSON_Print(root));
		fclose(file);
	}

	cJSON_Delete(root);
}

static void http_city_search(void)
{
	char *response_buffer = calloc(1, MAX_HTTP_REC_BUFFER);

	int len = strlen(city_url) + strlen(city_search);
	char *url = calloc(1, len + 1);
	strcat(url, city_url);
	strcat(url, city_search);

	esp_http_client_config_t config = {
			.url = url,
			.crt_bundle_attach = esp_crt_bundle_attach,
	};

	esp_http_client_handle_t client = esp_http_client_init(&config);
	esp_http_client_open(client, 0);
	esp_http_client_fetch_headers(client);

	FILE *file = fopen(METEO_CITY_PATH, "w");
	if (file == NULL)
	{
		printf("cant write \"%s\" file!\n", METEO_CITY_PATH);
		goto end;
	}

	while (true)
	{
		int ret = esp_http_client_read(client, response_buffer, MAX_HTTP_REC_BUFFER);
		if (ret < 1)
			break;

		fwrite(response_buffer, ret, 1, file);
	}
	fclose(file);

	end:
	free(url);
	esp_http_client_cleanup(client);
	free(response_buffer);

	glob_status_reg |= STATUS_METEO_CITY_SEARCH_DONE;
}

static char *generate_url_meteo(void)
{
	char *url = NULL;
	const char *second_part = "&longitude=";

	char *latitude = NULL;
	get_meteo_config_value("latitude", &latitude);

	char *longitude = NULL;
	get_meteo_config_value("longitude", &longitude);

	int len = strlen(meteo_url) + strlen(latitude) + strlen(second_part) + strlen(longitude);

	url = calloc(1, len + 1);
	strcat(url, meteo_url);
	strcat(url, latitude);
	strcat(url, second_part);
	strcat(url, longitude);

	free(latitude);
	free(longitude);

	return url;
}

static void read_meteo_conf(void)
{
	char *on = NULL;
	if (get_meteo_config_value("on", &on))
	{
		if (strcmp(on, "1") == 0)
			glob_status_reg |= STATUS_METEO_ON;
		free(on);
	}
}

static bool parse_open_meteo_weather(const char *data, open_meteo_data_t *open_meteo)
{
	if (data == NULL || open_meteo == NULL)
		return false;

//	printf("Data: \n%s\n", data);
	cJSON *monitor = cJSON_Parse(data);
	if (monitor == NULL)
		return false;

	cJSON *obj = cJSON_GetObjectItemCaseSensitive(monitor, "latitude");
	if (obj != NULL)
		open_meteo->latitude = cJSON_GetNumberValue(obj);

	obj = cJSON_GetObjectItemCaseSensitive(monitor, "longitude");
	if (obj != NULL)
		open_meteo->longitude = cJSON_GetNumberValue(obj);

	cJSON *current = cJSON_GetObjectItemCaseSensitive(monitor, "current");
	if (current == NULL)
		goto bad_end;

	obj = cJSON_GetObjectItemCaseSensitive(current, "temperature_2m");
	if (obj != NULL)
		open_meteo->temperature = cJSON_GetNumberValue(obj);

	obj = cJSON_GetObjectItemCaseSensitive(current, "apparent_temperature");
	if (obj != NULL)
		open_meteo->apparent_temperature = cJSON_GetNumberValue(obj);

	obj = cJSON_GetObjectItemCaseSensitive(current, "relative_humidity_2m");
	if (obj != NULL)
		open_meteo->relative_humidity = cJSON_GetNumberValue(obj);

	obj = cJSON_GetObjectItemCaseSensitive(current, "surface_pressure");
	if (obj != NULL)
	{
		open_meteo->surface_pressure = cJSON_GetNumberValue(obj) * 0.7501;
	}

	obj = cJSON_GetObjectItemCaseSensitive(current, "cloud_cover");
	if (obj != NULL)
		open_meteo->cloud_cover = cJSON_GetNumberValue(obj);

	obj = cJSON_GetObjectItemCaseSensitive(current, "wind_speed_10m");
	if (obj != NULL)
		open_meteo->wind_speed = cJSON_GetNumberValue(obj);

	obj = cJSON_GetObjectItemCaseSensitive(current, "wind_direction_10m");
	if (obj != NULL)
		open_meteo->wind_direction = cJSON_GetNumberValue(obj);

	obj = cJSON_GetObjectItemCaseSensitive(current, "wind_gusts_10m");
	if (obj != NULL)
		open_meteo->wind_gusts = cJSON_GetNumberValue(obj);

	obj = cJSON_GetObjectItemCaseSensitive(current, "precipitation");
	if (obj != NULL)
		open_meteo->precipitation = cJSON_GetNumberValue(obj);

	obj = cJSON_GetObjectItemCaseSensitive(current, "rain");
	if (obj != NULL)
		open_meteo->rain = cJSON_GetNumberValue(obj);

	obj = cJSON_GetObjectItemCaseSensitive(current, "showers");
	if (obj != NULL)
		open_meteo->showers = cJSON_GetNumberValue(obj);

	obj = cJSON_GetObjectItemCaseSensitive(current, "snowfall");
	if (obj != NULL)
		open_meteo->snowfall = cJSON_GetNumberValue(obj);

	obj = cJSON_GetObjectItemCaseSensitive(current, "time");
	if (obj != NULL)
		open_meteo->time = cJSON_GetNumberValue(obj);

	cJSON_Delete(monitor);
	return true;

	bad_end:
	cJSON_Delete(monitor);
	return false;
}

void service_weather_set_city(const char* city)
{
	if (city_search != NULL)
		free(city_search);

	city_search = url_encode(city);
}

void http_meteo_get(void)
{
	char *local_response_buffer = calloc(1, MAX_HTTP_REC_BUFFER);

	char *url = generate_url_meteo();
	esp_http_client_config_t config = {
			.url = url,
			.crt_bundle_attach = esp_crt_bundle_attach,
	};
	esp_http_client_handle_t client = esp_http_client_init(&config);

	esp_http_client_open(client, 0);
	esp_http_client_fetch_headers(client);
	int ret = esp_http_client_read(client, local_response_buffer, MAX_HTTP_REC_BUFFER);

	if (ret == MAX_HTTP_REC_BUFFER)
		ESP_LOGE(TAG, "http request >= MAX_HTTP_REC_BUFFER");

//	printf("ret = %d\n", ret);

	open_meteo_data_t *open_meteo = calloc(1, sizeof(open_meteo_data_t));
	if (parse_open_meteo_weather(local_response_buffer, open_meteo))
		glob_open_meteo = *open_meteo;

	if (glob_open_meteo.city_name != NULL)
		free(glob_open_meteo.city_name);
	get_meteo_config_value("city", &glob_open_meteo.city_name);

//	printf("Meteo information:\n"
//			"City: %s\n"
//			"latitude: %f\n"
//			"longitude: %f\n"
//			"temperature: %f\n"
//			"apparent_temperature: %f\n"
//			"surface_pressure: %f\n"
//			"relative_humidity: %d\n"
//			"wind_speed: %f\n"
//			"wind_gusts: %f\n"
//			"wind_direction: %d\n"
//			"cloud_cover: %d\n"
//			"precipitation: %d\n"
//			"rain: %d\n"
//			"showers: %d\n"
//			"snowfall: %d\n",
//			open_meteo->city_name, open_meteo->latitude, open_meteo->longitude, open_meteo->temperature, open_meteo->apparent_temperature,
//			open_meteo->surface_pressure, open_meteo->relative_humidity, open_meteo->wind_speed, open_meteo->wind_gusts, open_meteo->wind_direction,
//			open_meteo->cloud_cover, open_meteo->precipitation, open_meteo->rain, open_meteo->showers, open_meteo->snowfall);
	free(url);
	free(open_meteo);
	free(local_response_buffer);
	esp_http_client_cleanup(client);
}

void weather_service_task(void *pvParameters)
{
	check_meteo_conf_file();
	read_meteo_conf();

	int counter = 350; // раз в 5 минут
	for( ;; )
	{
		if (glob_status_err)
			break;

		if ( !(glob_status_reg & STATUS_WIFI_STA_CONNECTED) )
		{
			counter = 350;
			goto for_end;
		}

		if (glob_status_reg & STATUS_METEO_CITY_SEARCH)
		{
			http_city_search();
		}

		if (!(glob_status_reg & STATUS_METEO_ON) )
			goto for_end;

		if (counter++ < 350)
			goto for_end;
		counter = 0;

		http_meteo_get();

		for_end:
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
	vTaskDelete(NULL);
}
