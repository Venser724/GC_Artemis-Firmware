#ifndef ARTEMIS_FIRMWARE_MEDIA_H
#define ARTEMIS_FIRMWARE_MEDIA_H

#include <string>

// Shared now-playing interface registered as Service::Media. Backend differs per branch (a-dev:
// GadgetBridge musicinfo/musicstate over the Bangle UART link; i-dev: AMS GATT to the iPhone) -
// this is the common shape both expose so MusicScreen doesn't need to know which one it's talking to.
class Media {
public:
	struct NowPlaying {
		std::string title;
		std::string artist;
		bool playing = false;
	};

	enum class Command { Next, Previous, TogglePlayPause };

	virtual ~Media() = default;
	virtual NowPlaying get() = 0;
	virtual void sendCommand(Command cmd) = 0;
};

#endif //ARTEMIS_FIRMWARE_MEDIA_H
