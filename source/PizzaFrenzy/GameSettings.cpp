#include "GameSettings.h"

#include <string>

#include "engine/Win32RegistryKey.h"

// 0x418B90
void GameSettings::setReplayStory(bool replay)
{
	m_replayStory = replay;
}

// 0x418BA0
bool GameSettings::getReplayStory() const
{
	return m_replayStory;
}

// 0x418BB0: m_replayStory is left for load()
GameSettings::GameSettings(const std::string& company, const std::string& product)
	: engine::Settings(company, product)
{
}

// 0x418C70: ReplayStory defaults to on. The original constructs an empty string that it never uses (its EH state
// 0 covers it, state 1 the "ReplayStory" temporary).
void GameSettings::load()
{
	engine::Settings::load();
	std::string value;
	// stored after the name's temporary dies (getInt 0x418CDF, delete 0x418CF0, store 0x418CFD)
	int flag = m_registryKey->getInt("ReplayStory", 1);
	m_replayStory = flag != 0;
}

// 0x418D20
void GameSettings::save()
{
	engine::Settings::save();
	m_registryKey->setInt("ReplayStory", m_replayStory ? 1 : 0);
}
