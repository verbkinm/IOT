/*
 * wearther.c
 *
 *  Created on: 18 дек. 2023 г.
 *      Author: user
 */

#include "weather.h"

//#define MAX_HTTP_REC_BUFFER 4096

extern uint32_t glob_status_reg;
extern uint32_t glob_status_err;

static char *city_search = NULL;

static const char *TAG = "weather";

static open_meteo_data_t *open_meteo = NULL;
static const char *city_url = "https://geocoding-api.open-meteo.com/v1/search?count=20&language=ru&format=json&name=";
static const char *meteo_url = "https://api.open-meteo.com/v1/forecast?forecast_days=3&hourly=temperature_2m,relative_humidity_2m,apparent_temperature,precipitation,rain,showers,snowfall,surface_pressure,cloud_cover,wind_speed_10m,wind_direction_10m,wind_gusts_10m&timeformat=unixtime&timezone=auto&latitude=";

static void check_meteo_conf_file(void);
static void read_meteo_conf(void);
static char *generate_url_meteo(void);
static bool parse_open_meteo_weather(const char *data, open_meteo_data_t *open_meteo_week);
static void http_city_search(void);
static void http_meteo_to_file(void);
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
	char *response_buffer = calloc(1, BUFSIZ);

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
		int ret = esp_http_client_read(client, response_buffer, BUFSIZ);
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
	if (latitude == NULL)
		ESP_LOGE(TAG, "latitude error");

	char *longitude = NULL;
	get_meteo_config_value("longitude", &longitude);
	if (longitude == NULL)
		ESP_LOGE(TAG, "longitude error");

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

static bool parse_open_meteo_weather(const char *data, open_meteo_data_t *open_meteo_week)
{
	if (data == NULL || open_meteo_week == NULL)
		return false;

	cJSON *monitor = cJSON_Parse(data);
	if (monitor == NULL)
		return false;

	cJSON *obj_parent = cJSON_GetObjectItemCaseSensitive(monitor, "hourly");
	if (obj_parent == NULL)
		goto bad_end;

	cJSON *obj_child= cJSON_GetObjectItemCaseSensitive(obj_parent, "time");
	if (obj_child == NULL)
		goto bad_end;

	cJSON *obj;
	int i = 0;
	cJSON_ArrayForEach(obj, obj_child)
	open_meteo_week[i++].time = cJSON_GetNumberValue(obj);

	obj_child= cJSON_GetObjectItemCaseSensitive(obj_parent, "temperature_2m");
	if (obj_child == NULL)
		goto bad_end;

	i = 0;
	cJSON_ArrayForEach(obj, obj_child)
	{
		open_meteo_week[i++].temperature = cJSON_GetNumberValue(obj);
		if (i > OPEN_METEO_WEEK_SIZE)
			goto bad_end;
	}

	obj_child = cJSON_GetObjectItemCaseSensitive(obj_parent, "relative_humidity_2m");
	if (obj_child == NULL)
		goto bad_end;

	i = 0;
	cJSON_ArrayForEach(obj, obj_child)
	{
		open_meteo_week[i++].relative_humidity = cJSON_GetNumberValue(obj);
		if (i > OPEN_METEO_WEEK_SIZE)
			goto bad_end;
	}

	obj_child= cJSON_GetObjectItemCaseSensitive(obj_parent, "apparent_temperature");
	if (obj_child == NULL)
		goto bad_end;

	i = 0;
	cJSON_ArrayForEach(obj, obj_child)
	{
		open_meteo_week[i++].apparent_temperature = cJSON_GetNumberValue(obj);
		if (i > OPEN_METEO_WEEK_SIZE)
			goto bad_end;
	}

	obj_child= cJSON_GetObjectItemCaseSensitive(obj_parent, "precipitation");
	if (obj_child == NULL)
		goto bad_end;

	i = 0;
	cJSON_ArrayForEach(obj, obj_child)
	{
		open_meteo_week[i++].precipitation = cJSON_GetNumberValue(obj);
		if (i > OPEN_METEO_WEEK_SIZE)
			goto bad_end;
	}

	obj_child= cJSON_GetObjectItemCaseSensitive(obj_parent, "rain");
	if (obj_child == NULL)
		goto bad_end;

	i = 0;
	cJSON_ArrayForEach(obj, obj_child)
	{
		open_meteo_week[i++].rain = cJSON_GetNumberValue(obj);
		if (i > OPEN_METEO_WEEK_SIZE)
			goto bad_end;
	}

	obj_child= cJSON_GetObjectItemCaseSensitive(obj_parent, "showers");
	if (obj_child == NULL)
		goto bad_end;

	i = 0;
	cJSON_ArrayForEach(obj, obj_child)
	{
		open_meteo_week[i++].showers = cJSON_GetNumberValue(obj);
		if (i > OPEN_METEO_WEEK_SIZE)
			goto bad_end;
	}

	obj_child= cJSON_GetObjectItemCaseSensitive(obj_parent, "snowfall");
	if (obj_child == NULL)
		goto bad_end;

	i = 0;
	cJSON_ArrayForEach(obj, obj_child)
	{
		open_meteo_week[i++].snowfall = cJSON_GetNumberValue(obj);
		if (i > OPEN_METEO_WEEK_SIZE)
			goto bad_end;
	}

	obj_child= cJSON_GetObjectItemCaseSensitive(obj_parent, "surface_pressure");
	if (obj_child == NULL)
		goto bad_end;

	i = 0;
	cJSON_ArrayForEach(obj, obj_child)
	{
		open_meteo_week[i++].surface_pressure = cJSON_GetNumberValue(obj) * 0.75006; // hPa to mm Hg
		if (i > OPEN_METEO_WEEK_SIZE)
			goto bad_end;
	}

	obj_child= cJSON_GetObjectItemCaseSensitive(obj_parent, "cloud_cover");
	if (obj_child == NULL)
		goto bad_end;

	i = 0;
	cJSON_ArrayForEach(obj, obj_child)
	{
		open_meteo_week[i++].cloud_cover = cJSON_GetNumberValue(obj);
		if (i > OPEN_METEO_WEEK_SIZE)
			goto bad_end;
	}

	obj_child= cJSON_GetObjectItemCaseSensitive(obj_parent, "wind_speed_10m");
	if (obj_child == NULL)
		goto bad_end;

	i = 0;
	cJSON_ArrayForEach(obj, obj_child)
	{
		open_meteo_week[i++].wind_speed = cJSON_GetNumberValue(obj);
		if (i > OPEN_METEO_WEEK_SIZE)
			goto bad_end;
	}

	obj_child= cJSON_GetObjectItemCaseSensitive(obj_parent, "wind_direction_10m");
	if (obj_child == NULL)
		goto bad_end;

	i = 0;
	cJSON_ArrayForEach(obj, obj_child)
	{
		open_meteo_week[i++].wind_direction = cJSON_GetNumberValue(obj);
		if (i > OPEN_METEO_WEEK_SIZE)
			goto bad_end;
	}

	obj_child= cJSON_GetObjectItemCaseSensitive(obj_parent, "wind_gusts_10m");
	if (obj_child == NULL)
		goto bad_end;

	i = 0;
	cJSON_ArrayForEach(obj, obj_child)
	{
		open_meteo_week[i++].wind_gusts = cJSON_GetNumberValue(obj);
		if (i > OPEN_METEO_WEEK_SIZE)
			goto bad_end;
	}

	cJSON_Delete(monitor);
	return true;

	bad_end:
	cJSON_Delete(monitor);
	return false;
}

const open_meteo_data_t *service_weather_get_current_meteo_data(void)
{
	if (open_meteo == NULL)
		return NULL;

	time_t now;
	time(&now);

	if (now < open_meteo[0].time)
		return &open_meteo[0];

	for (int i = 1; i < OPEN_METEO_WEEK_SIZE - 1; ++i)
	{
		if (now > open_meteo[i - 1].time && now < open_meteo[i + 1].time)
		{
			if (now > (open_meteo[i + 1].time - (60 * 30)))
				return &open_meteo[i + 1];
			else if (now < (open_meteo[i - 1].time + (60 * 30)))
				return &open_meteo[i - 1];
			else
				return &open_meteo[i];
		}
	}

	if (now >= open_meteo[OPEN_METEO_WEEK_SIZE - 1].time)
		return &open_meteo[OPEN_METEO_WEEK_SIZE - 1];

	return NULL;
}

const open_meteo_data_t *service_weather_get_meteo_data(void)
{
	return open_meteo;
}

void service_weather_get_range(open_meteo_data_t *ret_min, open_meteo_data_t *ret_max)
{
	if (open_meteo == NULL || ret_min == NULL || ret_max == NULL)
		return;

	memcpy(ret_min, &open_meteo[0], sizeof(struct Open_Meteo_Data));
	memcpy(ret_max, &open_meteo[0], sizeof(struct Open_Meteo_Data));

	for (int i = 1; i < OPEN_METEO_WEEK_SIZE; ++i)
	{
		ret_max->temperature = MAX(ret_max->temperature, open_meteo[i].temperature);
		ret_max->apparent_temperature = MAX(ret_max->apparent_temperature, open_meteo[i].apparent_temperature);
		ret_max->surface_pressure = MAX(ret_max->surface_pressure, open_meteo[i].surface_pressure);
		ret_max->precipitation = MAX(ret_max->precipitation, open_meteo[i].precipitation);
		ret_max->rain = MAX(ret_max->rain, open_meteo[i].rain);
		ret_max->showers = MAX(ret_max->showers, open_meteo[i].showers);
		ret_max->snowfall = MAX(ret_max->snowfall, open_meteo[i].snowfall);
		ret_max->wind_speed = MAX(ret_max->wind_speed, open_meteo[i].wind_speed);
		ret_max->wind_gusts = MAX(ret_max->wind_gusts, open_meteo[i].wind_gusts);
		ret_max->relative_humidity = MAX(ret_max->relative_humidity, open_meteo[i].relative_humidity);
		ret_max->cloud_cover = MAX(ret_max->cloud_cover, open_meteo[i].cloud_cover);
		ret_max->wind_direction = MAX(ret_max->wind_direction, open_meteo[i].wind_direction);

		ret_min->temperature = MIN(ret_min->temperature, open_meteo[i].temperature);
		ret_min->apparent_temperature = MIN(ret_min->apparent_temperature, open_meteo[i].apparent_temperature);
		ret_min->surface_pressure = MIN(ret_min->surface_pressure, open_meteo[i].surface_pressure);
		ret_min->precipitation = MIN(ret_min->precipitation, open_meteo[i].precipitation);
		ret_min->rain = MIN(ret_min->rain, open_meteo[i].rain);
		ret_min->showers = MIN(ret_min->showers, open_meteo[i].showers);
		ret_min->snowfall = MIN(ret_min->snowfall, open_meteo[i].snowfall);
		ret_min->wind_speed = MIN(ret_min->wind_speed, open_meteo[i].wind_speed);
		ret_min->wind_gusts = MIN(ret_min->wind_gusts, open_meteo[i].wind_gusts);
		ret_min->relative_humidity = MIN(ret_min->relative_humidity, open_meteo[i].relative_humidity);
		ret_min->cloud_cover = MIN(ret_min->cloud_cover, open_meteo[i].cloud_cover);
		ret_min->wind_direction = MIN(ret_min->wind_direction, open_meteo[i].wind_direction);
	}
}

void service_weather_set_city(const char* city)
{
	if (city_search != NULL)
		free(city_search);

	city_search = url_encode(city);
}

const char *service_weather_get_city(void)
{
	return city_search;
}

static void http_meteo_to_file(void)
{
	char *response_buffer = malloc(BUFSIZ);
	if (response_buffer == NULL)
	{
		ESP_LOGE(TAG, "response_buffer = NULL");
		return;
	}

	char *url = generate_url_meteo();
	if (url == NULL)
	{
		ESP_LOGE(TAG, "url = NULL");
		free(response_buffer);
		return;
	}

	esp_http_client_config_t config = {
			.url = url,
			.crt_bundle_attach = esp_crt_bundle_attach,
	};
	esp_http_client_handle_t client = esp_http_client_init(&config);

	esp_http_client_open(client, 0);
	esp_http_client_fetch_headers(client);

	FILE *file = fopen(METEO_WEEK_PATH, "w");
	if (file == NULL)
	{
		ESP_LOGE(TAG, "cant write \"%s\" file", METEO_WEEK_PATH);
		goto file_error;
	}

	int bs = 0;
	while (true)
	{
		int ret = esp_http_client_read(client, response_buffer, BUFSIZ);
		if (ret < 1)
			break;

		bs += ret;

		//		response_buffer[ret] = '\0';
		fwrite(response_buffer, ret, 1, file);
		//		printf("%s", response_buffer);
		//		fflush(stdout);
	}
	//	printf("\n");
	//	fflush(stdout);


//	ESP_LOGI(TAG, "recive size = %d", bs);

	fclose(file);
	file_error:
	esp_http_client_cleanup(client);
	free(url);
	free(response_buffer);
}

bool service_weather_parse_meteo_data(void)
{
	bool result = false;

	struct stat st;
	stat(METEO_WEEK_PATH, &st);
	size_t size = st.st_size;

	if (size == 0)
	{
		ESP_LOGE(TAG, "size = 0");
		return false;
	}

	char *content = calloc(1, size + 1);
	if (content == NULL)
	{
		ESP_LOGE(TAG, "content error calloc");
		return false;
	}

	FILE *file = fopen(METEO_WEEK_PATH, "r");
	if (file == NULL)
	{
		ESP_LOGE(TAG, "cant read \"%s\" file", METEO_WEEK_PATH);
		goto end;
	}

//	ESP_LOGI(TAG, "file size = %d", size);

	fread(content, size, 1, file);
	//	printf("%s\n", content);
	fclose(file);

	if (open_meteo != NULL)
		free(open_meteo);

	open_meteo = calloc(OPEN_METEO_WEEK_SIZE, sizeof(open_meteo_data_t));
	if (open_meteo == NULL)
	{
		ESP_LOGE(TAG, "open_meteo = NULL");
		goto end;
	}

	if (!parse_open_meteo_weather(content, open_meteo))
	{
		ESP_LOGE(TAG, "error parse_open_meteo_weather");
		free(open_meteo);
		open_meteo = NULL;
		goto end;
	}

	result = true;

	end:
	free(content);

	return result;
}

static void debug_open_meteo()
{
	if (open_meteo == NULL)
		return;

	for (int i = 0; i < OPEN_METEO_WEEK_SIZE; ++i)
	{
		printf("%d:\ntime: %lld\n", i, open_meteo[i].time);
	}

}

void weather_service_task(void *pvParameters)
{
	check_meteo_conf_file();
	read_meteo_conf();

	int counter = 350; //350 =  1 раз в 5 минут
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
			http_city_search();

		if (!((glob_status_reg & STATUS_METEO_ON) && (glob_status_reg & STATUS_TIME_SYNC)))
			goto for_end;

		if (counter++ < 350)
			goto for_end;
		counter = 0;

		do
		{
			//			vTaskDelay(500 / portTICK_PERIOD_MS);
			http_meteo_to_file();
		}
		while(service_weather_parse_meteo_data() == false);
//		debug_open_meteo();


		for_end:
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
	vTaskDelete(NULL);
}
