#ifndef ARTEMIS_FIRMWARE_WEATHERELEMENT_H
#define ARTEMIS_FIRMWARE_WEATHERELEMENT_H

#include "LV_Interface/LVObject.h"
#include "Services/Weather.h"

class WeatherElement : public LVObject {
public:
	explicit WeatherElement(lv_obj_t* parent);

	void set(Weather::Condition condition, int8_t tempCelsius, int8_t hiCelsius, int8_t loCelsius, uint8_t rainPercent);

private:
	lv_obj_t* icon;
	lv_obj_t* rainLabel;
	lv_obj_t* hiLoLabel;
	lv_obj_t* tempLabel;

	void styleLabel(lv_obj_t* label);
};


#endif //ARTEMIS_FIRMWARE_WEATHERELEMENT_H
