/*
 * screendefault.c
 *
 *  Created on: 1 дек. 2023 г.
 *      Author: user
 */

#include "screendefault.h"

lv_style_t *screenStyleDefault()
{
    static lv_style_t screen_style;
    lv_style_init(&screen_style);
    lv_style_set_bg_color(&screen_style, lv_color_black());
    return &screen_style;
}
