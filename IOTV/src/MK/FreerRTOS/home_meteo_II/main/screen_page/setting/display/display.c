/*
 * display.c
 *
 *  Created on: 25 дек. 2023 г.
 *      Author: user
 */

#include "display.h"
//#include "settingpage.h"

extern lv_obj_t *sub_display_page;;
extern service_display_t display_service_data;

static lv_disp_rot_t rotate_display_tmp;

struct Display_page_obj
{
	lv_obj_t *switcher_rotate;
	lv_obj_t *switcher_night_mode;
	lv_obj_t *slider_brightness;
	lv_obj_t *slider_day;
	lv_obj_t *slider_night;
	time_block_t *time_block_day;
	time_block_t *time_block_night;
	lv_obj_t *btn_save;
};
typedef struct Display_page_obj display_page_obj_t;
static display_page_obj_t *display_page_obj = NULL;

static void display_switcher_event_handler(lv_event_t * e);
static void display_brightness_event_handler(lv_event_t * e);
static void display_night_mode_event_handler(lv_event_t * e);
static void display_save_event_handler(lv_event_t * e);

static void display_save_event_handler(lv_event_t * e)
{
	char buf[BUFSIZE] = {0};
	sprintf(buf, "%d", (int)lv_slider_get_value(display_page_obj->slider_brightness));
	set_display_config_value(BRIGHTNESS_STR, buf);

	set_display_config_value(ROTATE_STR, lv_obj_has_state(display_page_obj->switcher_rotate, LV_STATE_CHECKED) ? "1" : "0");
	if (lv_obj_has_state(display_page_obj->switcher_rotate, LV_STATE_CHECKED))
		rotate_display_tmp = LV_DISP_ROT_180;
	else
		rotate_display_tmp = LV_DISP_ROT_NONE;

	set_display_config_value(NIGHT_MODE_STR, lv_obj_has_state(display_page_obj->switcher_night_mode, LV_STATE_CHECKED) ? "1" : "0");

	memset(buf, 0, sizeof(buf));
	sprintf(buf, "%d", (int)lv_slider_get_value(display_page_obj->slider_day));
	set_display_config_value(BRIGHTNESS_DAY_STR, buf);

	memset(buf, 0, sizeof(buf));
	sprintf(buf, "%d", (int)lv_slider_get_value(display_page_obj->slider_night));
	set_display_config_value(BRIGHTNESS_NIGHT_STR, buf);

	memset(buf, 0, sizeof(buf));
	snprintf(buf, 18, "%.02d:%.02d:%.02d",
			lv_dropdown_get_selected(display_page_obj->time_block_day->h),
			lv_dropdown_get_selected(display_page_obj->time_block_day->m),
			lv_dropdown_get_selected(display_page_obj->time_block_day->s));
	set_display_config_value(DAY_BEGIN_STR, buf);

	memset(buf, 0, sizeof(buf));
	snprintf(buf, 18, "%.02d:%.02d:%.02d",
			lv_dropdown_get_selected(display_page_obj->time_block_night->h),
			lv_dropdown_get_selected(display_page_obj->time_block_night->m),
			lv_dropdown_get_selected(display_page_obj->time_block_night->s));
	set_display_config_value(NIGHT_BEGIN_STR, buf);

	service_display_read_conf();
}

static void display_night_mode_event_handler(lv_event_t * e)
{
	if (lv_obj_has_state(display_page_obj->switcher_night_mode, LV_STATE_CHECKED))
	{
		lv_obj_clear_state(display_page_obj->slider_day, LV_STATE_DISABLED);
		lv_obj_clear_state(display_page_obj->slider_night, LV_STATE_DISABLED);
		lv_obj_clear_state(display_page_obj->time_block_day->h, LV_STATE_DISABLED);
		lv_obj_clear_state(display_page_obj->time_block_day->m, LV_STATE_DISABLED);
		lv_obj_clear_state(display_page_obj->time_block_day->s, LV_STATE_DISABLED);
		lv_obj_clear_state(display_page_obj->time_block_night->h, LV_STATE_DISABLED);
		lv_obj_clear_state(display_page_obj->time_block_night->m, LV_STATE_DISABLED);
		lv_obj_clear_state(display_page_obj->time_block_night->s, LV_STATE_DISABLED);
	}
	else
	{
		lv_obj_add_state(display_page_obj->slider_day, LV_STATE_DISABLED);
		lv_obj_add_state(display_page_obj->slider_night, LV_STATE_DISABLED);
		lv_obj_add_state(display_page_obj->time_block_day->h, LV_STATE_DISABLED);
		lv_obj_add_state(display_page_obj->time_block_day->m, LV_STATE_DISABLED);
		lv_obj_add_state(display_page_obj->time_block_day->s, LV_STATE_DISABLED);
		lv_obj_add_state(display_page_obj->time_block_night->h, LV_STATE_DISABLED);
		lv_obj_add_state(display_page_obj->time_block_night->m, LV_STATE_DISABLED);
		lv_obj_add_state(display_page_obj->time_block_night->s, LV_STATE_DISABLED);
	}
}

static void display_brightness_event_handler(lv_event_t * e)
{
	uint8_t brightness = lv_slider_get_value(display_page_obj->slider_brightness);
	set_display_brightness(brightness);
	//	char buf[8] = {0};
	//	snprintf(buf, sizeof(buf) - 1, "%d", (int)brightness);
	//	set_display_config_value("brightness", buf);
	//	printf("display brightness: %d\n", (int)lv_slider_get_value(display_page_obj->slider_brightness));
}

static void display_switcher_event_handler(lv_event_t * e)
{
	lv_obj_t *switcher = e->target;
	if (lv_obj_has_state(switcher, LV_STATE_CHECKED))
	{
		rotate_display(LV_DISP_ROT_180);
		rotate_display_tmp = LV_DISP_ROT_NONE;
	}
	else
	{
		rotate_display(LV_DISP_ROT_NONE);
		rotate_display_tmp = LV_DISP_ROT_180;
	}
}

void create_display_sub_page(lv_event_t *e)
{
	clear_all_sub_page_child();

	display_page_obj = calloc(1, sizeof(display_page_obj_t));

	lv_obj_set_style_pad_hor(sub_display_page, 20, 0);
	lv_obj_t *section = lv_menu_section_create(sub_display_page);

	// Яркость
	create_slider(section, LV_SYMBOL_SETTINGS, "Яркость дисплея", 2, 255, display_service_data.brightnes, &display_page_obj->slider_brightness);
	lv_obj_add_event_cb(display_page_obj->slider_brightness, display_brightness_event_handler, LV_EVENT_VALUE_CHANGED, 0);

	// Поворот дисплея на 180
	create_switch(section, LV_SYMBOL_LOOP, "Поворот", get_rotate_display() == LV_DISP_ROT_NONE ? false : true, &display_page_obj->switcher_rotate);
	lv_obj_add_event_cb(display_page_obj->switcher_rotate, display_switcher_event_handler, LV_EVENT_CLICKED, 0);
	rotate_display_tmp = get_rotate_display();

	// Ночной режим по времени
	create_switch(section, LV_SYMBOL_EYE_OPEN, "Ночной режим", (glob_get_status_reg() & STATUS_DISPLAY_NIGHT_MODE_ON), &display_page_obj->switcher_night_mode);
	lv_obj_add_event_cb(display_page_obj->switcher_night_mode, display_night_mode_event_handler, LV_EVENT_CLICKED, 0);

	// Яркость днём
	create_slider(section, LV_SYMBOL_SETTINGS, "Яркость днём", 2, 255, display_service_data.brightness_day, &display_page_obj->slider_day);

	// Ярскость ночью
	create_slider(section, LV_SYMBOL_SETTINGS, "Яркость ночью", 2, 255, display_service_data.brightness_night, &display_page_obj->slider_night);

	// Включение ночного режима
	create_time_block(section, "Начало\nдневного режима:", 495, 40, &(display_page_obj->time_block_day));
	lv_dropdown_set_selected(display_page_obj->time_block_day->h, display_service_data.day_begin_h);
	lv_dropdown_set_selected(display_page_obj->time_block_day->m, display_service_data.day_begin_m);
	lv_dropdown_set_selected(display_page_obj->time_block_day->s, display_service_data.day_begin_s);

	// Включение дневного режима
	create_time_block(section, "Начало\nночного режима:", 495, 40, &display_page_obj->time_block_night);
	lv_dropdown_set_selected(display_page_obj->time_block_night->h, display_service_data.night_begin_h);
	lv_dropdown_set_selected(display_page_obj->time_block_night->m, display_service_data.night_begin_m);
	lv_dropdown_set_selected(display_page_obj->time_block_night->s, display_service_data.night_begin_s);

	if (!(lv_obj_has_state(display_page_obj->switcher_night_mode, LV_STATE_CHECKED)))
	{
		lv_obj_add_state(display_page_obj->slider_day, LV_STATE_DISABLED);
		lv_obj_add_state(display_page_obj->slider_night, LV_STATE_DISABLED);
		lv_obj_add_state(display_page_obj->time_block_day->h, LV_STATE_DISABLED);
		lv_obj_add_state(display_page_obj->time_block_day->m, LV_STATE_DISABLED);
		lv_obj_add_state(display_page_obj->time_block_day->s, LV_STATE_DISABLED);
		lv_obj_add_state(display_page_obj->time_block_night->h, LV_STATE_DISABLED);
		lv_obj_add_state(display_page_obj->time_block_night->m, LV_STATE_DISABLED);
		lv_obj_add_state(display_page_obj->time_block_night->s, LV_STATE_DISABLED);
	}

	// Кнопка - Сохранить
	create_button(section, SAVE_STR, 128, 40, &display_page_obj->btn_save);
	lv_obj_add_event_cb(display_page_obj->btn_save, display_save_event_handler, LV_EVENT_CLICKED, 0);
}

void free_display_sub_page(void)
{
	if (display_page_obj != NULL)
	{
		free(display_page_obj->time_block_day);
		free(display_page_obj->time_block_night);
		free(display_page_obj);
		display_page_obj = NULL;

		rotate_display(rotate_display_tmp);
	}
}
