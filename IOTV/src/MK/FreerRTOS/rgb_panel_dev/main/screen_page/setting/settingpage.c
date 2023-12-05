/*
 * settingpage.c
 *
 *  Created on: 1 дек. 2023 г.
 *      Author: user
 */

#include "settingpage.h"
#include "elements.h"
#include "esp_wifi.h"
#include "I2C/DS3231.h"

#include "esp_system.h"
#include "esp_netif.h"
#include "esp_eth.h"

extern void menuPageInit(void);

extern uint8_t glob_currentPage;
extern bool glob_wifi_STA_run;

extern struct DateTime glob_date_time;

static char* TAG = "settings";

static lv_obj_t *menu;
static lv_obj_t *root_page;

static lv_obj_t *sub_date_time_page;
static lv_obj_t *sub_display_page;
static lv_obj_t *sub_wifi_page;
static lv_obj_t *sub_about_page;
static lv_obj_t *sub_software_info_page;
static lv_obj_t *sub_legal_info_page;

static char *root_page_title = "Settings";
static char *date_time_page_title = "Date and time";
static char *display_page_title = "Display";
static char *wifi_page_title = "WIFI";

static void back_event_handler(lv_event_t * e);
static void create_date_time_sub_page(lv_event_t *e);
static void create_display_sub_page(lv_event_t *e);
static void create_wifi_sub_page(lv_event_t *e);

static void create_sub_pages(void);

static void create_date_time_page(lv_obj_t *parent);
static void create_display_page(lv_obj_t *parent);
static void create_wifi_page(lv_obj_t *parent);
static void create_other_pages(void);

static void back_event_handler(lv_event_t * e)
{
	lv_obj_t *obj = lv_event_get_target(e);
	lv_obj_t *_menu = lv_event_get_user_data(e);

	if(lv_menu_back_btn_is_root(_menu, obj))
	{
		((lv_menu_page_t *)root_page)->title = NULL;
		((lv_menu_page_t *)sub_date_time_page)->title = NULL;
		((lv_menu_page_t *)sub_display_page)->title = NULL;
		((lv_menu_page_t *)sub_wifi_page)->title = NULL;
		menuPageInit();
	}
}

static void save_date_time(lv_event_t *e)
{
	lv_obj_t *section = (lv_obj_t *)e->user_data;

	glob_date_time.hour = lv_spinbox_get_value(lv_obj_get_child(lv_obj_get_child(lv_obj_get_child(section, 0), 0), 1));
	glob_date_time.minutes = lv_spinbox_get_value(lv_obj_get_child(lv_obj_get_child(lv_obj_get_child(section, 1), 0), 1));
	glob_date_time.seconds = lv_spinbox_get_value(lv_obj_get_child(lv_obj_get_child(lv_obj_get_child(section, 2), 0), 0));
	//
	lv_calendar_t *calendar = (lv_calendar_t *)lv_obj_get_child(lv_obj_get_child(lv_obj_get_child(section, 3), 0), 0);
	//	printf("cal today %d\n", calendar->today.year);
	glob_date_time.year = calendar->today.year - 1900;
	glob_date_time.month = calendar->today.month;
	glob_date_time.date = calendar->today.day;

	//	DS3231_SetDataTime(&glob_date_time);
}

static void debug_lv_obj_t_tree(lv_obj_t *obj, int depth)
{
	for (int i = 0; i < lv_obj_get_child_cnt(obj); ++i)
	{
		for (int j = 0; j < depth; ++j)
			printf("\t");
		printf("depth: %d, current obj addr: %p,", depth, lv_obj_get_child(obj, i));
		printf("children: %d\n", (int)(lv_obj_get_child_cnt(lv_obj_get_child(obj, i))));
		debug_lv_obj_t_tree(lv_obj_get_child(obj, i), depth + 1);
	}
}

static void print_auth_mode(int authmode, char *buff, uint8_t size)
{
	switch (authmode)
	{
	case WIFI_AUTH_OPEN:
		snprintf(buff, size, "OPEN");
		ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_OPEN");
		break;
	case WIFI_AUTH_OWE:
		snprintf(buff, size, "OWE");
		ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_OWE");
		break;
	case WIFI_AUTH_WEP:
		snprintf(buff, size, "WEP");
		ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_WEP");
		break;
	case WIFI_AUTH_WPA_PSK:
		snprintf(buff, size, "WPA_PSK");
		ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_WPA_PSK");
		break;
	case WIFI_AUTH_WPA2_PSK:
		snprintf(buff, size, "WPA2_PSK");
		ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_WPA2_PSK");
		break;
	case WIFI_AUTH_WPA_WPA2_PSK:
		snprintf(buff, size, "WPA_WPA2_PSK");
		ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_WPA_WPA2_PSK");
		break;
	case WIFI_AUTH_WPA2_ENTERPRISE:
		snprintf(buff, size, "WPA2_ENTERPRISE");
		ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_WPA2_ENTERPRISE");
		break;
	case WIFI_AUTH_WPA3_PSK:
		snprintf(buff, size, "WPA3_PSK");
		ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_WPA3_PSK");
		break;
	case WIFI_AUTH_WPA2_WPA3_PSK:
		snprintf(buff, size, "WPA2_WPA3_PSK");
		ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_WPA2_WPA3_PSK");
		break;
	default:
		snprintf(buff, size, "AUTH_UNKNOWN");
		ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_UNKNOWN");
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

static void wifi_list_item(lv_obj_t **btn, lv_coord_t w, lv_coord_t h, const wifi_ap_record_t *ap_record)
{
	lv_obj_set_size(*btn, w, h);

	//	wifi_ap_record_t ap_info;
	//	esp_wifi_sta_get_ap_info(&ap_info);

	lv_obj_t *lbl_ssid = lv_label_create(*btn);

	//	if (memcmp(ap_info.bssid, ap_record->bssid, 6) == 0)
	lv_label_set_text_fmt(lbl_ssid, "%s %s", LV_SYMBOL_WIFI, ap_record->ssid);
	//	else
	//		lv_label_set_text_fmt(lbl_ssid, "%s", ap_record->ssid);

	lv_obj_align(lbl_ssid, LV_ALIGN_LEFT_MID, 0, 0);

	lv_obj_t *lbl_bssid = lv_label_create(*btn);
	lv_label_set_text_fmt(lbl_bssid, "(%02X:%02X:%02X:%02X:%02X:%02X)",
			ap_record->bssid[0], ap_record->bssid[1], ap_record->bssid[2],
			ap_record->bssid[3], ap_record->bssid[4], ap_record->bssid[5]);
	lv_obj_align_to(lbl_bssid, lbl_ssid, LV_ALIGN_OUT_RIGHT_MID, 5, 0);

	lv_obj_t *lbl_rssi = lv_label_create(*btn);
	lv_label_set_text_fmt(lbl_rssi, "%d", ap_record->rssi);
	lv_obj_align(lbl_rssi, LV_ALIGN_RIGHT_MID, 0, 0);

	lv_obj_t *lbl_authmode = lv_label_create(*btn);
	char buff[16] = {0};
	print_auth_mode(ap_record->authmode, buff, sizeof(buff));
	lv_label_set_text_fmt(lbl_authmode, "(%s)", buff);
	lv_obj_align_to(lbl_authmode, lbl_bssid, LV_ALIGN_OUT_RIGHT_MID, 5, 0);
}

static void ta_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * ta = lv_event_get_target(e);
    lv_obj_t * kb = lv_event_get_user_data(e);
    if(code == LV_EVENT_FOCUSED) {
        lv_keyboard_set_textarea(kb, ta);
        lv_obj_clear_flag(kb, LV_OBJ_FLAG_HIDDEN);
    }

    if(code == LV_EVENT_DEFOCUSED) {
        lv_keyboard_set_textarea(kb, NULL);
        lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
    }
}

static void close_widget_parent(lv_event_t * e)
{
	lv_obj_del(e->user_data);
}

static void wifi_connect(lv_event_t *e)
{
//    static const char * buttons[] = {"Apply", "Close", ""};

    lv_obj_t *main_widget = lv_obj_create(lv_scr_act());
    lv_obj_set_size(main_widget, 800, 480);

	lv_color_t bg_color = lv_obj_get_style_bg_color(main_widget, 0);
	if(lv_color_brightness(bg_color) > 127)
		lv_obj_set_style_bg_color(main_widget, lv_color_darken(lv_obj_get_style_bg_color(main_widget, 0), 20), 0);
	else
		lv_obj_set_style_bg_color(main_widget, lv_color_darken(lv_obj_get_style_bg_color(main_widget, 0), 50), 0);

	lv_obj_t *btn_close = lv_btn_create(main_widget);
	lv_obj_set_size(btn_close, 32, 32);
	lv_obj_align(btn_close, LV_ALIGN_TOP_RIGHT, 0, 0);
	lv_obj_add_event_cb(btn_close, close_widget_parent, LV_EVENT_CLICKED, main_widget);

	lv_obj_t *btn_close_lbl = lv_label_create(btn_close);
	lv_label_set_text(btn_close_lbl, LV_SYMBOL_CLOSE);
	lv_obj_center(btn_close_lbl);


    /*Create a keyboard to use it with an of the text areas*/
    lv_obj_t *kb = lv_keyboard_create(main_widget);
    lv_obj_add_event_cb(kb, close_widget_parent, LV_EVENT_CANCEL, main_widget);
    lv_obj_align(kb, LV_ALIGN_BOTTOM_MID, 0, 0);

    /*Create a text area. The keyboard will write here*/
    lv_obj_t * ta;
    ta = lv_textarea_create(main_widget);
    lv_obj_align(ta, LV_ALIGN_TOP_LEFT, 10, 50);
    lv_obj_add_event_cb(ta, ta_event_cb, LV_EVENT_ALL, kb);
    lv_textarea_set_placeholder_text(ta, "Hello");
    lv_obj_set_size(ta, 140, 80);

    ta = lv_textarea_create(main_widget);
    lv_obj_align(ta, LV_ALIGN_TOP_RIGHT, -10, 50);
    lv_obj_add_event_cb(ta, ta_event_cb, LV_EVENT_ALL, kb);
    lv_obj_set_size(ta, 140, 80);

    lv_keyboard_set_textarea(kb, ta);
//
//	wifi_ap_record_t *ap_info = e->user_data;
//	printf("%s\n", ap_info->ssid);
//
//	wifi_config_t wifi_config = {
//			.sta = {
////					.ssid = ap_info->ssid,
//					.password = "y3z4pfrm",
//					/* Authmode threshold resets to WPA2 as default if password matches WPA2 standards (pasword len => 8).
//					 * If you want to connect the device to deprecated WEP/WPA networks, Please set the threshold value
//					 * to WIFI_AUTH_WEP/WIFI_AUTH_WPA_PSK and set the password with length and format matching to
//					 * WIFI_AUTH_WEP/WIFI_AUTH_WPA_PSK standards.
//					 */
//					.threshold.authmode = ap_info->authmode,//WIFI_AUTH_WPA2_PSK,
//					//            .sae_pwe_h2e = ESP_WIFI_SAE_MODE,
//					//            .sae_h2e_identifier = "",
//			},
//	};
//
//	memcpy(wifi_config.sta.ssid, ap_info->ssid, sizeof(wifi_config.sta.ssid));
//
//	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
//	ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config) );
//	esp_wifi_disconnect();
//	ESP_ERROR_CHECK(esp_wifi_connect());
}

static void wifi_scan_heandler(lv_event_t *e)
{
#define SIZE 10
//	const uint8_t SIZE = 10;
	uint16_t number = SIZE;
	static wifi_ap_record_t ap_info[SIZE];
	uint16_t ap_count = 0;
	memset(ap_info, 0, sizeof(ap_info));

	//	esp_wifi_clear_ap_list();
	esp_wifi_scan_start(NULL, true);
	ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&number, ap_info));
	ESP_ERROR_CHECK(esp_wifi_scan_get_ap_num(&ap_count));
	ESP_LOGI("", "Total APs scanned = %u", ap_count);

	lv_obj_t *list = e->user_data;
	lv_obj_clean(list);

	for (int i = 0; (i < SIZE) && (i < ap_count); i++)
	{
		lv_obj_t *btn = lv_list_add_btn(list, 0, 0);
		wifi_list_item(&btn, 495, 50, &ap_info[i]);
		lv_obj_add_event_cb(btn, wifi_connect, LV_EVENT_CLICKED, &ap_info[i]);
		if (ap_info[i].authmode != WIFI_AUTH_WEP) {
			print_cipher_type(ap_info[i].pairwise_cipher, ap_info[i].group_cipher);
		}
		ESP_LOGI("", "Channel \t\t%d\n", ap_info[i].primary);
	}
#undef SIZE
}

static void wifi_switch_heandler(lv_event_t *e)
{
	lv_obj_t *obj_switch = e->target;
	lv_obj_t *section = e->user_data;

	lv_obj_t *lst = lv_obj_get_child(lv_obj_get_child(lv_obj_get_child(section, 1), 0), 0);
	lv_obj_t *btn = lv_obj_get_child(lv_obj_get_child(lv_obj_get_child(section, 2), 0), 0);

	if (lv_obj_has_state(obj_switch, LV_STATE_CHECKED))
	{
		esp_wifi_start();
		lv_obj_clear_state(btn, LV_STATE_DISABLED);
		lv_event_send(btn, LV_EVENT_CLICKED, 0);
		glob_wifi_STA_run = true;
	}
	else
	{
		esp_wifi_stop();
		for (int i = 0; i < lv_obj_get_child_cnt(lst); ++i)
			lv_obj_del(lv_obj_get_child(lst, 0));
		lv_obj_add_state(btn, LV_STATE_DISABLED);
		glob_wifi_STA_run = false;
	}
}

static void create_sub_pages(void)
{
	sub_date_time_page = lv_menu_page_create(menu, date_time_page_title);
	sub_display_page = lv_menu_page_create(menu, display_page_title);
	sub_wifi_page = lv_menu_page_create(menu, wifi_page_title);

	//	sub_software_info_page = lv_menu_page_create(menu, NULL);
	//	lv_obj_set_style_pad_hor(sub_software_info_page, 20, 0);
	//	section = lv_menu_section_create(sub_software_info_page);
	//	create_text(section, NULL, "Version 1.0", LV_MENU_ITEM_BUILDER_VARIANT_1);
	//
	//	sub_legal_info_page = lv_menu_page_create(menu, NULL);
	//	lv_obj_set_style_pad_hor(sub_legal_info_page, 20, 0);
	//	section = lv_menu_section_create(sub_legal_info_page);
	//
	//	for(uint32_t i = 0; i < 15; i++)
	//	{
	//		create_text(section, NULL,
	//				"This is a long long long long long long long long long text, if it is long enough it may scroll.",
	//				LV_MENU_ITEM_BUILDER_VARIANT_1);
	//	}
	//
	//	sub_about_page = lv_menu_page_create(menu, NULL);
	//	lv_obj_set_style_pad_hor(sub_about_page, 20, 0);
	//	lv_menu_separator_create(sub_about_page);
	//
	//	section = lv_menu_section_create(sub_about_page);
	//
	//	cont = create_text(section, LV_SYMBOL_RIGHT, "Software information", LV_MENU_ITEM_BUILDER_VARIANT_1);
	//	lv_menu_set_load_page_event(menu, cont, sub_software_info_page);
	//
	//	cont = create_text(section, LV_SYMBOL_RIGHT, "Legal information", LV_MENU_ITEM_BUILDER_VARIANT_1);
	//	lv_menu_set_load_page_event(menu, cont, sub_legal_info_page);
}

static void clear_all_sub_page_child(void)
{
	for (int i = 0; i < lv_obj_get_child_cnt(sub_date_time_page); ++i)
		lv_obj_del(lv_obj_get_child(sub_date_time_page, 0));

	for (int i = 0; i < lv_obj_get_child_cnt(sub_display_page); ++i)
		lv_obj_del(lv_obj_get_child(sub_display_page, 0));

	for (int i = 0; i < lv_obj_get_child_cnt(sub_wifi_page); ++i)
		lv_obj_del(lv_obj_get_child(sub_wifi_page, 0));


	//	for (int i = 0; i < lv_obj_get_child_cnt(sub_about_page); ++i)
	//		lv_obj_del(lv_obj_get_child(sub_about_page, 0));
	//
	//	for (int i = 0; i < lv_obj_get_child_cnt(sub_software_info_page); ++i)
	//		lv_obj_del(lv_obj_get_child(sub_software_info_page, 0));
	//
	//	for (int i = 0; i < lv_obj_get_child_cnt(sub_legal_info_page); ++i)
	//		lv_obj_del(lv_obj_get_child(sub_legal_info_page, 0));
}

static void create_date_time_sub_page(lv_event_t *e)
{
	clear_all_sub_page_child();

	lv_obj_set_style_pad_hor((lv_obj_t *)sub_date_time_page, 20, 0);
	lv_obj_t *section = lv_menu_section_create((lv_obj_t *)sub_date_time_page);
	create_spinbox(section, "Hour:\t", glob_date_time.hour, 0, 23);
	create_spinbox(section, "Minutes:", glob_date_time.minutes, 0, 59);
	create_spinbox(section, "Seconds:", glob_date_time.seconds, 0, 59);
	create_calendar(section);

	lv_obj_t *obj_btn = NULL;
	create_button(section, "Save", 70, 40, &obj_btn);
	lv_obj_add_event_cb(obj_btn, save_date_time, LV_EVENT_CLICKED, section);
}

static void create_display_sub_page(lv_event_t *e)
{
	clear_all_sub_page_child();

	lv_obj_set_style_pad_hor(sub_display_page, 20, 0);
	//    lv_obj_t *separ = lv_menu_separator_create(sub_display_page);
	lv_obj_t *section = lv_menu_section_create(sub_display_page);
	create_slider(section, LV_SYMBOL_SETTINGS, "Brightness", 0, 150, 100);
}

static void create_wifi_sub_page(lv_event_t *e)
{
	lv_obj_set_style_pad_hor(sub_wifi_page, 20, 0);
	lv_obj_t *section = lv_menu_section_create(sub_wifi_page);

	lv_obj_t *switch_obj = NULL;
	lv_obj_t *obj_list = NULL;
	lv_obj_t *obj_btn = NULL;

	create_switch(section, LV_SYMBOL_SETTINGS, "Enable", glob_wifi_STA_run, &switch_obj);
	create_list(section, 495, 280, &obj_list);
	create_button(section, "Scan", 70, 40, &obj_btn);

	if (glob_wifi_STA_run)
	{
		lv_obj_clear_state(obj_list, LV_STATE_DISABLED);
		lv_obj_clear_state(obj_btn, LV_STATE_DISABLED);
		lv_event_send(obj_btn, LV_EVENT_CLICKED, 0);
	}
	else
	{
		lv_obj_add_state(obj_list, LV_STATE_DISABLED);
		lv_obj_add_state(obj_btn, LV_STATE_DISABLED);
	}

	lv_obj_add_event_cb(obj_btn, wifi_scan_heandler, LV_EVENT_CLICKED, obj_list);
	lv_obj_add_event_cb(switch_obj, wifi_switch_heandler, LV_EVENT_CLICKED, section);
}

static void create_date_time_page(lv_obj_t *parent)
{
	lv_obj_t *cont = create_text(parent, LV_SYMBOL_LIST, "Date and Time", LV_MENU_ITEM_BUILDER_VARIANT_1);
	lv_menu_set_load_page_event(menu, cont, sub_date_time_page);
	lv_obj_add_event_cb(cont, create_date_time_sub_page, LV_EVENT_CLICKED, cont);
}

static void create_display_page(lv_obj_t *parent)
{
	lv_obj_t *cont = create_text(parent, LV_SYMBOL_SETTINGS, "Display", LV_MENU_ITEM_BUILDER_VARIANT_1);
	lv_menu_set_load_page_event(menu, cont, sub_display_page);
	lv_obj_add_event_cb(cont, create_display_sub_page, LV_EVENT_CLICKED, cont);
}

static void create_wifi_page(lv_obj_t *parent)
{
	lv_obj_t *cont = create_text(parent, LV_SYMBOL_WIFI, "WIFI", LV_MENU_ITEM_BUILDER_VARIANT_1);
	lv_menu_set_load_page_event(menu, cont, sub_wifi_page);
	lv_obj_add_event_cb(cont, create_wifi_sub_page, LV_EVENT_CLICKED, cont);
}

static void create_other_pages(void)
{
	create_text(root_page, NULL, "Others", LV_MENU_ITEM_BUILDER_VARIANT_1);
	lv_obj_t *section = lv_menu_section_create(root_page);
	lv_obj_t *cont = create_text(section, NULL, "About", LV_MENU_ITEM_BUILDER_VARIANT_1);
	lv_menu_set_load_page_event(menu, cont, sub_about_page);
}


void settingPageInit(void)
{
	lv_obj_t *scr = lv_obj_create(NULL);

	// Создание объекта Меню
	menu = lv_menu_create(scr);
	lv_obj_set_size(menu, 800, 480);

	// Фон немного "сереем"
	lv_color_t bg_color = lv_obj_get_style_bg_color(menu, 0);
	if(lv_color_brightness(bg_color) > 127)
		lv_obj_set_style_bg_color(menu, lv_color_darken(lv_obj_get_style_bg_color(menu, 0), 20), 0);
	else
		lv_obj_set_style_bg_color(menu, lv_color_darken(lv_obj_get_style_bg_color(menu, 0), 50), 0);

	// Кнопка меню BACK и связывание события нажатия на эту кнопку
	lv_menu_set_mode_root_back_btn(menu, LV_MENU_ROOT_BACK_BTN_ENABLED);
	lv_obj_add_event_cb(menu, back_event_handler, LV_EVENT_CLICKED, menu);

	create_sub_pages();

	lv_obj_t *section;

	//    /*Create a root page*/
	root_page = lv_menu_page_create(menu, root_page_title);
	lv_obj_set_style_pad_hor(root_page, 20, 0);
	section = lv_menu_section_create(root_page);

	create_date_time_page(section);
	create_display_page(section);
	create_wifi_page(section);
	create_other_pages();

	lv_menu_set_sidebar_page(menu, root_page);

	lv_scr_load_anim(scr, LV_SCR_LOAD_ANIM_NONE, 0, 0, 1);

	glob_currentPage = PAGE_SETTINGS;

	// открыть первый пункт меню
	//	lv_event_send(lv_obj_get_child(lv_obj_get_child(lv_menu_get_cur_sidebar_page(menu), 0), 0), LV_EVENT_CLICKED, NULL);

	printf("heap %u\n", heap_caps_get_free_size(0));
}

void drawSettingPage(void)
{

}

