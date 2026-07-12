#include "Weather.h"
#include <esp_log.h>

static const char* TAG = "Weather";

Weather::Weather(){
	auto err = nvs_open(NVSNamespace, NVS_READWRITE, &handle);
	if(err != ESP_OK){
		ESP_LOGW(TAG, "NVS open error: %d", err);
		return; // non-fatal: weather cache is best-effort, don't block boot over an NVS glitch
	}

	load();
}

void Weather::set(Condition condition, int8_t tempCelsius, int8_t hiCelsius, int8_t loCelsius, uint8_t rainPercent){
	state = { true, condition, tempCelsius, hiCelsius, loCelsius, rainPercent };
	store();
}

Weather::State Weather::get() const{
	return state;
}

void Weather::load(){
	size_t size = sizeof(State);
	auto err = nvs_get_blob(handle, BlobName, &state, &size);
	if(err != ESP_OK){
		state = State{}; // nothing persisted yet - stays blank until the phone syncs
	}
}

void Weather::store(){
	auto err = nvs_set_blob(handle, BlobName, &state, sizeof(State));
	if(err != ESP_OK){
		ESP_LOGW(TAG, "NVS store error: %d", err);
		return;
	}

	err = nvs_commit(handle);
	if(err != ESP_OK){
		ESP_LOGW(TAG, "NVS commit error: %d", err);
	}
}

Weather::Condition Weather::fromOpenWeatherMapCode(int code){
	int group = code / 100;
	switch(group){
		case 2: // thunderstorm
		case 3: // drizzle
			return Condition::Rainy;
		case 5: // rain (511 = freezing rain)
			return code == 511 ? Condition::Snowy : Condition::Rainy;
		case 6: // snow
			return Condition::Snowy;
		case 7: // atmosphere: mist/fog/haze/dust/sand/ash/squall/tornado
			return Condition::Cloudy;
		case 8: // clear/clouds
			return code == 800 ? Condition::Sunny : Condition::Cloudy;
		default:
			return Condition::Cloudy;
	}
}
