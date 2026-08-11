#ifndef ARTEMIS_FIRMWARE_GBMUSIC_H
#define ARTEMIS_FIRMWARE_GBMUSIC_H

#include "Services/Media.h"
#include <functional>
#include <mutex>

// Bangle/GadgetBridge-backed Media implementation. State is set by Bangle's handle_musicinfo/
// handle_musicstate as GB() messages arrive over the UART link; commands are forwarded back out
// through the sender callback Bangle installs (mirrors NotifSource's callback-setter pattern -
// GBMusic has no UART access of its own, so it can't send anything until Bangle wires it up).
class GBMusic : public Media {
public:
	using Sender = std::function<void(Command)>;

	void setSender(Sender sender);

	void setTrack(const std::string& title, const std::string& artist);
	void setPlaying(bool playing);

	NowPlaying get() override;
	void sendCommand(Command cmd) override;

private:
	std::mutex mut;
	NowPlaying state;
	Sender sender;
};

#endif //ARTEMIS_FIRMWARE_GBMUSIC_H
