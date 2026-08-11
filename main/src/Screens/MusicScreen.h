#ifndef ARTEMIS_FIRMWARE_MUSICSCREEN_H
#define ARTEMIS_FIRMWARE_MUSICSCREEN_H

#include "LV_Interface/LVScreen.h"
#include "Util/Events.h"
#include <string>

// Now-playing screen fed by Service::Media (GBMusic, populated from GadgetBridge's musicinfo/
// musicstate over the Bangle link on a-dev). Up = next, Down = prev, Select = play/pause,
// Alt = back to the lock screen. Track/artist use the Cyrillic-capable devin font.
class MusicScreen : public LVScreen {
public:
	MusicScreen();

private:
	void onStart() override;
	void onStop() override;
	void loop() override;

	void refresh();
	void handleInput();

	lv_obj_t* titleLabel = nullptr;
	lv_obj_t* artistLabel = nullptr;
	lv_obj_t* stateLabel = nullptr;
	lv_obj_t* hintLabel = nullptr;

	EventQueue queue;
	std::string lastTitle;
	std::string lastArtist;
	bool lastPlaying = false;
	bool first = true;
};

#endif //ARTEMIS_FIRMWARE_MUSICSCREEN_H
