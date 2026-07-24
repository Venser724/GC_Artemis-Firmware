#ifndef ARTEMIS_FIRMWARE_AMS_MODEL_H
#define ARTEMIS_FIRMWARE_AMS_MODEL_H

#include <cstdint>

// Apple Media Service (AMS) model. The iPhone acts as the GATT server; the watch is the
// client. See https://developer.apple.com/library/archive/documentation/CoreBluetooth/Reference/AppleMediaService_Reference/
namespace AMS {

enum class EntityID : uint8_t {
	Player = 0, Queue = 1, Track = 2
};

enum class PlayerAttributeID : uint8_t {
	Name = 0, PlaybackInfo = 1, Volume = 2
};

enum class TrackAttributeID : uint8_t {
	Artist = 0, Album = 1, Title = 2, Duration = 3
};

// Written (single byte) to the Remote Command characteristic to control the iPhone's player.
enum class RemoteCommandID : uint8_t {
	Play = 0, Pause = 1, TogglePlayPause = 2, NextTrack = 3, PreviousTrack = 4,
	VolumeUp = 5, VolumeDown = 6, AdvanceRepeatMode = 7, AdvanceShuffleMode = 8,
	SkipForward = 9, SkipBackward = 10, LikeTrack = 11, DislikeTrack = 12, Bookmark = 13
};

}

#endif //ARTEMIS_FIRMWARE_AMS_MODEL_H
