///*
// * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
// *
// * SPDX-License-Identifier: CC0-1.0
// */
//
//// This demo UI is adapted from LVGL official example: https://docs.lvgl.io/master/examples.html#scatter-chart
//
#include "lvgl.h"
#include "stdio.h"


//void draw_page(void)
//{

//	time_obj = lv_obj_create(lv_scr_act());
//	lv_obj_set_size(time_obj, 200, 40);
//	lv_obj_set_pos(time_obj, 42, 12);
//	lv_obj_set_style_border_width(time_obj, 1, 0);
//	lv_obj_set_style_border_color(time_obj, lv_color_black(), 0);
//	lv_obj_set_style_radius(time_obj , 0, 0);
//	lv_obj_set_scrollbar_mode(time_obj , LV_SCROLLBAR_MODE_OFF);
//	lv_obj_align(time_obj, LV_ALIGN_BOTTOM_LEFT, 0, 0);
//
//	label_time = lv_label_create(time_obj);
//	lv_obj_align(label_time, LV_ALIGN_CENTER, 0, 0);
//
//	static lv_style_t style_time;
//	lv_style_init(&style_time);
//
//	lv_style_set_text_font(&style_time, &lv_font_montserrat_14);
//	lv_obj_add_style(label_time, &style_time, 0);
//
//    lv_obj_t * label;
//
//    lv_obj_t * btn1 = lv_btn_create(lv_scr_act());
//    lv_obj_set_size(btn1, 100, 100);
//    lv_obj_set_pos(btn1, 100, 100);
//    lv_obj_add_event_cb(btn1, event_handler, LV_EVENT_ALL, NULL);
////    lv_obj_align(btn1, LV_ALIGN_CENTER, 0, -100);
//
//    label = lv_label_create(btn1);
//    lv_label_set_text(label, "Button");
//    lv_obj_center(label);
//
//    btn2 = lv_btn_create(lv_scr_act());
//    lv_obj_set_size(btn2, 100, 100);
//    lv_obj_add_event_cb(btn2, event_handler, LV_EVENT_ALL, NULL);
//    lv_obj_align(btn2, LV_ALIGN_CENTER, 0, 100);
//    lv_obj_add_flag(btn2, LV_OBJ_FLAG_CHECKABLE);
//    lv_obj_set_height(btn2, LV_SIZE_CONTENT);
//
//    label = lv_label_create(btn2);
//    lv_label_set_text(label, "Toggle");
//    lv_obj_center(label);
//
//    lv_timer_create(debug_coord, 10, label_time);



//
//
//    lv_obj_t * led1  = lv_led_create(lv_scr_act());
//    lv_obj_align(led1, LV_ALIGN_CENTER, -80, 0);
//    lv_led_off(led1);
//}
