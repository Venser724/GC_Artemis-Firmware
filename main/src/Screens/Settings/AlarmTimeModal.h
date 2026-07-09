#ifndef ARTEMIS_FIRMWARE_ALARMTIMEMODAL_H
#define ARTEMIS_FIRMWARE_ALARMTIMEMODAL_H

#include <functional>
#include "LV_Interface/LVModal.h"
#include "LabelElement.h"

// Hour/minute-only picker for the alarm time, saved via callback instead of TimePickerModal's
// direct write into the Time service (alarm time lives in Settings, not the system clock).
class AlarmTimeModal : public LVModal {
public:
	AlarmTimeModal(LVScreen* parent, uint8_t hour, uint8_t minute, const std::function<void(uint8_t, uint8_t)>& onSaved);

private:
	void buildStyles();
	void buildUI();
	void save();

	lv_obj_t* createPicker(lv_obj_t* parent, int32_t value, int32_t rangeMin, int32_t rangeMax);

	uint8_t hour, minute;

	lv_obj_t* hourPicker, * minutePicker, * meridiem;
	lv_obj_t* timeCont;
	LabelElement* saveButton;

	LVStyle defaultStyle;
	LVStyle focusedStyle;
	LVStyle labelStyle;

	std::function<void(uint8_t, uint8_t)> onSaved;

	const bool startingInputInversion;
	const bool timeFormat24h;

	lv_anim_t blinkAnim;
	static void animFunc(void* var, int32_t val);
	void startAnim(lv_obj_t* target);

	static constexpr lv_style_selector_t SelDefault = LV_PART_MAIN | LV_STATE_DEFAULT;
	static constexpr lv_style_selector_t SelFocus = LV_PART_MAIN | LV_STATE_FOCUSED;
	static constexpr const char* MeridiemNames = "AM\nPM";
};


#endif //ARTEMIS_FIRMWARE_ALARMTIMEMODAL_H
