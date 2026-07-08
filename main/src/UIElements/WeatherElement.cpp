#include "WeatherElement.h"
#include "Filepaths.hpp"
#include "Theme/theme.h"
#include "Settings/Settings.h"
#include "Util/Services.h"

WeatherElement::WeatherElement(lv_obj_t* parent) : LVObject(parent){
	// spans the same row as NotifIconsElement: icon pinned left, temp pinned right,
	// notif icons stay centered in between - see LockSkin::buildUI
	lv_obj_set_size(*this, 128, 16);
	lv_obj_set_style_pad_hor(*this, 4, 0);
	lv_obj_set_flex_flow(*this, LV_FLEX_FLOW_ROW);
	lv_obj_set_flex_align(*this, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
	lv_obj_set_layout(*this, LV_LAYOUT_FLEX);
	lv_obj_set_style_bg_opa(*this, LV_OPA_0, 0);
	lv_obj_set_style_border_width(*this, 0, 0);

	icon = lv_img_create(*this);

	tempLabel = lv_label_create(*this);
	lv_obj_set_style_text_font(tempLabel, &devin, 0);
	// devin reserves base_line space below the glyph ink, which visually shifts text
	// up compared to the icon's full-height artwork - nudge down to match
	lv_obj_set_style_translate_y(tempLabel, 3, 0);

	Settings* settings = (Settings*) Services.get(Service::Settings);
	if(settings != nullptr){
		lv_obj_set_style_text_color(tempLabel, settings->get().themeData.dateColor, 0);
	}
}

void WeatherElement::set(Condition condition, int8_t tempCelsius){
	const char* path;
	switch(condition){
		case Condition::Sunny:
			path = File::Weather::Theme1::Sun;
			break;
		case Condition::Rainy:
			path = File::Weather::Theme1::Rain;
			break;
		case Condition::Snowy:
			path = File::Weather::Theme1::Snow;
			break;
		case Condition::Cloudy:
		default:
			path = File::Weather::Theme1::Cloud;
			break;
	}
	lv_img_set_src(icon, path);

	char buf[10];
	snprintf(buf, sizeof(buf), "%d\xC2\xB0" "C", tempCelsius); // \xC2\xB0 = UTF-8 degree sign, devin now includes U+00B0
	lv_label_set_text(tempLabel, buf);
}
