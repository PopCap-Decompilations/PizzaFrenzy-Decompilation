// engine::GameBase: the engine layer under the concrete game: creates the sound system and switches sound and music.
#pragma once

#include "Game.h"

namespace engine
{
	// Game +0x00, members from +0x0C, then the vtordisp (+0x10) and the Interface subobject (+0x14): 0x18 bytes.
	class GameBase : public Game
	{
	public:
		GameBase();
		virtual ~GameBase();

		// category volume 1 or 0 on the sound system (category 0 sounds, 1 music); `this` is unused
		virtual void setSoundEnabled(bool enabled);						// slot 6
		virtual void setMusicEnabled(bool enabled);						// slot 7

		bool initSound();
		bool isSoundAvailable() const;									// 0x4D1140 (folded with URLConnection::getUseCaches)
		bool isMusicAvailable() const;

		bool m_soundAvailable;								// +0x0C set by initSound
		bool m_musicAvailable;								// +0x0D set by initSound
	};
}
