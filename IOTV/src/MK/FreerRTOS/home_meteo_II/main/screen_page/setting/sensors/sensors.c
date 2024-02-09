/*
 * sensors.c
 *
 *  Created on: 9 февр. 2024 г.
 *      Author: user
 */

#include "sensors.h"

extern lv_font_t ubuntu_mono_14;
extern lv_obj_t *sub_sensors_page;

static char *txt = "Калибровка датчика температуры, влажности и давления:";

static int8_t tmp_temperature_calib_per;
static int8_t tmp_humidity_calib_per;
static int8_t tmp_pressure_calib_per;

static lv_obj_t *spinbox_temp;
static lv_obj_t *spinbox_hum;
static lv_obj_t *spinbox_press;

static lv_obj_t *lbl_cur_temp;
static lv_obj_t *lbl_cur_hum;
static lv_obj_t *lbl_cur_press;

static lv_timer_t *timer = NULL;

static void save(lv_event_t *e);
static void reset(lv_event_t *e);
static void change_calib_val(lv_event_t *e);
static void timer_loop(lv_timer_t *timer);

static void timer_loop(lv_timer_t *timer)
{
	struct THP thp = BME280_readValues_without_calibration();

	if (thp.err != true)
	{
		thp.temperature += thp.temperature / 100 * tmp_temperature_calib_per;
		thp.humidity += thp.humidity / 100 * tmp_humidity_calib_per;
		thp.pressure += thp.pressure / 100 * tmp_pressure_calib_per;
	}

	lv_label_set_text_fmt(lbl_cur_temp, "%+.2f°C", thp.temperature);
	lv_label_set_text_fmt(lbl_cur_hum, "%.2f %%", thp.humidity);
	lv_label_set_text_fmt(lbl_cur_press, "%.0f мм рт.ст.", thp.pressure);
}

static void save(lv_event_t *e)
{
	BME280_set_calib_temperature(tmp_temperature_calib_per);
	BME280_set_calib_humidity(tmp_humidity_calib_per);
	BME280_set_calib_pressure(tmp_pressure_calib_per);

	BME280_service_save_calibrations();
}

static void reset(lv_event_t *e)
{
	tmp_temperature_calib_per = 0;
	tmp_humidity_calib_per = 0;
	tmp_pressure_calib_per = 0;

	lv_spinbox_set_value(spinbox_temp, 0);
	lv_spinbox_set_value(spinbox_hum, 0);
	lv_spinbox_set_value(spinbox_press, 0);

	lv_timer_ready(timer);
}

static void change_calib_val(lv_event_t *e)
{
	tmp_temperature_calib_per = lv_spinbox_get_value(spinbox_temp);
	tmp_humidity_calib_per = lv_spinbox_get_value(spinbox_hum);
	tmp_pressure_calib_per = lv_spinbox_get_value(spinbox_press);

	lv_timer_ready(timer);
}

void create_sensors_sub_page(lv_event_t *e)
{
	tmp_temperature_calib_per = BME280_get_calib_temperature();
	tmp_humidity_calib_per = BME280_get_calib_humidity();
	tmp_pressure_calib_per = BME280_get_calib_pressure();

	clear_all_sub_page_child();
	lv_obj_set_style_pad_hor(sub_sensors_page, 20, 0);

	lv_obj_t *section = lv_menu_section_create(sub_sensors_page);

	create_text(section, NULL, txt, LV_MENU_ITEM_BUILDER_VAR_1);

	lv_obj_t *btn_minus, *btn_plus;

	lv_obj_t *cont = create_spinbox(section, "Температура(%):", tmp_temperature_calib_per, -50, 50, &spinbox_temp, &btn_minus, &btn_plus);

	lv_obj_add_event_cb(btn_minus, change_calib_val, LV_EVENT_CLICKED, section);
	lv_obj_add_event_cb(btn_plus, change_calib_val, LV_EVENT_CLICKED, section);

	lbl_cur_temp = lv_label_create(lv_obj_get_child(cont, 0));
	lv_obj_align_to(lbl_cur_temp, spinbox_temp, LV_ALIGN_OUT_RIGHT_MID, 110, 0);


	cont = create_spinbox(section, "Влажность(%):", tmp_humidity_calib_per, -50, 50, &spinbox_hum, &btn_minus, &btn_plus);

	lv_obj_add_event_cb(btn_minus, change_calib_val, LV_EVENT_CLICKED, section);
	lv_obj_add_event_cb(btn_plus, change_calib_val, LV_EVENT_CLICKED, section);

	lbl_cur_hum = lv_label_create(lv_obj_get_child(cont, 0));
	lv_obj_align_to(lbl_cur_hum, spinbox_hum, LV_ALIGN_OUT_RIGHT_MID, 110, 0);


	cont = create_spinbox(section, "Давление(%):", tmp_pressure_calib_per, -50, 50, &spinbox_press, &btn_minus, &btn_plus);

	lv_obj_add_event_cb(btn_minus, change_calib_val, LV_EVENT_CLICKED, section);
	lv_obj_add_event_cb(btn_plus, change_calib_val, LV_EVENT_CLICKED, section);

	lbl_cur_press = lv_label_create(lv_obj_get_child(cont, 0));
	lv_obj_align_to(lbl_cur_press, spinbox_press, LV_ALIGN_OUT_RIGHT_MID, 95, 0);


	lv_obj_t *btn_save = NULL;
	create_button(section, SAVE_STR, 128, 40, &btn_save);
	lv_obj_add_event_cb(btn_save, save, LV_EVENT_CLICKED, section);

	lv_obj_t *btn_reset = create_button_simply(lv_obj_get_parent(btn_save), RESET_STR, 128, 40, &ubuntu_mono_14);
	lv_obj_align_to(btn_reset, btn_save, LV_ALIGN_OUT_LEFT_MID, -5, 0);
	lv_obj_add_event_cb(btn_reset, reset, LV_EVENT_CLICKED, section);

	timer = lv_timer_create(timer_loop, 1000, 0);
	lv_timer_ready(timer);
}

void free_sensors_sub_page(void)
{
	if (timer != NULL)
	{
		lv_timer_del(timer);
		timer = NULL;
	}
}
