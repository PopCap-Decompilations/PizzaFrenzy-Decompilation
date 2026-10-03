// engine::Settings: the user settings kept in the registry (full screen, music, sound, distributor links).
#pragma once

#include <string>

#include "Object.h"
#include "RefPtr.h"

namespace engine
{
	class RegistryKey;

	// Values under <company>\<product> (resource strings 201 and 202 in the game): FullScreen, MusicEnabled,
	// SoundEnabled, ShowLink (int, default 1), VisitURL, ILink (string, default ""). The game's GameSettings (0x4FA868)
	// adds ReplayStory. Object +0x00, members from +0x0C, then the vtordisp (+0x4C) and the Interface subobject
	// (+0x50): 0x54 bytes. Slots 2, 4, 10 and 11 have folded bodies (0x44E1E0, 0x4797F0, 0x4833C0, 0x479850).
	class Settings : public Object
	{
	public:
		Settings(const std::string& company, const std::string& product);
		virtual ~Settings();

		virtual void setFullScreen(bool fullScreen);							// slot 1
		virtual bool isFullScreen() const;										// slot 2: folded 0x44E1E0
		virtual void setMusicEnabled(bool enabled);								// slot 3
		virtual bool isMusicEnabled() const;									// slot 4: folded 0x4797F0
		virtual void setSoundEnabled(bool enabled);								// slot 5
		virtual bool isSoundEnabled() const;									// slot 6
		virtual bool getShowLink() const;										// slot 7 (the getter precedes the setter here)
		virtual void setShowLink(bool show);									// slot 8
		virtual const std::string& getVisitUrl() const;							// slot 9
		virtual void setVisitUrl(const std::string& url);						// slot 10: folded 0x4833C0
		virtual const std::string& getILink() const;							// slot 11: folded 0x479850
		virtual void setILink(const std::string& link);							// slot 12
		virtual void load();													// slot 13
		virtual void save();													// slot 14

		RefPtr<RegistryKey> m_registryKey;		// +0x0C Application::openRootRegistryKey(1, company + "\\" + product)
		bool m_fullScreen;						// +0x10 "FullScreen"
		bool m_musicEnabled;					// +0x11 "MusicEnabled"
		bool m_soundEnabled;					// +0x12 "SoundEnabled"
		bool m_showLink;						// +0x13 "ShowLink"
		std::string m_iLink;					// +0x14 "ILink"
		std::string m_visitUrl;					// +0x30 "VisitURL"
	};
}
