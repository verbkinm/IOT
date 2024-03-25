/*
 * wifipage.c
 *
 *  Created on: 7 дек. 2023 г.
 *      Author: user
 */

#include "wifi.h"

#define AP_INFO_ARR_SIZE 10

extern lv_obj_t *sub_wifi_page;

extern lv_font_t ubuntu_mono_14;
extern esp_netif_t *sta_netif;

static const char *TAG = "wifi";

static wifi_ap_record_t *ap_info = NULL;
static bool autoconn_pause_from_scan = false;

struct Wifi_page_obj {
	lv_obj_t *btn_scan;
	lv_obj_t *wifi_switch;
	lv_obj_t *list;
	lv_obj_t *busy_ind;
	lv_timer_t *timer;
};
static struct Wifi_page_obj *wifi_page_obj = NULL;

enum
{
	WAIT_FLAG_NONE = 0,
	WAIT_FLAG_SCAN = 1,
	WAIT_FLAG_CONN = 2
};

static uint8_t wait_flag = WAIT_FLAG_NONE;

static void wifi_scan_starting_heandler(lv_event_t *e);
static void wifi_connect_step1(lv_event_t *e);
static void wifi_connect_step2(lv_event_t *e);
static void wifi_switch_heandler(lv_event_t *e);
static void wifi_list_item(lv_obj_t **btn, lv_coord_t w, lv_coord_t h, wifi_ap_record_t *ap_record);

static void timer_loop(lv_timer_t *timer);
static void conn_step1_timer_loop(lv_timer_t *timer);
static void wifi_disconnect_handler(lv_event_t * e);
static void info_handler(lv_event_t * e);

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
	lv_label_set_text_fmt(lbl_ssid, "%s", ap_record->ssid);

	if (glob_get_status_reg() & STATUS_WIFI_STA_CONNECTED)
	{
		wifi_config_t wifi_config;
		esp_wifi_get_config(WIFI_IF_STA, &wifi_config);

		if (memcmp(ap_record->bssid, wifi_config.sta.bssid, 6) == 0)
			lv_obj_set_style_text_color(lbl_ssid, lv_color_make(0, 200, 00), 0);
	}

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
		glob_set_bits_status_reg(STATUS_WIFI_STA_START);

		bool res = set_wifi_config_value("on", "1");
		if (!res)
			printf("on not write\n");

		lv_obj_clear_state(wifi_page_obj->btn_scan, LV_STATE_DISABLED);
	}
	else
	{
		esp_wifi_stop();
		lv_obj_clean(wifi_page_obj->list);
		lv_obj_add_state(wifi_page_obj->btn_scan, LV_STATE_DISABLED);
		glob_clear_bits_status_reg(STATUS_WIFI_STA_START);

		bool res = set_wifi_config_value("on", "0");
		if (!res)
			printf("on not write\n");
	}
}

static void wifi_connect_step2(lv_event_t *e)
{
	lv_obj_t *widget = e->user_data;
	wifi_ap_record_t *ap_info = widget->user_data;
	lv_obj_t *text_area = lv_obj_get_child(widget, 1);

	const char *pwd = lv_textarea_get_text(text_area);

	if ( (strlen(pwd) < 8) )// || (ap_info->authmode != WIFI_AUTH_OPEN) )
	{
//		printf("pwd = %s\n", pwd);
		create_msgbox(NULL, ATTENTION_STR, "Размер пароля должен быть больше 8-ми символов!");
		return;
	}

	if (wifi_page_obj->busy_ind != NULL)
		clear_busy_indicator(&wifi_page_obj->busy_ind);
	wifi_page_obj->busy_ind = create_busy_indicator(widget, LCD_H_RES, LCD_V_RES - LCD_PANEL_STATUS_H, 100, 100, LV_OPA_70);

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

	glob_set_bits_status_reg(STATUS_WIFI_STA_CONNECTING);
	wait_flag = WAIT_FLAG_CONN;
	esp_wifi_connect();
}

static void wifi_connect_step1(lv_event_t *e)
{
	wifi_ap_record_t *ap_info = e->user_data;

	// основное окно
	lv_obj_t *widget = lv_obj_create(lv_obj_get_child(lv_scr_act(), 1));
	lv_obj_set_scroll_dir(widget, LV_DIR_NONE);
	lv_obj_set_size(widget, LCD_H_RES, LCD_V_RES - LCD_PANEL_STATUS_H);
	lv_obj_set_style_text_font(widget, &ubuntu_mono_14, 0);

	// Фон немного "сереем"
	lv_color_t bg_color = lv_obj_get_style_bg_color(widget, 0);
	if(lv_color_brightness(bg_color) > 127)
		lv_obj_set_style_bg_color(widget, lv_color_darken(lv_obj_get_style_bg_color(widget, 0), 20), 0);
	else
		lv_obj_set_style_bg_color(widget, lv_color_darken(lv_obj_get_style_bg_color(widget, 0), 50), 0);

	lv_obj_set_user_data(widget, ap_info);

	// кнопка закрытия окна
	lv_obj_t *btn_close = lv_btn_create(widget);
	lv_obj_set_size(btn_close, 32, 32);
	lv_obj_set_style_radius(btn_close, 90, 0);
	lv_obj_align(btn_close, LV_ALIGN_TOP_RIGHT, 0, 0);
	lv_obj_add_event_cb(btn_close, delete_obj_handler, LV_EVENT_CLICKED, widget);

	lv_obj_t *btn_close_lbl = lv_label_create(btn_close);
	lv_label_set_text(btn_close_lbl, LV_SYMBOL_CLOSE);
	lv_obj_center(btn_close_lbl);

	// поле ввода пароля
	lv_obj_t *ta;
	ta = lv_textarea_create(widget);
	lv_obj_set_size(ta, 760, 60);
	lv_textarea_set_max_length(ta, 64);
	lv_textarea_set_placeholder_text(ta, "Введите пароль точки доступа wifi");

	// клавиатура
	lv_obj_t *kb = lv_keyboard_create(widget);
	lv_obj_align(kb, LV_ALIGN_BOTTOM_MID, 0, 0);

	lv_obj_add_event_cb(kb, delete_obj_handler, LV_EVENT_CANCEL, widget);
	lv_obj_add_event_cb(kb, wifi_connect_step2, LV_EVENT_READY, widget);

	lv_obj_align_to(ta, kb, LV_ALIGN_OUT_TOP_MID, 0, -10);
	lv_keyboard_set_textarea(kb, ta);

	// текстовые поля с информацией от wifi
	lv_obj_t *wifi_info_left = lv_label_create(widget);

	lv_label_set_text(wifi_info_left,
			"SSID:\n"
			"BSSID:\n"
			"Auth mode:\n"
			"RSSI:\n");
	lv_obj_align(wifi_info_left, LV_ALIGN_TOP_LEFT, 0, 0);

	lv_obj_t *wifi_info_right = lv_label_create(widget);


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


	// Кнопка подключиться/отключиться
	lv_obj_t *btn_con = lv_btn_create(widget);
	lv_obj_set_size(btn_con, 128, 40);
	lv_obj_align_to(btn_con, ta, LV_ALIGN_OUT_RIGHT_TOP, -128, -40 -10);

	lv_obj_t *btn_con_lbl = lv_label_create(btn_con);
	lv_label_set_text(btn_con_lbl, "Ожидайте...");
	lv_obj_center(btn_con_lbl);

	lv_timer_t *timer = lv_timer_create(conn_step1_timer_loop, 500, widget);
	lv_obj_add_event_cb(widget, delete_timer_handler, LV_EVENT_DELETE, timer);
}

static void wifi_disconnect_handler(lv_event_t * e)
{
	set_wifi_config_value("auto", "0");
	glob_clear_bits_status_reg(STATUS_WIFI_AUTOCONNECT);
	esp_wifi_disconnect();
}

static void conn_step1_timer_loop(lv_timer_t *timer)
{
	lv_obj_t *widget = timer->user_data;
	lv_obj_t *btn_con = lv_obj_get_child(widget, 5);
	lv_obj_t *btn_con_lbl = lv_obj_get_child(btn_con, 0);
	wifi_ap_record_t *ap_info = widget->user_data;

	// Если устройство подключено к данной точке доступа.
	if (glob_get_status_reg() & STATUS_WIFI_STA_CONNECTED)
	{
		wifi_config_t wifi_config;
		esp_wifi_get_config(WIFI_IF_STA, &wifi_config);

		if (memcmp(ap_info->bssid, wifi_config.sta.bssid, 6) == 0)
		{
			lv_label_set_text(btn_con_lbl, "Отключиться");
			lv_obj_remove_event_cb(btn_con, wifi_connect_step2);
			lv_obj_remove_event_cb(btn_con, wifi_disconnect_handler);
			lv_obj_add_event_cb(btn_con, wifi_disconnect_handler, LV_EVENT_CLICKED, widget);
		}
	}
	else
	{
		lv_label_set_text(btn_con_lbl, "Подключиться");
		lv_obj_remove_event_cb(btn_con, wifi_disconnect_handler);
		lv_obj_remove_event_cb(btn_con, wifi_connect_step2);
		lv_obj_add_event_cb(btn_con, wifi_connect_step2, LV_EVENT_CLICKED, widget);
	}
}

static void wifi_scan_starting_heandler(lv_event_t *e)
{
	// Если включено автосоединение - запоминаем его состояние и выключаем его
	if (glob_get_status_reg() & STATUS_WIFI_AUTOCONNECT)
	{
		autoconn_pause_from_scan = (glob_get_status_reg() & STATUS_WIFI_AUTOCONNECT) > 0;
		glob_clear_bits_status_reg(STATUS_WIFI_AUTOCONNECT);
	}

	// Если идёт соединение - останавливаем его.
	if (glob_get_status_reg() & STATUS_WIFI_STA_CONNECTING)
	{
		esp_wifi_disconnect();
		glob_clear_bits_status_reg(STATUS_WIFI_STA_CONNECTING);
	}

	esp_wifi_scan_start(NULL, false);

	glob_clear_bits_status_reg(STATUS_WIFI_SCAN_DONE);
	wait_flag = WAIT_FLAG_SCAN;
	glob_set_bits_status_reg(STATUS_WIFI_SCANNING);

	lv_obj_add_state(wifi_page_obj->btn_scan, LV_STATE_DISABLED);
	lv_obj_t *section = wifi_page_obj->list->parent;

	if (wifi_page_obj->busy_ind != NULL)
		clear_busy_indicator(&wifi_page_obj->busy_ind);

	wifi_page_obj->busy_ind = create_busy_indicator(section, lv_obj_get_width(section), lv_obj_get_height(section), 80, 80, LV_OPA_70);
}

static void wifi_scan_done(void)
{
	// Если была остановка автоподключение перед сканирование, включаем его
	if (autoconn_pause_from_scan)
	{
		glob_set_bits_status_reg(STATUS_WIFI_AUTOCONNECT);
		autoconn_pause_from_scan = false;
	}

	lv_obj_clear_state(wifi_page_obj->btn_scan, LV_STATE_DISABLED);

	uint16_t number = AP_INFO_ARR_SIZE;
	uint16_t ap_count = 0;

	esp_wifi_scan_get_ap_records(&number, ap_info);
	esp_wifi_scan_get_ap_num(&ap_count);
	ESP_LOGI(TAG, "Total APs scanned = %u", ap_count);
	for (int i = 0; (i < AP_INFO_ARR_SIZE) && (i < ap_count); i++)
	{
		ESP_LOGI(TAG, "SSID \t\t%s", ap_info[i].ssid);
		ESP_LOGI(TAG, "RSSI \t\t%d", ap_info[i].rssi);
		if (ap_info[i].authmode != WIFI_AUTH_WEP)
			print_cipher_type(ap_info[i].pairwise_cipher, ap_info[i].group_cipher);

		ESP_LOGI(TAG, "Channel \t\t%d\n", ap_info[i].primary);
	}

	if (wifi_page_obj != NULL)
		lv_obj_clean(wifi_page_obj->list);

	for (int i = 0; (i < AP_INFO_ARR_SIZE) && (i < ap_count); i++)
	{
		lv_obj_t *btn = lv_list_add_btn(wifi_page_obj->list, 0, 0); //!!!

		wifi_list_item(&btn, 495, 50, &(ap_info[i]));
		lv_obj_add_event_cb(btn, wifi_connect_step1, LV_EVENT_CLICKED, &(ap_info[i]));
	}

	clear_busy_indicator(&wifi_page_obj->busy_ind);
}

static void info_handler(lv_event_t * e)
{
	lv_obj_t *mbox = create_msgbox(NULL, "IP INFO", "0");
	lv_obj_t *msg_lbl = lv_msgbox_get_text(mbox);
	lv_obj_center(mbox);

	char ip[16] = {0};
	char netmask[16] = {0};
	char dns_server[16] = {0};
	char gw[16] = {0};

	esp_netif_ip_info_t ip_info;
	esp_netif_get_ip_info(sta_netif, &ip_info);

	esp_netif_dns_info_t dns;
	esp_netif_get_dns_info(sta_netif, ESP_NETIF_DNS_MAIN, &dns);
	esp_ip_addr_t ipaddr;
	memcpy(&ipaddr.u_addr.ip4,  &dns.ip, 4);

	esp_ip4addr_ntoa(&ip_info.ip, ip, 15);
	esp_ip4addr_ntoa(&ip_info.netmask, netmask, 15);
	esp_ip4addr_ntoa(&ipaddr.u_addr.ip4, dns_server, 15);
	esp_ip4addr_ntoa(&ip_info.gw, gw, 15);

	uint8_t mac[6];
	esp_read_mac(mac, ESP_MAC_WIFI_STA);

	char mac_str[18] = {0};
	sprintf(mac_str, "%02x:%02x:%02x:%02x:%02x:%02x",
			mac[0], mac[1], mac[2],
			mac[3], mac[4], mac[5]);

	lv_label_set_text_fmt(msg_lbl,
			"mac: %s\n"
			"addr: %s\n"
			"netmask: %s\n"
			"dns: %s\n"
			"gateway: %s",
			mac_str, ip, netmask, dns_server, gw);
}

void create_wifi_sub_page(lv_event_t *e)
{
	clear_all_sub_page_child();

	lv_obj_set_style_pad_hor(sub_wifi_page, 20, 0);
	lv_obj_t *section = lv_menu_section_create(sub_wifi_page);

	wifi_page_obj = malloc(sizeof(struct Wifi_page_obj));
	wifi_page_obj->busy_ind = NULL;

	create_switch(section, LV_SYMBOL_SETTINGS, "Включить", (glob_get_status_reg() & STATUS_WIFI_STA_START), &(wifi_page_obj->wifi_switch));
	create_list(section, 495, 255, &(wifi_page_obj->list));
	create_button(section, "Сканировать", 128, 40, &(wifi_page_obj->btn_scan));

	lv_obj_t *info_btn = lv_btn_create(lv_obj_get_parent(wifi_page_obj->btn_scan));
	lv_obj_set_size(info_btn, 128, 40);
	lv_obj_align(info_btn, LV_ALIGN_LEFT_MID, 0, 0);

	lv_obj_add_event_cb(info_btn, info_handler, LV_EVENT_CLICKED, 0);

	lv_obj_t *info_btn_lbl = lv_label_create(info_btn);
	lv_label_set_text(info_btn_lbl, "Информация");
	lv_obj_center(info_btn_lbl);

	lv_obj_add_event_cb(wifi_page_obj->btn_scan, wifi_scan_starting_heandler, LV_EVENT_CLICKED, 0);
	lv_obj_add_event_cb(wifi_page_obj->wifi_switch, wifi_switch_heandler, LV_EVENT_CLICKED, 0);

	if (glob_get_status_reg() & STATUS_WIFI_STA_START)
	{
		lv_obj_clear_state(wifi_page_obj->btn_scan, LV_STATE_DISABLED);
		lv_event_send(wifi_page_obj->btn_scan, LV_EVENT_CLICKED, 0);
	}
	else
		lv_obj_add_state(wifi_page_obj->btn_scan, LV_STATE_DISABLED);

	ap_info = calloc(AP_INFO_ARR_SIZE, sizeof(wifi_ap_record_t));
	wifi_page_obj->timer = lv_timer_create(timer_loop, 500, 0);
}

static void timer_loop(lv_timer_t *timer)
{
	if (wait_flag == WAIT_FLAG_SCAN)
	{
		if (glob_get_status_reg() & STATUS_WIFI_SCAN_DONE)
		{
//			glob_status_reg &= ~STATUS_WIFI_SCAN_DONE;
			wifi_scan_done();
			wait_flag = WAIT_FLAG_NONE;
		}
	}
	else if (wait_flag == WAIT_FLAG_CONN)
	{
		if ( !(glob_get_status_reg() & STATUS_WIFI_STA_CONNECTING))
		{
			clear_busy_indicator(&wifi_page_obj->busy_ind);
			wait_flag = WAIT_FLAG_NONE;
		}
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

	wait_flag = WAIT_FLAG_NONE;
}
