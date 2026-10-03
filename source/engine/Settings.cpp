#include "Settings.h"

#include <string>

#include "Application.h"
#include "Win32RegistryKey.h"

namespace engine
{
	// 0x4797D0
	void Settings::setFullScreen(bool fullScreen)
	{
		m_fullScreen = fullScreen;
	}

	// 0x44E1E0 (folded): one body with the bool getters at +0x10 (EventAction::isDone...)
	bool Settings::isFullScreen() const
	{
		return m_fullScreen;
	}

	// 0x4797E0
	void Settings::setMusicEnabled(bool enabled)
	{
		m_musicEnabled = enabled;
	}

	// 0x4797F0 (folded): one body with the bool getters at +0x11
	bool Settings::isMusicEnabled() const
	{
		return m_musicEnabled;
	}

	// 0x479800
	void Settings::setSoundEnabled(bool enabled)
	{
		m_soundEnabled = enabled;
	}

	// 0x479810
	bool Settings::isSoundEnabled() const
	{
		return m_soundEnabled;
	}

	// 0x479820
	void Settings::setShowLink(bool show)
	{
		m_showLink = show;
	}

	// 0x479830
	bool Settings::getShowLink() const
	{
		return m_showLink;
	}

	// 0x479840
	const std::string& Settings::getVisitUrl() const
	{
		return m_visitUrl;
	}

	// 0x4833C0 (folded): one body with HighScoreTable::setTitle (a string assigned at +0x30)
	void Settings::setVisitUrl(const std::string& url)
	{
		m_visitUrl = url;
	}

	// 0x479850 (folded): one body with the getters of a string at +0x14
	const std::string& Settings::getILink() const
	{
		return m_iLink;
	}

	// 0x479860
	Settings::~Settings()
	{
		m_registryKey = 0;
	}

	// 0x479960: the key <company>\<product> under the application's root key 1; the values are read by load()
	Settings::Settings(const std::string& company, const std::string& product)
	{
		Application* application = getApplication();
		std::string path = company;
		path += "\\";
		path += product;
		m_registryKey = application->openRootRegistryKey(1, path);
	}

	// 0x479AC0
	void Settings::setILink(const std::string& link)
	{
		m_iLink = link;
	}

	// 0x479AE0: the flags default to on, the links to "". The original constructs an empty string that it never uses
	// (its EH state 0 covers it), and keeps each getInt result (at [esp+0x10]) until the name temporary is destroyed
	// before it tests it and stores the flag (0x479B5A, 0x479B7F-0x479B88).
	void Settings::load()
	{
		std::string value;
		int flag = m_registryKey->getInt("FullScreen", 1);
		m_fullScreen = flag != 0;
		flag = m_registryKey->getInt("MusicEnabled", 1);
		m_musicEnabled = flag != 0;
		flag = m_registryKey->getInt("SoundEnabled", 1);
		m_soundEnabled = flag != 0;
		flag = m_registryKey->getInt("ShowLink", 1);
		m_showLink = flag != 0;
		m_visitUrl = m_registryKey->getString("VisitURL", "");
		m_iLink = m_registryKey->getString("ILink", "");
	}

	// 0x479E50
	void Settings::save()
	{
		m_registryKey->setInt("FullScreen", m_fullScreen ? 1 : 0);
		m_registryKey->setInt("MusicEnabled", m_musicEnabled ? 1 : 0);
		m_registryKey->setInt("SoundEnabled", m_soundEnabled ? 1 : 0);
		m_registryKey->setInt("ShowLink", m_showLink ? 1 : 0);
		m_registryKey->setString("VisitURL", m_visitUrl);
		m_registryKey->setString("ILink", m_iLink);
	}
}
