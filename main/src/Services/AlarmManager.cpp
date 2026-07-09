#include "AlarmManager.h"
#include <ctime>

AlarmManager::AlarmManager(Settings& settings, Time& time, SleepMan& sleepMan, LVGL& lvgl) :
		SleepyThreaded(CheckInterval, "Alarm", StackSize, 5, -1),
		settings(settings), time(time), sleepMan(sleepMan), lvgl(lvgl){
	start();
}

void AlarmManager::sleepyLoop(){
	auto s = settings.get();
	if(!s.alarmEnabled){
		firedForMinute = -1;
		return;
	}

	tm now = time.getTime();
	if(now.tm_hour != s.alarmHour || now.tm_min != s.alarmMinute){
		firedForMinute = -1;
		return;
	}

	if(firedForMinute == now.tm_min){
		return;
	}
	firedForMinute = now.tm_min;

	if(sleepMan.isAsleep()){
		sleepMan.wakeForAlarm();
	}else{
		lvgl.requestAlarmScreen();
	}
}
