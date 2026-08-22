#ifndef CLOCKSTAR_FIRMWARE_THEME_H
#define CLOCKSTAR_FIRMWARE_THEME_H

#include <lvgl.h>

LV_FONT_DECLARE(clockfont);
LV_FONT_DECLARE(devin);
LV_FONT_DECLARE(devin2);
LV_FONT_DECLARE(landerfont);
LV_FONT_DECLARE(batteryFont);
LV_FONT_DECLARE(mediaIcons);

lv_theme_t* theme_init(lv_disp_t* disp);

#endif //CLOCKSTAR_FIRMWARE_THEME_H
