/*
 * wifipage.c
 *
 *  Created on: 7 дек. 2023 г.
 *      Author: user
 */

#include "wifi.h"


#define AP_INFO_ARR_SIZE 10

extern bool glob_wifi_STA_run;
extern lv_obj_t *sub_wifi_page;

extern lv_obj_t *glob_busy_indicator;
extern uint32_t glob_status_reg;

static const char *TAG = "wifi";

static wifi_ap_record_t *ap_info = NULL;

struct Wifi_page_obj {
	lv_obj_t *btn_scan;
	lv_obj_t *wifi_switch;
	lv_obj_t *list;
	lv_timer_t *timer;
};

static struct Wifi_page_obj *wifi_page_obj = NULL;

static void wifi_scan_starting_heandler(lv_event_t *e);
static void wifi_connect_step1(lv_event_t *e);
static void wifi_connect_step2(lv_event_t *e);
static void wifi_switch_heandler(lv_event_t *e);
static void wifi_list_item(lv_obj_t **btn, lv_coord_t w, lv_coord_t h, wifi_ap_record_t *ap_record);

static void timer_loop(lv_timer_t *timer);
static void wifi_scan_done(void);

static void print_auth_mode(int authmode, char *buff, uint8_t size)
{
	switch (authmode)
	{
	case WIFI_AUTH_OPEN:
		snprintf(buff, size, "OPEN");
		break;
	case WIFI_AUTH_OWE:
		snprintf(buff, size, "OWE");
		break;
	case WIFI_AUTH_WEP:
		snprintf(buff, size, "WEP");
		break;
	case WIFI_AUTH_WPA_PSK:
		snprintf(buff, size, "WPA_PSK");
		break;
	case WIFI_AUTH_WPA2_PSK:
		snprintf(buff, size, "WPA2_PSK");
		break;
	case WIFI_AUTH_WPA_WPA2_PSK:
		snprintf(buff, size, "WPA_WPA2_PSK");
		break;
	case WIFI_AUTH_WPA2_ENTERPRISE:
		snprintf(buff, size, "WPA2_ENTERPRISE");
		break;
	case WIFI_AUTH_WPA3_PSK:
		snprintf(buff, size, "WPA3_PSK");
		break;
	case WIFI_AUTH_WPA2_WPA3_PSK:
		snprintf(buff, size, "WPA2_WPA3_PSK");
		break;
	default:
		snprintf(buff, size, "AUTH_UNKNOWN");
		break;
	}
}

static void print_cipher_type(int pairwise_cipher, int group_cipher)
{
	switch (pairwise_cipher) {
	case WIFI_CIPHER_TYPE_NONE:
		ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_NONE");
		break;
	case WIFI_CIPHER_TYPE_WEP40:
		ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_WEP40");
		break;
	case WIFI_CIPHER_TYPE_WEP104:
		ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_WEP104");
		break;
	case WIFI_CIPHER_TYPE_TKIP:
		ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_TKIP");
		break;
	case WIFI_CIPHER_TYPE_CCMP:
		ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_CCMP");
		break;
	case WIFI_CIPHER_TYPE_TKIP_CCMP:
		ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_TKIP_CCMP");
		break;
	case WIFI_CIPHER_TYPE_AES_CMAC128:
		ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_AES_CMAC128");
		break;
	case WIFI_CIPHER_TYPE_SMS4:
		ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_SMS4");
		break;
	case WIFI_CIPHER_TYPE_GCMP:
		ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_GCMP");
		break;
	case WIFI_CIPHER_TYPE_GCMP256:
		ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_GCMP256");
		break;
	default:
		ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_UNKNOWN");
		break;
	}

	switch (group_cipher) {
	case WIFI_CIPHER_TYPE_NONE:
		ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_NONE");
		break;
	case WIFI_CIPHER_TYPE_WEP40:
		ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_WEP40");
		break;
	case WIFI_CIPHER_TYPE_WEP104:
		ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_WEP104");
		break;
	case WIFI_CIPHER_TYPE_TKIP:
		ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_TKIP");
		break;
	case WIFI_CIPHER_TYPE_CCMP:
		ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_CCMP");
		break;
	case WIFI_CIPHER_TYPE_TKIP_CCMP:
		ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_TKIP_CCMP");
		break;
	case WIFI_CIPHER_TYPE_SMS4:
		ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_SMS4");
		break;
	case WIFI_CIPHER_TYPE_GCMP:
		ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_GCMP");
		break;
	case WIFI_CIPHER_TYPE_GCMP256:
		ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_GCMP256");
		break;
	default:
		ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_UNKNOWN");
		break;
	}
}



static void wifi_list_item(lv_obj_t **btn, lv_coord_t w, lv_coord_t h, wifi_ap_record_t *ap_record)
{
	lv_obj_set_size(*btn, w, h);

	lv_obj_t *lbl_icon = lv_label_create(*btn);
	lv_label_set_text(lbl_icon, LV_SYMBOL_WIFI);
	lv_obj_align(lbl_icon, LV_ALIGN_LEFT_MID, 0, 0);

	lv_obj_t *lbl_ssid = lv_label_create(*btn);
	// if (memcmp(ap_info.bssid, ap_record->bssid, 6) == 0)
	lv_label_set_text_fmt(lbl_ssid, "%s", ap_record->ssid);
	// else
	// lv_label_set_text_fmt(lbl_ssid, "%s", ap_record->ssid);
	lv_obj_align_to(lbl_ssid, lbl_icon, LV_ALIGN_OUT_RIGHT_BOTTOM, 10, -10);

	lv_obj_t *lbl_bssid = lv_label_create(*btn);
	lv_label_set_text_fmt(lbl_bssid, "(%02X:%02X:%02X:%02X:%02X:%02X)",
			ap_record->bssid[0], ap_record->bssid[1], ap_record->bssid[2],
			ap_record->bssid[3], ap_record->bssid[4], ap_record->bssid[5]);
	lv_obj_align_to(lbl_bssid, lbl_ssid, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 0);

	lv_obj_t *lbl_rssi = lv_label_create(*btn);
	lv_label_set_text_fmt(lbl_rssi, "%d", ap_record->rssi);
	lv_obj_align(lbl_rssi, LV_ALIGN_RIGHT_MID, 0, 0);

	lv_obj_t *lbl_authmode = lv_label_create(*btn);
	char buff[16] = {0};
	print_auth_mode(ap_record->authmode, buff, sizeof(buff));
	lv_label_set_text_fmt(lbl_authmode, "(%s)", buff);
	lv_obj_align_to(lbl_authmode, lbl_rssi, LV_ALIGN_OUT_LEFT_MID, -10, 0);
}

static void wifi_switch_heandler(lv_event_t *e)
{
	if (lv_obj_has_state(wifi_page_obj->wifi_switch, LV_STATE_CHECKED))
	{
		esp_wifi_start();
		lv_event_send(wifi_page_obj->btn_scan, LV_EVENT_CLICKED, 0);
		glob_wifi_STA_run = true;
	}
	else
	{
		esp_wifi_stop();
		lv_obj_clean(wifi_page_obj->list);
		lv_obj_add_state(wifi_page_obj->btn_scan, LV_STATE_DISABLED);
		glob_wifi_STA_run = false;
	}
}

static void wifi_connect_step2(lv_event_t *e)
{
	lv_obj_t *main_widget = e->user_data;
	lv_obj_t *text_area = lv_obj_get_child(main_widget, 2);

	const char *pwd = lv_textarea_get_text(text_area);

	if (strlen(pwd) < 8)
	{
		lv_obj_t *mbox1 = lv_msgbox_create(NULL, "Error", "Password size error!", 0, true);
		lv_obj_center(mbox1);
		return;
	}

	if (glob_busy_indicator == NULL)
		glob_busy_indicator = create_busy_indicator(lv_scr_act(), LCD_H_RES, LCD_V_RES, 100, 100, LV_OPA_70);

	wifi_ap_record_t *ap_info = main_widget->user_data;
	wifi_config_t wifi_config = {
			.sta = {
					//					.ssid = ap_info->ssid,
					//					.password = lv_textarea_get_text(text_area),
					/* Authmode threshold resets to WPA2 as default if password matches WPA2 standards (pasword len => 8).
					 * If you want to connect the device to deprecated WEP/WPA networks, Please set the threshold value
					 * to WIFI_AUTH_WEP/WIFI_AUTH_WPA_PSK and set the password with length and format matching to
					 * WIFI_AUTH_WEP/WIFI_AUTH_WPA_PSK standards.
					 */
					.threshold.authmode = ap_info->authmode,//WIFI_AUTH_WPA2_PSK,
					//            .sae_pwe_h2e = ESP_WIFI_SAE_MODE,
					//            .sae_h2e_identifier = "",
			},
	};

	memcpy(wifi_config.sta.ssid, ap_info->ssid, sizeof(wifi_config.sta.ssid));
	memcpy(wifi_config.sta.password, pwd, strlen(pwd));

	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
	ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config) );
	esp_wifi_disconnect();

	glob_status_reg |= STATUS_WIFI_STA_CONNECTING;
	esp_wifi_connect();
}

static void wifi_connect_step1(lv_event_t *e)
{
	wifi_ap_record_t *ap_info = e->user_data;

	// основное окно
	lv_obj_t *main_widget = lv_obj_create(lv_scr_act());
	lv_obj_set_scroll_dir(main_widget, LV_DIR_NONE);
	lv_obj_set_size(main_widget, 800, 480);

	lv_color_t bg_color = lv_obj_get_style_bg_color(main_widget, 0);
	if(lv_color_brightness(bg_color) > 127)
		lv_obj_set_style_bg_color(main_widget, lv_color_darken(lv_obj_get_style_bg_color(main_widget, 0), 20), 0);
	else
		lv_obj_set_style_bg_color(main_widget, lv_color_darken(lv_obj_get_style_bg_color(main_widget, 0), 50), 0);

	lv_obj_set_user_data(main_widget, ap_info);

	// кнопка закрытия окна
	lv_obj_t *btn_close = lv_btn_create(main_widget);
	lv_obj_set_size(btn_close, 32, 32);
	lv_obj_align(btn_close, LV_ALIGN_TOP_RIGHT, 0, 0);
	lv_obj_add_event_cb(btn_close, delete_obj_handler, LV_EVENT_CLICKED, main_widget);

	lv_obj_t *btn_close_lbl = lv_label_create(btn_close);
	lv_label_set_text(btn_close_lbl, LV_SYMBOL_CLOSE);
	lv_obj_center(btn_close_lbl);

	// клавиатура
	lv_obj_t *kb = lv_keyboard_create(main_widget);
	lv_obj_align(kb, LV_ALIGN_BOTTOM_MID, 0, 0);

	lv_obj_add_event_cb(kb, delete_obj_handler, LV_EVENT_CANCEL, main_widget);
	lv_obj_add_event_cb(kb, wifi_connect_step2, LV_EVENT_READY, main_widget);

	// поле ввода пароля
	lv_obj_t *ta;
	ta = lv_textarea_create(main_widget);
	lv_obj_set_size(ta, 760, 60);
	lv_obj_align_to(ta, kb, LV_ALIGN_OUT_TOP_MID, 0, -10);

	lv_textarea_set_max_length(ta, 64);
	lv_textarea_set_placeholder_text(ta, "Enter wifi password");
	//	lv_obj_add_event_cb(ta, ta_event_cb, LV_EVENT_ALL, kb);

	lv_keyboard_set_textarea(kb, ta);

	// текстовые поля с информацией от wifi
	lv_obj_t *wifi_info_left = lv_label_create(main_widget);

	lv_label_set_text(wifi_info_left,
			"SSID:\n"
			"BSSID:\n"
			"Auth mode:\n"
			"RSSI:\n");
	lv_obj_align(wifi_info_left, LV_ALIGN_TOP_LEFT, 0, 0);

	lv_obj_t *wifi_info_right = lv_label_create(main_widget);


	char auth[16] = {0};
	print_auth_mode(ap_info->authmode, auth, sizeof(auth));

	lv_label_set_text_fmt(wifi_info_right,
			"%s\n"
			"%02X:%02X:%02X:%02X:%02X:%02X\n"
			"%s\n"
			"%d\n",
			ap_info->ssid,
			ap_info->bssid[0], ap_info->bssid[1], ap_info->bssid[2], ap_info->bssid[3], ap_info->bssid[4], ap_info->bssid[5],
			auth,
			ap_info->rssi);
	lv_obj_align_to(wifi_info_right, wifi_info_left, LV_ALIGN_OUT_RIGHT_TOP, 30, 0);
}

static void wifi_scan_starting_heandler(lv_event_t *e)
{
	//	esp_wifi_clear_ap_list();
	esp_wifi_scan_start(NULL, false);

	glob_status_reg |= STATUS_WIFI_SCANNING;

	lv_obj_add_state(wifi_page_obj->btn_scan, LV_STATE_DISABLED);
	lv_obj_t *section = wifi_page_obj->list->parent;
	glob_busy_indicator = create_busy_indicator(section, lv_obj_get_width(section), lv_obj_get_height(section), 80, 80, LV_OPA_70);
}

static void wifi_scan_done(void)
{
	lv_obj_clear_state(wifi_page_obj->btn_scan, LV_STATE_DISABLED);

	uint16_t number = AP_INFO_ARR_SIZE;
	uint16_t ap_count = 0;

	esp_wifi_scan_get_ap_records(&number, ap_info);
	esp_wifi_scan_get_ap_num(&ap_count);
	ESP_LOGI(TAG, "Total APs scanned = %u", ap_count);
	for (int i = 0; (i < AP_INFO_ARR_SIZE) && (i < ap_count); i++) {
		ESP_LOGI(TAG, "SSID \t\t%s", ap_info[i].ssid);
		ESP_LOGI(TAG, "RSSI \t\t%d", ap_info[i].rssi);
		//        print_auth_mode(ap_info[i].authmode);
		if (ap_info[i].authmode != WIFI_AUTH_WEP) {
			print_cipher_type(ap_info[i].pairwise_cipher, ap_info[i].group_cipher);
		}
		ESP_LOGI(TAG, "Channel \t\t%d\n", ap_info[i].primary);
	}

	lv_obj_clean(wifi_page_obj->list);

	for (int i = 0; (i < AP_INFO_ARR_SIZE) && (i < ap_count); i++)
	{
		lv_obj_t *btn = lv_list_add_btn(wifi_page_obj->list, 0, 0);
		wifi_list_item(&btn, 495, 50, &(ap_info[i]));
		lv_obj_add_event_cb(btn, wifi_connect_step1, LV_EVENT_CLICKED, &(ap_info[i]));
	}
}

void create_wifi_sub_page(lv_event_t *e)
{
	lv_obj_set_style_pad_hor(sub_wifi_page, 20, 0);
	lv_obj_t *section = lv_menu_section_create(sub_wifi_page);

	wifi_page_obj = malloc(sizeof(struct Wifi_page_obj));
	create_switch(section, LV_SYMBOL_SETTINGS, "Enable", glob_wifi_STA_run, &(wifi_page_obj->wifi_switch));
	create_list(section, 495, 280, &(wifi_page_obj->list));
	create_button(section, "Scan", 70, 40, &(wifi_page_obj->btn_scan));

	if (glob_wifi_STA_run)
	{
		lv_obj_clear_state(wifi_page_obj->list, LV_STATE_DISABLED);
		lv_obj_clear_state(wifi_page_obj->btn_scan, LV_STATE_DISABLED);
		lv_event_send(wifi_page_obj->list, LV_EVENT_CLICKED, 0);
	}
	else
	{
		lv_obj_add_state(wifi_page_obj->list, LV_STATE_DISABLED);
		lv_obj_add_state(wifi_page_obj->btn_scan, LV_STATE_DISABLED);
	}

	lv_obj_add_event_cb(wifi_page_obj->btn_scan, wifi_scan_starting_heandler, LV_EVENT_CLICKED, 0);
	lv_obj_add_event_cb(wifi_page_obj->wifi_switch, wifi_switch_heandler, LV_EVENT_CLICKED, 0);

	ap_info = calloc(AP_INFO_ARR_SIZE, sizeof(wifi_ap_record_t));

	wifi_page_obj->timer = lv_timer_create(timer_loop, 1000, 0);

	//	// не срабатывает unregister
	//	if (first_start)
	//	{
	//		esp_event_handler_instance_register(WIFI_EVENT, WIFI_EVENT_SCAN_DONE, &wifi_scan_done_handler, 0, 0);
	//		first_start = false;
	//	}
}

static void timer_loop(lv_timer_t *timer)
{
	if (!(glob_status_reg & STATUS_WIFI_SCANNING) &&     // Если не идёт сканирование
			(glob_status_reg & STATUS_WIFI_SCAN_DONE))    // И сканирование завершено
	{
		glob_status_reg &= ~STATUS_WIFI_SCAN_DONE;
		wifi_scan_done();
	}
}

void free_wifi_sub_page(void)
{
	if (ap_info != NULL)
	{
		free(ap_info);
		ap_info = NULL;
	}

	if (wifi_page_obj != NULL)
	{
		lv_timer_del(wifi_page_obj->timer);
		free(wifi_page_obj);
		wifi_page_obj = NULL;
	}
}
