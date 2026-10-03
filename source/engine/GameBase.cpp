#include "GameBase.h"

#include "Application.h"
#include "SimpleSound.h"
#include "SimpleSoundDX.h"

namespace engine
{
	// 0x46ADD0
	GameBase::~GameBase()
	{
	}

	// 0x46AE00
	bool GameBase::initSound()
	{
		SimpleSound* sound = SimpleSoundDX::create(getApplication()->getWindowHandle());
		getApplication()->setSoundSystem(sound);
		m_soundAvailable = sound != 0;
		m_musicAvailable = sound != 0;
		return m_soundAvailable;
	}

	// 0x46AE40
	bool GameBase::isMusicAvailable() const
	{
		return m_musicAvailable;
	}

	// 0x46AE50
	void GameBase::setMusicEnabled(bool enabled)
	{
		SimpleSound* sound = getSoundSystem();
		if (sound)
			sound->setCategoryVolume(1, enabled ? 1.0f : 0.0f);
	}

	// 0x46AE90
	void GameBase::setSoundEnabled(bool enabled)
	{
		SimpleSound* sound = getSoundSystem();
		if (sound)
			sound->setCategoryVolume(0, enabled ? 1.0f : 0.0f);
	}

	// 0x46AED0
	GameBase::GameBase()
		: m_soundAvailable(false)
		, m_musicAvailable(false)
	{
	}

	// 0x4D1140 (folded)
	bool GameBase::isSoundAvailable() const
	{
		return m_soundAvailable;
	}
}
