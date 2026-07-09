#include "AlarmScreen.h"
#include "Screens/Lock/LockScreen.h"
#include "Services/StatusCenter.h"
#include "Services/ChirpSystem.h"
#include "Util/Services.h"
#include "Util/stdafx.h"
#include "Devices/Input.h"
#include "Theme/theme.h"

AlarmScreen::AlarmScreen() : queue(8){
	lv_obj_set_size(*this, 128, 128);

	bg = lv_obj_create(*this);
	lv_obj_add_flag(bg, LV_OBJ_FLAG_FLOATING);
	lv_obj_set_size(bg, 128, 128);
	lv_obj_set_pos(bg, 0, 0);
	lv_obj_set_style_bg_color(bg, lv_color_black(), 0);
	lv_obj_set_style_bg_opa(bg, LV_OPA_COVER, 0);

	label = lv_label_create(*this);
	lv_label_set_text(label, "ALARM\n\nHold ALT\nto stop");
	lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
	lv_obj_set_style_text_font(label, &devin, 0);
	lv_obj_set_style_text_color(label, lv_color_white(), 0);
	lv_obj_center(label);
}

void AlarmScreen::onStart(){
	queue.reset();
	Events::listen(Facility::Input, &queue);

	startTime = millis();
	lastRing = 0;
	altDown = false;
}

void AlarmScreen::onStop(){
	Events::unlisten(&queue);
}

void AlarmScreen::loop(){
	if(millis() - lastRing >= RingInterval){
		ring();
		lastRing = millis();
	}

	Event evt;
	while(queue.get(evt, 0)){
		if(evt.facility == Facility::Input){
			auto data = (Input::Data*) evt.data;
			if(data->btn == Input::Alt){
				if(data->action == Input::Data::Press){
					altDown = true;
					altDownTime = millis();
				}else{
					altDown = false;
				}
			}
		}
		free(evt.data);
	}

	if(altDown && millis() - altDownTime >= DismissHoldTime){
		dismiss();
		return;
	}

	if(millis() - startTime >= AutoStopTime){
		dismiss();
		return;
	}
}

void AlarmScreen::ring(){
	auto* audio = (ChirpSystem*) Services.get(Service::Audio);
	if(audio != nullptr){
		audio->play({
				Chirp{ .startFreq = 900, .endFreq = 900, .duration = 150 },
				Chirp{ .startFreq = 0, .endFreq = 0, .duration = 80 },
				Chirp{ .startFreq = 900, .endFreq = 900, .duration = 150 },
				Chirp{ .startFreq = 0, .endFreq = 0, .duration = 80 },
				Chirp{ .startFreq = 900, .endFreq = 900, .duration = 150 }
		});
	}

	auto* status = (StatusCenter*) Services.get(Service::Status);
	if(status != nullptr){
		status->blinkAll();
	}
}

void AlarmScreen::dismiss(){
	auto* audio = (ChirpSystem*) Services.get(Service::Audio);
	if(audio != nullptr){
		audio->stop();
	}

	transition([](){ return std::make_unique<LockScreen>(); });
}
