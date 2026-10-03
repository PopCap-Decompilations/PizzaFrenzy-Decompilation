// GameSettings: the game's registry-backed settings (the engine's plus ReplayStory).
#pragma once

#include <string>

#include "engine/Settings.h"

// Implicit destructor (0x418C60).
class GameSettings : public engine::Settings
{
public:
	GameSettings(const std::string& company, const std::string& product);

	virtual void setReplayStory(bool replay);								// slot 15
	virtual bool getReplayStory() const;									// slot 16

	// overrides
	virtual void load();													// engine::Settings slot 13
	virtual void save();													// engine::Settings slot 14

	bool m_replayStory;									// +0x4C registry value "ReplayStory"
};
