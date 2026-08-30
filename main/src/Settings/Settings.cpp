#include "Settings.h"
#include <nvs_flash.h>
#include <esp_log.h>
#include <cstring>

static const char* TAG = "Settings";

// NVS gives us back raw bytes. A field that used to live at a different offset/type in an older
// build's SettingsStruct layout (e.g. before the alarm fields were inserted) can leave a bool
// holding something other than 0/1 - the compiler is free to assume a bool is always canonical,
// so comparisons/negation on an un-sanitized value can silently misbehave. Normalize through an
// actual integer read instead of trusting the bool's own operators.
static void sanitizeBool(bool& field){
	uint8_t raw;
	memcpy(&raw, &field, sizeof(bool));
	field = (raw != 0);
}

Settings::Settings(){
	auto err = nvs_open(NVSNamespace, NVS_READWRITE, &handle);
	ESP_ERROR_CHECK(err);
	load();
}

SettingsStruct Settings::get(){
	return settingsStruct;
}

void Settings::set(SettingsStruct& settings){
	settingsStruct = settings;
}

void Settings::store(){
	esp_err_t err = nvs_set_blob(handle, BlobName, &settingsStruct, sizeof(SettingsStruct) - sizeof(ThemeStruct) + sizeof(Theme));

	if(err != ESP_OK){
		ESP_LOGW(TAG, "NVS settings store error: %d", err);
		return;
	}

	err = nvs_commit(handle);
	if(err != ESP_OK){
		ESP_LOGW(TAG, "NVS settings commit error: %d", err);
	}
}

void Settings::load(){
	size_t out_size = sizeof(SettingsStruct) - sizeof(ThemeStruct) + sizeof(Theme);

	auto err = nvs_get_blob(handle, BlobName, &settingsStruct, &out_size);

	if(err != ESP_OK){
		ESP_LOGI(TAG, "Settings not found, writing defaults");
		store();
		err = nvs_get_blob(handle, BlobName, &settingsStruct, &out_size);
		if(err != ESP_OK){
			ESP_LOGE(TAG, "Couldn't access NVS settings: %d", err);
		}
	}

	sanitizeBool(settingsStruct.notificationSounds);
	sanitizeBool(settingsStruct.ledEnable);
	sanitizeBool(settingsStruct.motionDetection);
	sanitizeBool(settingsStruct.screenRotate);
	sanitizeBool(settingsStruct.timeFormat24h);
	sanitizeBool(settingsStruct.alarmEnabled);

	settingsStruct.themeData = createTheme(settingsStruct.themeData.theme);
}
