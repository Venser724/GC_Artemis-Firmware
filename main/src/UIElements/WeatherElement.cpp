#include "WeatherElement.h"
#include "Filepaths.hpp"
#include "Theme/theme.h"
#include "Settings/Settings.h"
#include "Util/Services.h"

WeatherElement::WeatherElement(lv_obj_t* parent) : LVObject(parent){
	// one row, left to right: condition icon, rain chance, hi/lo, current temp - space-between
	// spreads them across the width automatically as children are added in that order
	lv_obj_set_size(*this, 128, 16);
	lv_obj_set_style_pad_hor(*this, 4, 0);
	lv_obj_set_flex_flow(*this, LV_FLEX_FLOW_ROW);
	lv_obj_set_flex_align(*this, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
	lv_obj_set_layout(*this, LV_LAYOUT_FLEX);
	lv_obj_set_style_bg_opa(*this, LV_OPA_0, 0);
	lv_obj_set_style_border_width(*this, 0, 0);

	icon = lv_img_create(*this);

	rainLabel = lv_label_create(*this);
	styleLabel(rainLabel);

	hiLoLabel = lv_label_create(*this);
	styleLabel(hiLoLabel);

	tempLabel = lv_label_create(*this);
	styleLabel(tempLabel);
}

void WeatherElement::styleLabel(lv_obj_t* label){
	lv_label_set_text(label, ""); // LVGL defaults new labels to "Text" - blank until real data arrives
	lv_obj_set_style_text_font(label, &devin, 0);
	// devin reserves base_line space below the glyph ink, which visually shifts text
	// up compared to the icon's full-height artwork - nudge down to match
	lv_obj_set_style_translate_y(label, 3, 0);

	Settings* settings = (Settings*) Services.get(Service::Settings);
	if(settings != nullptr){
		lv_obj_set_style_text_color(label, settings->get().themeData.dateColor, 0);
	}
}

void WeatherElement::set(Weather::Condition condition, int8_t tempCelsius, int8_t hiCelsius, int8_t loCelsius, uint8_t rainPercent){
	const char* path;
	switch(condition){
		case Weather::Condition::Sunny:
			path = File::Weather::Theme1::Sun;
			break;
		case Weather::Condition::Rainy:
			path = File::Weather::Theme1::Rain;
			break;
		case Weather::Condition::Snowy:
			path = File::Weather::Theme1::Snow;
			break;
		case Weather::Condition::Cloudy:
		default:
			path = File::Weather::Theme1::Cloud;
			break;
	}
	lv_img_set_src(icon, path);

	char buf[16];

	snprintf(buf, sizeof(buf), "%d%%", rainPercent);
	lv_label_set_text(rainLabel, buf);

	snprintf(buf, sizeof(buf), "%d/%d", hiCelsius, loCelsius);
	lv_label_set_text(hiLoLabel, buf);

	snprintf(buf, sizeof(buf), "%d\xC2\xB0" "C", tempCelsius); // \xC2\xB0 = UTF-8 degree sign, devin now includes U+00B0
	lv_label_set_text(tempLabel, buf);
}
