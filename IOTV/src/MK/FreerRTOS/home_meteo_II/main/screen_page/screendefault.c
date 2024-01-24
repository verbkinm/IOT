/*
 * screendefault.c
 *
 *  Created on: 1 дек. 2023 г.
 *      Author: user
 */

#include "screendefault.h"

lv_style_t *screenStyleDefault()
{
	static bool init = false;
	static lv_style_t style;

	if (init)
		return &style;

    lv_style_init(&style);
    lv_style_set_bg_color(&style, lv_color_black());
	lv_style_set_border_width(&style, 0);
	lv_style_set_radius(&style, 0);

	init = true;
    return &style;
}
