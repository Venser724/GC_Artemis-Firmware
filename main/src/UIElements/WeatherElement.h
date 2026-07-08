#ifndef ARTEMIS_FIRMWARE_WEATHERELEMENT_H
#define ARTEMIS_FIRMWARE_WEATHERELEMENT_H

#include "LV_Interface/LVObject.h"

class WeatherElement : public LVObject {
public:
	enum class Condition {
		Sunny, Cloudy, Rainy
	};

	explicit WeatherElement(lv_obj_t* parent);

	void set(Condition condition, int8_t tempCelsius);

private:
	lv_obj_t* icon;
	lv_obj_t* tempLabel;
};


#endif //ARTEMIS_FIRMWARE_WEATHERELEMENT_H
