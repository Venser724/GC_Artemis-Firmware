#ifndef ARTEMIS_FIRMWARE_WEATHER_H
#define ARTEMIS_FIRMWARE_WEATHER_H

#include <cstdint>
#include <nvs.h>

class Weather {
public:
	enum class Condition {
		Sunny, Cloudy, Rainy, Snowy
	};

	struct State {
		bool valid = false;
		Condition condition = Condition::Cloudy;
		int8_t tempCelsius = 0;
		int8_t hiCelsius = 0;
		int8_t loCelsius = 0;
		uint8_t rainPercent = 0;
	};

	Weather();

	void set(Condition condition, int8_t tempCelsius, int8_t hiCelsius, int8_t loCelsius, uint8_t rainPercent);
	State get() const;

	// Maps an OpenWeatherMap condition code to our reduced set of icons - GadgetBridge's
	// Bangle.js weather message uses this same code table (see espruino/BangleApps
	// apps/weather/lib.js, chooseIconByCode) https://openweathermap.org/weather-conditions
	static Condition fromOpenWeatherMapCode(int code);

private:
	State state;
	nvs_handle_t handle{};

	// Persisted in NVS (not just kept in RAM) so the lock screen still has last-known
	// weather to show right after a reboot, instead of blank until the next GadgetBridge sync.
	void load();
	void store();

	static constexpr const char* NVSNamespace = "Artemis";
	static constexpr const char* BlobName = "Weather";
};


#endif //ARTEMIS_FIRMWARE_WEATHER_H
