// engine::AppConfig: the window settings the game fills for the application (Game::getSettings, before the
// window exists); held by value in Win32Application (+0x190). Only the string is constructed there.
#pragma once

#include <string>

namespace engine
{
	struct AppConfig
	{
		std::string title;						// +0x00 window title and class caption (resource string 203)
		int iconId;								// +0x1C icon resource, read as a WORD for MAKEINTRESOURCE (103)
		int frameRate;							// +0x20 frames per second of the main loop (30); must not be 0
		int width;								// +0x24 client width (800)
		int height;								// +0x28 client height (600)
		bool fullscreen;						// +0x2C
	};
}
