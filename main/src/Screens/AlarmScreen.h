#ifndef ARTEMIS_FIRMWARE_ALARMSCREEN_H
#define ARTEMIS_FIRMWARE_ALARMSCREEN_H

#include "LV_Interface/LVScreen.h"
#include "Util/Events.h"

// Fullscreen alert forced on top of whatever was showing when the alarm fires (see AlarmManager/SleepMan::wakeForAlarm).
// Repeats buzzer+LED feedback until Alt is held down, or gives up on its own after AutoStopTime.
class AlarmScreen : public LVScreen {
public:
	AlarmScreen();

private:
	void onStart() override;
	void onStop() override;
	void loop() override;

	void ring();
	void dismiss();

	lv_obj_t* bg = nullptr;
	lv_obj_t* label = nullptr;

	EventQueue queue;

	uint64_t startTime = 0;
	uint64_t lastRing = 0;

	bool altDown = false;
	uint64_t altDownTime = 0;

	static constexpr uint32_t RingInterval = 1200; // [ms]
	static constexpr uint32_t AutoStopTime = 60000; // [ms] silence itself if nobody dismisses it
	static constexpr uint32_t DismissHoldTime = 600; // [ms] hold Alt this long to silence
};


#endif //ARTEMIS_FIRMWARE_ALARMSCREEN_H
