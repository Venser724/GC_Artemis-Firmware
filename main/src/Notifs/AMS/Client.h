#ifndef ARTEMIS_FIRMWARE_AMS_CLIENT_H
#define ARTEMIS_FIRMWARE_AMS_CLIENT_H

#include <BLE/Client.h>
#include "Util/Threaded.h"
#include "Model.h"
#include <string>
#include <mutex>

namespace AMS {

// Apple Media Service client: the watch subscribes to the iPhone's AMS to receive now-playing
// info (track title, artist, playback state) and to send playback commands (play/pause/next/
// prev/volume). Registered as Service::Media so a media UI can read the state and issue commands.
// 128-bit UUIDs are stored little-endian (byte-reversed from the string form), same as ANCS.
class Client : public Threaded {
public:
	Client(BLE::Client* client);

	struct NowPlaying {
		std::string title;
		std::string artist;
		bool playing = false;
	};

	NowPlaying get();
	void sendCommand(RemoteCommandID cmd);

private:
	std::shared_ptr<BLE::Client::Service> service;
	std::shared_ptr<BLE::Client::Char> remoteCmd;
	std::shared_ptr<BLE::Client::Char> entityUpdate;

	void loop() override;
	void onEntityUpdateConnected();
	void handleEntityUpdate(const std::vector<uint8_t>& data);

	std::mutex mut;
	NowPlaying state;

	static constexpr esp_bt_uuid_t ServiceUUID       = { .len = ESP_UUID_LEN_128, .uuid = { .uuid128 = { 0xDC, 0xF8, 0x55, 0xAD, 0x02, 0xC5, 0xF4, 0x8E, 0x3A, 0x43, 0x36, 0x0F, 0x2B, 0x50, 0xD3, 0x89 }}};
	static constexpr esp_bt_uuid_t RemoteCommandUUID = { .len = ESP_UUID_LEN_128, .uuid = { .uuid128 = { 0xC2, 0x51, 0xCA, 0xF7, 0x56, 0x0E, 0xDF, 0xB8, 0x8A, 0x4A, 0xB1, 0x57, 0xD8, 0x81, 0x3C, 0x9B }}};
	static constexpr esp_bt_uuid_t EntityUpdateUUID  = { .len = ESP_UUID_LEN_128, .uuid = { .uuid128 = { 0x02, 0xC1, 0x96, 0xBA, 0x92, 0xBB, 0x0C, 0x9A, 0x1F, 0x41, 0x8D, 0x80, 0xCE, 0xAB, 0x7C, 0x2F }}};
};

}

#endif //ARTEMIS_FIRMWARE_AMS_CLIENT_H
