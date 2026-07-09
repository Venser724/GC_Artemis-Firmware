#include "Weather.h"

void Weather::set(Condition condition, int8_t tempCelsius, int8_t hiCelsius, int8_t loCelsius, uint8_t rainPercent){
	state = { true, condition, tempCelsius, hiCelsius, loCelsius, rainPercent };
}

Weather::State Weather::get() const{
	return state;
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
