#include "Threaded.h"
#include "stdafx.h"
#include <esp_log.h>

Threaded::Threaded(const char* name, size_t stackSize, uint8_t priority, int8_t core) : name(name), stackSize(stackSize), priority(priority), core(core){
	stopSem = xSemaphoreCreateBinary();
	stopMut = xSemaphoreCreateMutex();
}

Threaded::~Threaded(){
	if(state != Stopped){
		ESP_LOGE("Threaded", "Threaded %s destructing while still running", name);
		abort();
	}

	vSemaphoreDelete(stopSem);
	vSemaphoreDelete(stopMut);
}

void Threaded::start(){
	if(state != Stopped) return;

	if(!onStart()) return;

	state = Running;

	if(core == -1){
		xTaskCreate(Threaded::threadFunc, name, stackSize, this, priority, &task);
	}else{
		xTaskCreatePinnedToCore(Threaded::threadFunc, name, stackSize, this, priority, &task, core);
	}
}

void Threaded::stop(TickType_t wait){
	if(xSemaphoreTake(stopMut, wait) == pdFALSE) return;

	if(state != Running){
		xSemaphoreGive(stopMut);
		return;
	}

	beforeStop();
	state = Stopping;
	afterStopSignal();

	xSemaphoreTake(stopSem, wait);

	xSemaphoreGive(stopMut);
}

void Threaded::threadFunc(void* arg){
	auto thr = static_cast<Threaded*>(arg);

	while(thr->state == Running){
		thr->loop();
	}

	thr->onStop();

	thr->state = Stopped;
	xSemaphoreGive(thr->stopSem);

	vTaskDelete(nullptr);
}

bool Threaded::onStart(){
	return true;
}

void Threaded::onStop(){ }

void Threaded::beforeStop(){ }

void Threaded::afterStopSignal(){ }

bool Threaded::running(){
	return state == Running || state == Stopping;
}

ThreadedClosure::ThreadedClosure(Lambda loopFn, const char* name, size_t stackSize, uint8_t priority, int8_t core) : Threaded(name, stackSize, priority, core), fn(std::move(loopFn)){}

void ThreadedClosure::loop(){
	fn();
}

SleepyThreaded::SleepyThreaded(TickType_t loopInterval, const char* name, size_t stackSize, uint8_t priority, int8_t core) : Threaded(name, stackSize, priority, core), SleepTime(loopInterval){}

SleepyThreaded::~SleepyThreaded() = default;

void SleepyThreaded::pause(){
	// Suspends the task in place instead of stop()+start()-ing it (which deleted and recreated
	// the FreeRTOS task, and its stack, on every single sleep/wake cycle - the exact same
	// alloc/free-every-cycle shape that fragmented the retention pool in Sleep.cpp, just against
	// the general internal-8bit pool instead. Confirmed via a coredump: a "Mem alloc fail" abort
	// trying to allocate this task's stack back on resume(), from Sleep::sleep()'s wake path.
	if(paused || !running()) return;
	paused = true;
	vTaskSuspend(getTask());
}

void SleepyThreaded::resume(){
	if(!paused) return;
	paused = false;
	vTaskResume(getTask());
}

void SleepyThreaded::resetTime(){
	lastLoop = millis();
}

void SleepyThreaded::loop(){
	if(millis() - lastLoop < SleepTime){
		vTaskDelay(SleepTime - (millis() - lastLoop));
		return;
	}

	resetTime();
	sleepyLoop();
}
