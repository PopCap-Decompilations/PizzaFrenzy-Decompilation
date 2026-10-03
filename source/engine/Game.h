// engine::Game: the abstract game the application drives (settings, normal and screen-saver runs, shutdown).
#pragma once

#include <windows.h>

#include "Object.h"

namespace engine
{
	struct AppConfig;
	class Application;

	// Made by the game's factory, held by the Win32 application and deleted after its message loop. Layout: Object
	// +0x00, then the vtordisp and Interface (0x14 bytes); no members of its own.
	class Game : public Object
	{
	public:
		Game();
		virtual ~Game();

		// slot 1: fills the application's window settings before the window exists
		virtual void getSettings(AppConfig& settings) = 0;
		// slot 2: normal run, called once the window exists
		virtual void start(Application* app) = 0;
		// slot 3: "/s" command line (screen-saver run)
		virtual void startScreenSaver(Application* app) = 0;
		// slot 4: "/c" command line (screen-saver configuration; parent from the argument or the foreground window)
		virtual void configureScreenSaver(HINSTANCE instance, HWND parent) = 0;
		// slot 5: after the message loop, before the Game is deleted
		virtual void shutdown() = 0;
	};
}
