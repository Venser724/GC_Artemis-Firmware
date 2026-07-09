#ifndef CLOCKSTAR_FIRMWARE_SETTINGS_H
#define CLOCKSTAR_FIRMWARE_SETTINGS_H

#include <nvs.h>
#include "core/lv_theme.h"
#include "Screens/Lock/Slider.h"

enum class Theme : uint8_t {
	Theme1 [[maybe_unused]],
	Default [[maybe_unused]] = Theme1
};

struct ThemeStruct {
	Theme theme = Theme::Default;
	lv_color_t primaryColor = lv_color_black();
	uint8_t opacity = LV_OPA_100;
	lv_color_t secondaryColor = lv_color_black();
	lv_color_t highlightColor = lv_color_black();
	lv_color_t backgroundColor = lv_color_black();
	lv_color_t dateColor = lv_color_white();
	SliderConfig sliderConfig = {};
	lv_coord_t sliderY = 0;
	lv_coord_t clockX = 0;
	lv_coord_t clockY = 0;
	lv_coord_t batteryX = 0;
	lv_coord_t batteryY = 0;
	lv_coord_t phoneX = 0;
	lv_coord_t phoneY = 0;
	lv_coord_t dateX = 0;
	lv_coord_t dateY = 0;
	lv_coord_t weatherX = 0;
	lv_coord_t weatherY = 0;
	bool verticalClock = false;
	int16_t clockPadding = 0;
	bool specialPhone = false;

	struct {
		lv_coord_t x = 0;
		lv_coord_t y = 0;
		lv_coord_t w = 128;
		lv_coord_t h = 11;
		lv_align_t align = LV_ALIGN_CENTER;
		lv_coord_t gapPad = 2;
		lv_flex_flow_t flex = LV_FLEX_FLOW_ROW;
		lv_flex_align_t mainAlign = LV_FLEX_ALIGN_CENTER;
		lv_flex_align_t crossAlign = LV_FLEX_ALIGN_CENTER;
		lv_flex_align_t trackAlign = LV_FLEX_ALIGN_CENTER;
		size_t maxNotifs = 20;
	} notifData;
};

inline static ThemeStruct createTheme(Theme theme){
	ThemeStruct data = {};
	data.theme = theme;

	switch(theme){
		case Theme::Theme1:{
			data.primaryColor = lv_color_make(255, 101, 0);
			data.secondaryColor = lv_color_make(0, 0, 0);
			data.highlightColor = lv_color_make(255, 101, 0);
			data.dateColor = lv_color_make(255, 101, 0);
			data.opacity = LV_OPA_100;
			data.sliderConfig.start = 6;
			data.sliderConfig.end = 122;
			data.sliderConfig.y = 3;
			data.sliderY = 111;
			data.clockY = -14;
			data.batteryX = 4;
			data.batteryY = 4;
			data.phoneX = 56;
			data.phoneY = -57;
			data.notifData.y = -57; // top row, alongside battery/phone indicators
			data.notifData.gapPad = 3;
			data.dateY = 14;
			data.weatherY = 32; // shares the old notif icons row, below the date
			data.notifData.maxNotifs = 10;

			break;
		}
		default:{
			break;
		}
	}

	return data;
}
const static inline ThemeStruct Default = createTheme(Theme::Theme1);

enum class DateFormat : uint8_t {
	Regular,
	Reverse
};

struct SettingsStruct {
	bool notificationSounds = true;
	uint8_t screenBrightness = 100;
	uint8_t sleepTime = 1;
	bool ledEnable = true;
	bool motionDetection = true;
	bool screenRotate = false;
	bool timeFormat24h = true;
	DateFormat dateFormat = DateFormat::Regular;
	ThemeStruct themeData = Default;
};

class Settings {
public:
	Settings();

	SettingsStruct get();
	void set(SettingsStruct& settings);
	void store();

	static constexpr uint8_t SleepSteps = 5;
	static constexpr const uint32_t SleepSeconds[SleepSteps] = { 0, 30, 60, 2 * 60, 5 * 60 };
	static constexpr const char* SleepText[SleepSteps] = { "OFF", "30 sec", "1 min", "2 min", "5 min" };

private:
	nvs_handle_t handle{};
	SettingsStruct settingsStruct;

	static constexpr const char* NVSNamespace = "Artemis";
	static constexpr const char* BlobName = "Settings";

	void load();
};


#endif //CLOCKSTAR_FIRMWARE_SETTINGS_H
