/*
 * update.c
 *
 *  Created on: 23 янв. 2024 г.
 *      Author: user
 */

#include "update.h"

extern lv_obj_t *sub_update_page;

static const char *TAG = "update";

static lv_obj_t *btn_check_update;
static lv_obj_t *btn_update;
static lv_obj_t *btn_facrory_reset;
static lv_obj_t *busy_ind = NULL;
static lv_timer_t *timer = NULL;
static lv_obj_t *new_version_lbl = NULL;

static lv_obj_t *dialog_box;
static const char *btns_txt[] = {"Да", "Нет", NULL};

static const char *current_version_str = "Текущая версия программного обеспечения: ";
static const char *build_date_str = "Дата сборки: ";
static const char *new_version_str = "Доступная версия: ";

static const char *url_update_info = "https://ota.verbkinm.ru/other/esp32/meteo_home_II/update_info.json";
static char *url_update = NULL;

static void update_check_handler(lv_event_t *e);
static void update_handler(lv_event_t *e);
static void apply_update_handler(lv_event_t * e);
static void factory_reset_handler(lv_event_t * e);
static void apply_factory_reset_handler(lv_event_t * e);
static esp_err_t http_event_handler(esp_http_client_event_t *evt);

static void timer_loop(lv_timer_t *timer);
static void timer_once_loop(lv_timer_t *timer);

static bool parse_http_response(const char* content);

static esp_err_t http_event_handler(esp_http_client_event_t *evt)
{
	//	esp_http_client_handle_t client = evt->client;
	switch (evt->event_id)
	{
	case HTTP_EVENT_ERROR:
		ESP_LOGI(TAG, "HTTP_EVENT_ERROR");
		break;
	case HTTP_EVENT_ON_CONNECTED:
		ESP_LOGI(TAG, "HTTP_EVENT_ON_CONNECTED");
		break;
	case HTTP_EVENT_HEADER_SENT:
		ESP_LOGI(TAG, "HTTP_EVENT_HEADER_SENT");
		break;
	case HTTP_EVENT_ON_HEADER:
		ESP_LOGI(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
		break;
	case HTTP_EVENT_ON_DATA:
		ESP_LOGI(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
		break;
	case HTTP_EVENT_ON_FINISH:
		ESP_LOGI(TAG, "HTTP_EVENT_ON_FINISH");
		break;
	case HTTP_EVENT_DISCONNECTED:
		ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
		clear_busy_indicator(&busy_ind);
		break;
	case HTTP_EVENT_REDIRECT:
		ESP_LOGI(TAG, "HTTP_EVENT_REDIRECT");
		break;
	}
	return ESP_OK;
}

static void update_check_handler(lv_event_t *e)
{
	if ( !(glob_get_status_reg() & STATUS_IP_GOT) )
	{
		create_msgbox_not_connected();
		return;
	}

	busy_ind = create_busy_indicator(lv_obj_get_child(lv_scr_act(), 1), LCD_H_RES, LCD_V_RES - LCD_PANEL_STATUS_H, 100, 100, LV_OPA_70);
}

static void update_handler(lv_event_t *e)
{
	if ( !(glob_get_status_reg() & STATUS_IP_GOT) )
	{
		create_msgbox_not_connected();
		return;
	}

	dialog_box = create_dialog_box(NULL, ATTENTION_STR, "Вы подтверждаете обновление программного обеспечения устройства?", btns_txt);
	lv_obj_add_event_cb(dialog_box, apply_update_handler, LV_EVENT_VALUE_CHANGED, NULL);
}

static void apply_update_handler(lv_event_t * e)
{
	lv_obj_t *obj = lv_event_get_current_target(e);

	if (strcmp(lv_msgbox_get_active_btn_text(obj), btns_txt[0]) == 0)
	{
		glob_set_bits_status_reg(STATUS_UPDATE);

		full_screen_page_init("Обновление...", UPDATE_IMG);
		lv_timer_create(timer_once_loop, 1000, 0);
	}

	lv_msgbox_close(dialog_box);
}

static void factory_reset_handler(lv_event_t * e)
{
	dialog_box = create_dialog_box(NULL, ATTENTION_STR, "Выполнить сброс устройства на заводские настройки?", btns_txt);
	lv_obj_add_event_cb(dialog_box, apply_factory_reset_handler, LV_EVENT_VALUE_CHANGED, NULL);
}

static void apply_factory_reset_handler(lv_event_t * e)
{
	lv_obj_t *obj = lv_event_get_current_target(e);
	if (strcmp(lv_msgbox_get_active_btn_text(obj), btns_txt[0]) == 0)
	{
		if (backtofactory() == ESP_OK)
			esp_restart();
	}

	create_msgbox(NULL, ERROR_STR, "Ошибка сброса на заводские настройки. На устройстве отсутствует заводской раздел.");
}

static void timer_loop(lv_timer_t *timer)
{
	if (busy_ind == NULL)
		return;

	lv_timer_pause(timer);

	esp_http_client_config_t config = {
			.url = url_update_info,
			.crt_bundle_attach = esp_crt_bundle_attach,
			.event_handler = http_event_handler,
	};

	esp_http_client_handle_t client = esp_http_client_init(&config);
	esp_http_client_open(client, 0);
	esp_http_client_fetch_headers(client);

	char *response_buffer = calloc(1, BUFSIZE);
	int ret = esp_http_client_read(client, response_buffer, BUFSIZE);

	if (ret < 1 || parse_http_response(response_buffer) == false)
	{
		lv_obj_add_state(btn_update, LV_STATE_DISABLED);
		lv_label_set_text_fmt(new_version_lbl, "%s %s", new_version_str, "Ошибка сервера обновлений!");
	}

	esp_http_client_cleanup(client);
	free(response_buffer);

	lv_timer_resume(timer);
}

static void timer_once_loop(lv_timer_t *timer)
{
	lv_timer_del(timer);

	esp_err_t ret = ota_firmware(url_update);

	if (ret == ESP_OK)
		full_screen_page_init("Перезагрузите устройство...", UPDATE_OK);
	else
	{
		glob_set_bits_status_err(STATUS_UPDATE_ERROR);
		full_screen_page_init("Ошибка обновления!", UPDATE_FAIL);
	}

	glob_clear_bits_status_reg(STATUS_UPDATE);
}

static bool parse_http_response(const char* content)
{
	cJSON *root = cJSON_Parse(content);
	if (root == NULL)
		return false;

	cJSON *firmware = cJSON_GetObjectItemCaseSensitive(root, "firmware");
	if (firmware == NULL)
		goto bad_end;

	cJSON *version = cJSON_GetObjectItemCaseSensitive(firmware, "version");
	if (version == NULL)
		goto bad_end;

	char *new_version_from_server = cJSON_GetStringValue(version);
	if (new_version_from_server == NULL || strlen(new_version_from_server) == 0)
		goto bad_end;

	cJSON *url = cJSON_GetObjectItemCaseSensitive(firmware, "url");
	char *new_url = cJSON_GetStringValue(url);
	if (new_url == NULL || strlen(new_url) == 0)
		goto bad_end;

	if (url_update != NULL)
		free(url_update);

	url_update = calloc(1, strlen(new_url) + 1);
	strcpy(url_update, new_url);

	lv_label_set_text_fmt(new_version_lbl, "%s %s", new_version_str, new_version_from_server);

	const esp_app_desc_t *esp_app = esp_app_get_description();
	if (strcmp(new_version_from_server, esp_app->version) > 0)
		lv_obj_clear_state(btn_update, LV_STATE_DISABLED);
	else
		lv_obj_add_state(btn_update, LV_STATE_DISABLED);

	cJSON_Delete(root);
	return true;

	bad_end:
	cJSON_Delete(root);
	return false;
}

void create_update_sub_page(lv_event_t *e)
{
	clear_all_sub_page_child();

	lv_obj_set_style_pad_hor(sub_update_page, 20, 0);
	lv_obj_t *section = lv_menu_section_create(sub_update_page);

	const esp_app_desc_t *esp_app = esp_app_get_description();

	char *cur_ver = malloc(strlen(current_version_str) + sizeof(esp_app->version) + 1);
	strcpy(cur_ver, current_version_str);
	strcat(cur_ver, esp_app->version);

	char *date = malloc(strlen(build_date_str) + sizeof(esp_app->date) + 1);
	strcpy(date, build_date_str);
	strcat(date, esp_app->date);

	create_text(section, NULL, cur_ver, LV_MENU_ITEM_BUILDER_VAR_1);
	create_text(section, NULL, date, LV_MENU_ITEM_BUILDER_VAR_1);
	lv_obj_t *obj = create_text(section, NULL, new_version_str, LV_MENU_ITEM_BUILDER_VAR_1);
	new_version_lbl = lv_obj_get_child(obj, 0);

	create_button(section, "Доступная версия", 220, 40, &btn_check_update);
	create_button(section, "Обновить", 220, 40, &btn_update);
	lv_obj_add_state(btn_update, LV_STATE_DISABLED);
	create_button(section, "Заводские настройки", 220, 40, &btn_facrory_reset);
	lv_obj_set_style_bg_color(btn_facrory_reset, lv_color_make(255, 0, 0), 0);

	lv_obj_add_event_cb(btn_check_update, update_check_handler, LV_EVENT_CLICKED, 0);
	lv_obj_add_event_cb(btn_update, update_handler, LV_EVENT_CLICKED, 0);
	lv_obj_add_event_cb(btn_facrory_reset, factory_reset_handler, LV_EVENT_CLICKED, 0);

	free(date);
	free(cur_ver);

	timer = lv_timer_create(timer_loop, 100, 0);
}

void free_update_sub_page(void)
{
	if (timer != NULL)
	{
		lv_timer_del(timer);
		timer = NULL;
	}
	if (url_update != NULL)
	{
		free(url_update);
		url_update = NULL;
	}
}
