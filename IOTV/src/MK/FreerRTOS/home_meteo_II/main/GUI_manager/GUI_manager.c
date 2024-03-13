/*
 * GUI_manager.c
 *
 *  Created on: 29 янв. 2024 г.
 *      Author: user
 */

#include "GUI_manager.h"

static const char *TAG = "GUI_MANAGER";
static lv_obj_t *singel_msgbox;

// Обработка сообщений из вне LVGL
static void timer_handler(lv_timer_t *timer);
static void timer_startscreen_end_handler(lv_timer_t *timer);

static void update_handler(lv_event_t * e);
static void msgbox_close_handler(lv_event_t * e);

static void update_handler(lv_event_t * e)
{
	lv_obj_t *dialog_box = lv_event_get_current_target(e);

	if (strcmp(lv_msgbox_get_active_btn_text(dialog_box), YES_STR) == 0)
	{
		ESP_LOGE(TAG, YES_STR);
		if ( !(glob_get_status_reg() & STATUS_IP_GOT) )
		{
			create_msgbox_not_connected();
			return;
		}

		glob_set_bits_update_reg(UPDATE_NOW);
		full_screen_page_init("Обновление...", UPDATE_IMG);
	}
	else
		lv_msgbox_close(dialog_box);
}

static void timer_startscreen_end_handler(lv_timer_t *timer)
{
	lv_timer_del(timer);
	lv_obj_del(lv_obj_get_child(lv_scr_act(), 0));

	lv_obj_set_style_pad_all(lv_scr_act(), 0, 0);

	// Верхняя панель
	status_panel_init();

	// Основной виджет
	lv_obj_t *main_widget = lv_obj_create(lv_scr_act());
	lv_obj_set_size(main_widget, LCD_H_RES, LCD_V_RES - LCD_PANEL_STATUS_H);
	lv_obj_set_y(main_widget, LCD_PANEL_STATUS_H);
	lv_obj_set_scroll_dir(main_widget, LV_DIR_NONE);
	lv_obj_set_style_pad_all(main_widget, 0, 0);
	lv_obj_add_style(main_widget, screenStyleDefault(), 0);

	page_t *page = current_page();
	page->title = NULL;
	page->widget = main_widget;
	page->deinit = default_page_deinit;

//	start_services();
	homePageInit();

	lv_timer_t *gui_manager_timer = lv_timer_create(timer_handler, 1000, 0);
	lv_timer_ready(gui_manager_timer);
}

static void timer_handler(lv_timer_t *timer)
{
	if (glob_get_status_err() & STATUS_SD_ERROR)
	{
		lv_timer_del(timer);
		LV_IMG_DECLARE(sd);
		full_screen_page_init("Ошибка SD карты!", (void *)&sd);
		return;
	}

	if (glob_get_update_reg() & UPDATE_NOW)
		return;

	if (glob_get_status_err() & STATUS_UPDATE_ERROR)
	{
		lv_timer_del(timer);
		full_screen_page_init("Ошибка обновления!", UPDATE_FAIL);
		return;
	}

	if (glob_get_update_reg() & UPDATE_DONE)
	{
		lv_timer_del(timer);
		full_screen_page_init("Перезагрузите устройство...", UPDATE_OK);
		return;
	}

	if (glob_get_update_reg() & UPDATE_MESSAGE)
	{
		glob_clear_bits_update_reg(UPDATE_MESSAGE);
		if (singel_msgbox == NULL)
		{
			singel_msgbox = create_dialog_box(NULL, ATTENTION_STR, "Доступно новое обновление. Произвести установку?", btns_yes_no_matrix(), NULL);
			lv_obj_add_event_cb(singel_msgbox, update_handler, LV_EVENT_VALUE_CHANGED, NULL);
			lv_obj_add_event_cb(singel_msgbox, msgbox_close_handler, LV_EVENT_DELETE, NULL);
		}
	}

	status_panel_update();
}

static void msgbox_close_handler(lv_event_t * e)
{
	singel_msgbox = NULL;
}

void GUI_manager_init(void)
{
	startscreen_init();
	lv_timer_create(timer_startscreen_end_handler, 8000, 0);
}
