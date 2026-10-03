// engine::Win32Application: the Win32 implementation of engine::Application (window class "Gatsu", screensaver
// switches, PeekMessage loop at the game's frame rate, frame timers, DirectDraw display, factory of the platform
// objects); createApplication and the global g_win32Application that WinMain owns.
#pragma once

#include <string>

#include <windows.h>
#include <wininet.h>

#include "AppConfig.h"
#include "Application.h"
#include "CommandLine.h"
#include "MovingAverage.h"
#include "Point.h"
#include "RefPtr.h"
#include "sigslot.h"

namespace engine
{
	class Bitmap;
	class DDrawDisplay;
	class Font;
	class Game;
	class Graphics;
	class RegistryKey;
	class Runnable;
	class SeekableInputStream;
	class SimpleSound;
	class Thread;
	class URLConnection;

	// Not an engine::Object: Application (+0x000, 0x14C bytes) and has_slots<> (+0x14C) are its only bases.
	class Win32Application : public Application, public sigslot::has_slots<>
	{
	public:
		Win32Application();
		virtual ~Win32Application();

		// Application
		virtual std::string getVersion();													// slot 0
		virtual bool createDisplay(bool fullScreen);										// slot 1
		virtual bool setFullScreen(bool fullScreen);										// slot 2
		virtual bool isFullScreen();														// slot 3
		virtual int getWidth();																// slot 4
		virtual int getHeight();															// slot 5
		virtual HWND getWindowHandle();														// slot 6
		virtual HINSTANCE getInstanceHandle();												// slot 7
		virtual Bitmap* createImage(const char* path);										// slot 25
		virtual Bitmap* createDiskImage(const char* path);									// slot 26
		virtual Bitmap* createBlankImage(int width, int height);							// slot 30
		virtual Graphics* createGraphics(Bitmap* image);									// slot 31
		virtual bool nativeFileExists(const std::string& path);							// slot 32
		virtual SeekableInputStream* openNativeFile(const std::string& path);				// slot 33
		virtual URLConnection* openUrl(const std::string& url);								// slot 34
		virtual std::string loadString(unsigned int id);									// slot 35
		virtual std::string formatString(unsigned int id, ...);							// slot 36
		virtual RegistryKey* openRegistryKey(RegistryKey* parent, const std::string& name);	// slot 37
		virtual RegistryKey* openRootRegistryKey(int root, const std::string& path);		// slot 38
		virtual Thread* createThread(Runnable* runnable);									// slot 39
		virtual void log(const char* format, ...);											// slot 40 (0x4BD010, folded with slot 41)
		virtual void logSystem(const char* format, ...);									// slot 41 (0x4BD010)
		virtual std::string getErrorMessage(unsigned long errorCode);						// slot 42
		virtual __int64 getTicks();															// slot 43
		virtual __int64 getTicksPerSecond();												// slot 44
		virtual float getFrameTime();														// slot 45
		virtual float getFrameRate();														// slot 46
		virtual int getCountPerSecond();													// slot 47
		virtual void quit(int exitCode);													// slot 48
		virtual void setSoundSystem(SimpleSound* soundSystem);								// slot 49
		virtual const Point& getMousePosition();											// slot 50
		virtual void invokeCallback(int a, int b);											// slot 51
		virtual Font* createFont(const char* path);											// slot 53
		virtual Font* createDiskFont(const char* path);										// slot 54

		void setWindowHandle(HWND hwnd);
		void initTimer();
		void onClose();
		void captureMouse(HWND hwnd, int button);
		void releaseMouse(int button);
		void setMousePosition(Point& position);
		__int64 tick();
		LRESULT handleMessage(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
		Game* createGame();
		int run(HINSTANCE instance, LPSTR commandLine);

		HINSTANCE m_instance;								// +0x15C set by run()
		CommandLine m_commandLine;							// +0x160 run() queries the switches s, p, l, c
		AppConfig m_config;									// +0x190 filled by the game's getSettings (only its string is constructed)
		bool m_embedded;									// +0x1C0 true until run() starts: quit() posts no WM_QUIT, full screen is refused
		std::string m_version;								// +0x1C4 cache of getVersion
		std::string m_baseDirectory;						// +0x1E0 prefix of relative paths; never assigned (empty)
		Game* m_game;										// +0x1FC from ::createGame, deleted at the end of run(); not initialised by the constructor
		HWND m_hwnd;										// +0x200 the main window
		HINTERNET m_internet;								// +0x204 InternetOpenA("Sprout Game") session made by the first openUrl; never closed
		RefPtr<DDrawDisplay> m_display;						// +0x208 made by createDisplay
															// +0x20C (padding: m_frameStartTime is 8-aligned)
		__int64 m_frameStartTime;							// +0x210 getTicks() at the start of the current frame; not initialised by the constructor
		__int64 m_ticksPerFrame;							// +0x218 getTicksPerSecond() / m_config.frameRate; not initialised by the constructor
		MovingAverage m_frameIntervals;						// +0x220 the last 30 frame-to-frame intervals in ticks (getFrameRate)
		MovingAverage m_frameDurations;						// +0x240 the last 30 update+draw durations in ticks (getFrameTime)
		int m_mouseCapture;									// +0x260 buttons held (1 left, 2 right): SetCapture on the first, ReleaseCapture after the last
		Point m_mousePosition;								// +0x264 the last mouse position, clamped to the window
		void (__stdcall* m_hostCallback)(int, int, void*);	// +0x26C called by invokeCallback; only zeroed in this build
		void* m_hostCallbackData;							// +0x270 third argument of m_hostCallback
		bool m_minimized;									// +0x274 the loop sleeps instead of ticking
	};

	Win32Application* createApplication();
}

// The game's factory (defined by the game, PizzaFrenzy.cpp); Win32Application::createGame calls it.
engine::Game* createGame();

extern engine::Win32Application* g_win32Application;
