#include "MusicScreen.h"
#include "Screens/Lock/LockScreen.h"
#include "Notifs/AMS/Client.h"
#include "Util/Services.h"
#include "Devices/Input.h"
#include "Theme/theme.h"
#include <memory>

MusicScreen::MusicScreen() : queue(8){
	lv_obj_set_size(*this, 128, 128);
	lv_obj_set_style_bg_color(*this, lv_color_black(), 0);
	lv_obj_set_style_bg_opa(*this, LV_OPA_COVER, 0);
	lv_obj_set_style_border_width(*this, 0, 0);
	lv_obj_set_style_pad_all(*this, 0, 0);

	titleLabel = lv_label_create(*this);
	lv_label_set_long_mode(titleLabel, LV_LABEL_LONG_SCROLL_CIRCULAR);
	lv_obj_set_width(titleLabel, 120);
	lv_obj_set_style_text_align(titleLabel, LV_TEXT_ALIGN_CENTER, 0);
	lv_obj_set_style_text_font(titleLabel, &devin, 0);
	lv_obj_set_style_text_color(titleLabel, lv_color_white(), 0);
	lv_obj_align(titleLabel, LV_ALIGN_TOP_MID, 0, 18);
	lv_label_set_text(titleLabel, "—");

	artistLabel = lv_label_create(*this);
	lv_label_set_long_mode(artistLabel, LV_LABEL_LONG_SCROLL_CIRCULAR);
	lv_obj_set_width(artistLabel, 120);
	lv_obj_set_style_text_align(artistLabel, LV_TEXT_ALIGN_CENTER, 0);
	lv_obj_set_style_text_font(artistLabel, &devin, 0);
	lv_obj_set_style_text_color(artistLabel, lv_color_hex(0xAAAAAA), 0);
	lv_obj_align(artistLabel, LV_ALIGN_TOP_MID, 0, 46);
	lv_label_set_text(artistLabel, "");

	stateLabel = lv_label_create(*this);
	lv_obj_set_style_text_font(stateLabel, &devin, 0);
	lv_obj_set_style_text_color(stateLabel, lv_color_white(), 0);
	lv_obj_align(stateLabel, LV_ALIGN_CENTER, 0, 16);
	lv_label_set_text(stateLabel, "||");

	hintLabel = lv_label_create(*this);
	lv_obj_set_style_text_align(hintLabel, LV_TEXT_ALIGN_CENTER, 0);
	lv_obj_set_style_text_font(hintLabel, &devin, 0);
	lv_obj_set_style_text_color(hintLabel, lv_color_hex(0x555555), 0);
	lv_obj_align(hintLabel, LV_ALIGN_BOTTOM_MID, 0, -4);
	lv_label_set_text(hintLabel, "prev  OK  next");
}

void MusicScreen::onStart(){
	queue.reset();
	Events::listen(Facility::Input, &queue);
	first = true;
}

void MusicScreen::onStop(){
	Events::unlisten(&queue);
}

void MusicScreen::loop(){
	handleInput();
	refresh();
}

void MusicScreen::refresh(){
	auto* media = (AMS::Client*) Services.get(Service::Media);
	if(media == nullptr) return;

	const auto np = media->get();
	if(first || np.title != lastTitle){
		lv_label_set_text(titleLabel, np.title.empty() ? "—" : np.title.c_str());
		lastTitle = np.title;
	}
	if(first || np.artist != lastArtist){
		lv_label_set_text(artistLabel, np.artist.c_str());
		lastArtist = np.artist;
	}
	if(first || np.playing != lastPlaying){
		lv_label_set_text(stateLabel, np.playing ? ">" : "||");
		lastPlaying = np.playing;
	}
	first = false;
}

void MusicScreen::handleInput(){
	auto* media = (AMS::Client*) Services.get(Service::Media);

	Event evt;
	while(queue.get(evt, 0)){
		if(evt.facility == Facility::Input){
			auto data = (Input::Data*) evt.data;
			if(data->action == Input::Data::Press){
				if(data->btn == Input::Alt){
					free(evt.data);
					transition([](){ return std::make_unique<LockScreen>(); });
					return;
				}
				if(media != nullptr){
					switch(data->btn){
						case Input::Up:     media->sendCommand(AMS::RemoteCommandID::NextTrack); break;
						case Input::Down:   media->sendCommand(AMS::RemoteCommandID::PreviousTrack); break;
						case Input::Select: media->sendCommand(AMS::RemoteCommandID::TogglePlayPause); break;
						default: break;
					}
				}
			}
		}
		free(evt.data);
	}
}
