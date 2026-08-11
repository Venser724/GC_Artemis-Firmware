#include "GBMusic.h"

void GBMusic::setSender(Sender s){
	sender = std::move(s);
}

void GBMusic::setTrack(const std::string& title, const std::string& artist){
	std::lock_guard<std::mutex> lock(mut);
	state.title = title;
	state.artist = artist;
}

void GBMusic::setPlaying(bool playing){
	std::lock_guard<std::mutex> lock(mut);
	state.playing = playing;
}

Media::NowPlaying GBMusic::get(){
	std::lock_guard<std::mutex> lock(mut);
	return state;
}

void GBMusic::sendCommand(Command cmd){
	if(sender) sender(cmd);
}
