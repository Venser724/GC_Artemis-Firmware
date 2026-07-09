#include "AlarmTimeModal.h"
#include "Theme/theme.h"
#include "LV_Interface/InputLVGL.h"
#include "Settings/Settings.h"
#include <Util/Services.h>
#include <cmath>

AlarmTimeModal::AlarmTimeModal(LVScreen* parent, uint8_t hour, uint8_t minute, const std::function<void(uint8_t, uint8_t)>& onSaved) :
		LVModal(parent), hour(hour), minute(minute), onSaved(onSaved),
		startingInputInversion(InputLVGL::getInstance()->getInvertDirections()),
		timeFormat24h(((Settings*) Services.get(Service::Settings))->get().timeFormat24h){
	buildStyles();
	buildUI();
}

void AlarmTimeModal::buildStyles(){
	auto* settings = (Settings*) Services.get(Service::Settings);
	if(settings == nullptr){
		return;
	}

	lv_style_set_border_width(defaultStyle, 1);
	lv_style_set_border_opa(defaultStyle, 0);
	lv_style_set_pad_all(defaultStyle, 1);
	lv_style_set_bg_opa(defaultStyle, 0);

	lv_style_set_border_width(focusedStyle, 1);
	lv_style_set_border_color(focusedStyle, settings->get().themeData.primaryColor);
	lv_style_set_border_opa(focusedStyle, LV_OPA_COVER);

	lv_style_set_text_font(labelStyle, &devin);
	lv_style_set_text_color(labelStyle, settings->get().themeData.highlightColor);
	lv_style_set_text_opa(labelStyle, LV_OPA_COVER);
	lv_style_set_opa(labelStyle, LV_OPA_COVER);
	lv_style_set_text_align(labelStyle, LV_TEXT_ALIGN_CENTER);
}

void AlarmTimeModal::buildUI(){
	auto* settings = (Settings*) Services.get(Service::Settings);
	if(settings == nullptr){
		return;
	}

	lv_obj_set_layout(*this, LV_LAYOUT_FLEX);
	lv_obj_set_flex_align(*this, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
	lv_obj_set_flex_flow(*this, LV_FLEX_FLOW_COLUMN);

	timeCont = lv_obj_create(*this);
	lv_obj_set_layout(timeCont, LV_LAYOUT_FLEX);
	lv_obj_set_flex_align(timeCont, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
	lv_obj_set_flex_flow(timeCont, LV_FLEX_FLOW_ROW);

	if(timeFormat24h){
		hourPicker = createPicker(timeCont, hour, 0, 23);
	}else{
		int hh = hour % 12;
		if(hh == 0){
			hh = 12;
		}
		hourPicker = createPicker(timeCont, hh, 1, 12);
	}

	lv_obj_t* colon = lv_label_create(timeCont);
	lv_label_set_text(colon, ":");
	lv_obj_add_style(colon, labelStyle, 0);

	minutePicker = createPicker(timeCont, minute, 0, 59);

	if(!timeFormat24h){
		meridiem = lv_roller_create(timeCont);
		lv_roller_set_options(meridiem, MeridiemNames, LV_ROLLER_MODE_NORMAL);
		lv_roller_set_visible_row_count(meridiem, 1);
		const uint8_t ampm = hour >= 12;
		lv_roller_set_selected(meridiem, ampm, LV_ANIM_OFF);

		lv_obj_add_style(meridiem, defaultStyle, LV_PART_MAIN | LV_STATE_DEFAULT);
		lv_obj_add_style(meridiem, focusedStyle, LV_PART_MAIN | LV_STATE_FOCUSED);
		lv_obj_add_style(meridiem, labelStyle, LV_PART_MAIN);
		lv_obj_set_style_pad_hor(meridiem, 1, LV_PART_SELECTED);
		lv_obj_set_style_bg_color(meridiem, settings->get().themeData.backgroundColor, LV_PART_SELECTED | LV_STATE_DEFAULT);
		lv_obj_set_style_bg_opa(meridiem, LV_OPA_COVER, LV_PART_SELECTED | LV_STATE_DEFAULT);
		lv_obj_set_style_bg_color(meridiem, settings->get().themeData.backgroundColor, LV_PART_MAIN | LV_STATE_DEFAULT);
		lv_obj_set_style_bg_opa(meridiem, LV_OPA_COVER, LV_PART_MAIN | LV_STATE_DEFAULT);
		lv_obj_set_style_radius(meridiem, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
		lv_obj_set_style_anim_time(meridiem, 250, LV_PART_MAIN);
		lv_obj_set_style_text_line_space(meridiem, 2, LV_PART_MAIN);
		lv_group_add_obj(inputGroup, meridiem);

		lv_obj_add_event_cb(meridiem, [](lv_event_t* e){
			if(lv_event_get_key(e) != LV_KEY_ENTER) return;

			lv_anim_del(e->target, nullptr);
		}, LV_EVENT_KEY, this);

		lv_obj_add_event_cb(meridiem, [](lv_event_t* e){
			auto modal = (AlarmTimeModal*) e->user_data;

			if(lv_group_get_editing(modal->inputGroup)){
				modal->startAnim(e->target);
			}
		}, LV_EVENT_FOCUSED, this);
	}

	lv_obj_set_size(timeCont, lv_pct(108), LV_SIZE_CONTENT);

	saveButton = new LabelElement(*this, "Save", [this](){
		save();

		if(onSaved){
			onSaved(this->hour, this->minute);
		}
	}, true, LV_ALIGN_CENTER);

	lv_obj_set_width(*saveButton, 32);
	lv_obj_set_align(*saveButton, LV_ALIGN_CENTER);

	lv_group_add_obj(inputGroup, *saveButton);
}

void AlarmTimeModal::save(){
	if(timeFormat24h){
		hour = lv_spinbox_get_value(hourPicker);
	}else{
		const bool ampm = lv_roller_get_selected(meridiem);
		uint8_t hh = lv_spinbox_get_value(hourPicker);
		hour = (hh % 12) + ampm * 12;
	}
	minute = lv_spinbox_get_value(minutePicker);
}

lv_obj_t* AlarmTimeModal::createPicker(lv_obj_t* parent, int32_t value, int32_t rangeMin, int32_t rangeMax){
	const auto maxDigits = (uint8_t) (std::log10(rangeMax) + 1);

	lv_obj_t* picker = lv_spinbox_create(parent);
	lv_spinbox_set_value(picker, value);
	lv_spinbox_set_range(picker, rangeMin, rangeMax);
	lv_spinbox_set_rollover(picker, true);
	lv_spinbox_set_digit_format(picker, maxDigits, 0);
	lv_obj_add_style(picker, focusedStyle, SelFocus);
	lv_obj_add_style(picker, defaultStyle, SelDefault);
	lv_obj_add_style(picker, labelStyle, LV_PART_MAIN);
	auto sb = (lv_spinbox_t*) picker;
	lv_obj_add_style(sb->ta.label, labelStyle, 0);

	lv_obj_add_event_cb(picker, [](lv_event_t* e){
		if(lv_event_get_key(e) != LV_KEY_ENTER) return;

		auto modal = (AlarmTimeModal*) e->user_data;
		lv_group_set_editing(modal->inputGroup, false);
		InputLVGL::getInstance()->invertDirections(modal->startingInputInversion);
		lv_anim_del(e->target, nullptr);
	}, LV_EVENT_KEY, this);

	lv_obj_add_event_cb(picker, [](lv_event_t* e){
		lv_spinbox_set_cursor_pos(e->target, 0);
		auto modal = (AlarmTimeModal*) e->user_data;

		if(lv_group_get_editing(modal->inputGroup)){
			InputLVGL::getInstance()->invertDirections(!modal->startingInputInversion);
			modal->startAnim(e->target);
		}
	}, LV_EVENT_FOCUSED, this);

	lv_group_add_obj(inputGroup, picker);
	const auto width = (lv_coord_t) (lv_font_get_glyph_width(lv_obj_get_style_text_font(picker, LV_PART_MAIN), '0', '0') * (maxDigits + 1) + 2);
	lv_obj_set_width(picker, width);

	return picker;
}

void AlarmTimeModal::animFunc(void* var, int32_t val){
	lv_obj_set_style_border_opa((lv_obj_t*) var, val, LV_STATE_EDITED);
	lv_obj_invalidate((lv_obj_t*) var);
}

void AlarmTimeModal::startAnim(lv_obj_t* target){
	lv_anim_init(&blinkAnim);
	lv_anim_set_exec_cb(&blinkAnim, animFunc);
	lv_anim_set_values(&blinkAnim, LV_OPA_0, LV_OPA_100);
	lv_anim_set_time(&blinkAnim, 350);
	lv_anim_set_playback_time(&blinkAnim, 350);
	lv_anim_set_path_cb(&blinkAnim, lv_anim_path_step);
	lv_anim_set_repeat_count(&blinkAnim, LV_ANIM_REPEAT_INFINITE);
	lv_anim_set_var(&blinkAnim, target);
	lv_anim_start(&blinkAnim);
}
