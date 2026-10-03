// engine::OptionsScreen: the standard options screen (sound, music and full-screen checkboxes, version text).
#pragma once

#include <string>

#include "Screen.h"

namespace engine
{
	class GameBase;
	class Settings;

	// Screen +0x00, members from +0x1D4, then the vtordisp (+0x1DC) and the Interface subobject (+0x1E0): 0x1E4
	// bytes. The constructor leaves m_settings and m_game uninitialised (init sets them).
	class OptionsScreen : public Screen
	{
	public:
		OptionsScreen();
		virtual ~OptionsScreen();

		// checkboxes from the settings (unavailable sound/music disabled at alpha 0.8), m_actionSignal ->
		// handleAction, "Version %s" in VersionText
		virtual void init(GameBase* game, Settings* settings);			// slot 95
		// toggleSound, toggleMusic, toggleFullScreen: store, apply, settings->save()
		virtual void handleAction(const std::string& action);			// slot 96

		Settings* m_settings;								// +0x1D4
		GameBase* m_game;									// +0x1D8
	};
}
