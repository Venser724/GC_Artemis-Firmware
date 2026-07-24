#include "Client.h"
#include <esp_log.h>

static const char* TAG = "AMS";

namespace AMS {

Client::Client(BLE::Client* client) : Threaded("AMS", 3 * 1024){
	service = client->addService(ServiceUUID);
	remoteCmd = service->addChar(RemoteCommandUUID, ESP_GATT_CHAR_PROP_BIT_WRITE | ESP_GATT_CHAR_PROP_BIT_NOTIFY);
	entityUpdate = service->addChar(EntityUpdateUUID, ESP_GATT_CHAR_PROP_BIT_WRITE | ESP_GATT_CHAR_PROP_BIT_NOTIFY);

	// Subscribing (CCCD write) also drives the char to "connected", after which we can write.
	remoteCmd->setOnConnectedCb([this](){
		remoteCmd->writeDescr(ESP_GATT_UUID_CHAR_CLIENT_CONFIG, { 0x01, 0x00 });
	});
	entityUpdate->setOnConnectedCb([this](){ onEntityUpdateConnected(); });

	start();
}

void Client::onEntityUpdateConnected(){
	// Subscribe, then register which entities/attributes we want notifications for. Each write
	// is one entity followed by the attribute ids we care about.
	entityUpdate->writeDescr(ESP_GATT_UUID_CHAR_CLIENT_CONFIG, { 0x01, 0x00 });
	entityUpdate->write({ (uint8_t) EntityID::Track,  (uint8_t) TrackAttributeID::Title, (uint8_t) TrackAttributeID::Artist });
	entityUpdate->write({ (uint8_t) EntityID::Player, (uint8_t) PlayerAttributeID::PlaybackInfo });
}

void Client::loop(){
	if(entityUpdate == nullptr || !entityUpdate->connected()){
		vTaskDelay(500);
		return;
	}

	auto notif = entityUpdate->getNextNotif();
	if(!notif) return;

	handleEntityUpdate(notif->data);
}

void Client::handleEntityUpdate(const std::vector<uint8_t>& data){
	// Notification format: EntityID, AttributeID, EntityUpdateFlags, then the UTF-8 value
	// (possibly truncated - the Entity Attribute characteristic can fetch the full value if needed).
	if(data.size() < 3) return;

	const auto entity = (EntityID) data[0];
	const auto attr = data[1];
	const std::string value(data.cbegin() + 3, data.cend());

	std::lock_guard<std::mutex> lock(mut);
	if(entity == EntityID::Track){
		if(attr == (uint8_t) TrackAttributeID::Title)       state.title = value;
		else if(attr == (uint8_t) TrackAttributeID::Artist) state.artist = value;
		ESP_LOGI(TAG, "Now playing: %s - %s", state.artist.c_str(), state.title.c_str());
	}else if(entity == EntityID::Player){
		if(attr == (uint8_t) PlayerAttributeID::PlaybackInfo){
			// value is "state,rate,elapsedTime"; state 0=paused 1=playing 2=rewind 3=fast-fwd
			state.playing = !value.empty() && value.front() == '1';
			ESP_LOGI(TAG, "Playback %s", state.playing ? "playing" : "paused");
		}
	}
}

Client::NowPlaying Client::get(){
	std::lock_guard<std::mutex> lock(mut);
	return state;
}

void Client::sendCommand(RemoteCommandID cmd){
	if(remoteCmd == nullptr || !remoteCmd->connected()) return;
	remoteCmd->write({ (uint8_t) cmd });
}

}
