#ifndef ARTEMIS_FIRMWARE_ALARMMANAGER_H
#define ARTEMIS_FIRMWARE_ALARMMANAGER_H

#include "Util/Threaded.h"
#include "Settings/Settings.h"
#include "Time.h"
#include "SleepMan.h"
#include "LV_Interface/LVGL.h"

// Fires the on-watch AlarmScreen when the clock matches the alarm time saved in Settings.
class AlarmManager : public SleepyThreaded {
public:
	AlarmManager(Settings& settings, Time& time, SleepMan& sleepMan, LVGL& lvgl);

private:
	Settings& settings;
	Time& time;
	SleepMan& sleepMan;
	LVGL& lvgl;

	void sleepyLoop() override;

	// -1 = hasn't fired for the current matching minute yet; guards against re-firing every check within that minute
	int8_t firedForMinute = -1;

	// alarms only have minute resolution, so there's nothing to gain from checking more often
	static constexpr uint32_t CheckInterval = 60000; // [ms]

	// Time gets away with a 2KB stack only because its ESP_LOGI calls are filtered out at the
	// project's default WARN level and never actually format anything (see project CLAUDE.md).
	// This task does log at WARN, so its formatting genuinely runs - give it real headroom.
	static constexpr size_t StackSize = 4 * 1024;
};


#endif //ARTEMIS_FIRMWARE_ALARMMANAGER_H
