#include <float.h>
#include <io.h>
#include <stdlib.h>
#include <string.h>

#include <algorithm>
#include <string>

#include <windows.h>
#include <shlwapi.h>
#include <strsafe.h>

#include "Win32Application.h"

#include "Blitter.h"
#include "Component.h"
#include "DDrawDisplay.h"
#include "Exception.h"
#include "ExceptionHandler.h"
#include "FileInputStream.h"
#include "Font.h"
#include "Game.h"
#include "MemoryImage.h"
#include "SoftwareGraphics.h"
#include "Win32RegistryKey.h"
#include "Win32Thread.h"
#include "WinInetUrl.h"
#include "Window.h"

engine::Win32Application* g_win32Application;

namespace engine
{
	// 0x4BC830
	void Win32Application::setWindowHandle(HWND hwnd)
	{
		m_hwnd = hwnd;
		m_minimized = IsIconic(hwnd) != FALSE;
	}

	// 0x4BC860
	void Win32Application::setSoundSystem(SimpleSound* soundSystem)
	{
		Application::setSoundSystem(soundSystem);
	}

	// 0x4BC870
	void Win32Application::initTimer()
	{
		_controlfp(_MCW_EM, _MCW_EM);
		m_ticksPerFrame = getTicksPerSecond() / m_config.frameRate;
		m_frameStartTime = getTicks();
	}

	// 0x4BC8D0
	void Win32Application::onClose()
	{
		quit(0);
	}

	// 0x4BC8E0
	void Win32Application::invokeCallback(int a, int b)
	{
		if (m_hostCallback != 0)
		{
			m_hostCallback(a, b, m_hostCallbackData);
		}
	}

	// 0x4BC900
	int Win32Application::getWidth()
	{
		return m_config.width;
	}

	// 0x4BC910
	int Win32Application::getHeight()
	{
		return m_config.height;
	}

	// 0x4BC920
	HWND Win32Application::getWindowHandle()
	{
		return m_hwnd;
	}

	// 0x4BC930
	HINSTANCE Win32Application::getInstanceHandle()
	{
		return m_instance;
	}

	// 0x4BC940
	Bitmap* Win32Application::createImage(const char* path)
	{
		MemoryImage* image = new MemoryImage();
		image->load(path, false);
		return image;
	}

	// 0x4BC9D0
	Bitmap* Win32Application::createDiskImage(const char* path)
	{
		MemoryImage* image = new MemoryImage();
		image->load(path, true);
		return image;
	}

	// 0x4BCA60
	Font* Win32Application::createFont(const char* path)
	{
		Font* font = new Font(this);
		font->load(path);
		return font;
	}

	// 0x4BCAD0
	Font* Win32Application::createDiskFont(const char* path)
	{
		Font* font = new Font(this);
		font->loadResource(path);
		return font;
	}

	// 0x4BCB40
	Bitmap* Win32Application::createBlankImage(int width, int height)
	{
		MemoryImage* image = new MemoryImage();
		image->create(width, height);
		return image;
	}

	// 0x4BCBD0
	Graphics* Win32Application::createGraphics(Bitmap* image)
	{
		return new SoftwareGraphics(static_cast<Surface*>(image));
	}

	// 0x4BCC50
	URLConnection* Win32Application::openUrl(const std::string& url)
	{
		if (m_internet == 0)
		{
			m_internet = InternetOpenA("Sprout Game", INTERNET_OPEN_TYPE_PRECONFIG, 0, 0, 0);
		}
		return new WinInetUrl(m_internet, url);
	}

	// 0x4BCCF0
	RegistryKey* Win32Application::openRootRegistryKey(int root, const std::string& path)
	{
		Win32RegistryKey* key = new Win32RegistryKey();
		if (key->open(root, path))
		{
			return key;
		}
		delete key;
		return 0;
	}

	// 0x4BCD80
	RegistryKey* Win32Application::openRegistryKey(RegistryKey* parent, const std::string& name)
	{
		Win32RegistryKey* key = new Win32RegistryKey();
		if (key->open(parent, name))
		{
			return key;
		}
		delete key;
		return 0;
	}

	// 0x4BCE10
	Thread* Win32Application::createThread(Runnable* runnable)
	{
		return new Win32Thread(runnable);
	}

	// 0x4BCE80
	__int64 Win32Application::getTicks()
	{
		LARGE_INTEGER counter;
		if (!QueryPerformanceCounter(&counter))
		{
			return timeGetTime();
		}
		return counter.QuadPart;
	}

	// 0x4BCEB0
	__int64 Win32Application::getTicksPerSecond()
	{
		LARGE_INTEGER frequency;
		if (!QueryPerformanceFrequency(&frequency))
		{
			return 1000;
		}
		return frequency.QuadPart;
	}

	// 0x4BCEE0
	float Win32Application::getFrameTime()
	{
		return (float)m_frameDurations.getAverage() / (float)getTicksPerSecond();
	}

	// 0x4BCF20
	float Win32Application::getFrameRate()
	{
		return (float)getTicksPerSecond() / (float)m_frameIntervals.getAverage();
	}

	// 0x4BCF60
	int Win32Application::getCountPerSecond()
	{
		return (int)getFilteredBlitRate();
	}

	// 0x4BCF70
	void Win32Application::quit(int exitCode)
	{
		if (!m_embedded)
		{
			PostQuitMessage(exitCode);
		}
	}

	// 0x4BCF90
	void Win32Application::captureMouse(HWND hwnd, int button)
	{
		if (m_mouseCapture == 0)
		{
			SetCapture(hwnd);
		}
		m_mouseCapture |= button;
	}

	// 0x4BCFE0
	void Win32Application::releaseMouse(int button)
	{
		m_mouseCapture &= ~button;
		if (m_mouseCapture == 0)
		{
			ReleaseCapture();
		}
	}

	// 0x4BD000
	const Point& Win32Application::getMousePosition()
	{
		return m_mousePosition;
	}

	// 0x4BD010 (folded)
	void Win32Application::log(const char* format, ...)
	{
		char* buffer = getTextBuffer();
		va_list args;
		va_start(args, format);
		StringCchVPrintfA(buffer, getTextBufferSize(), format, args);
		ExceptionHandler::getInstance()->write(buffer);
		OutputDebugStringA(buffer);
	}

	// 0x4BD010 (folded)
	void Win32Application::logSystem(const char* format, ...)
	{
		char* buffer = getTextBuffer();
		va_list args;
		va_start(args, format);
		StringCchVPrintfA(buffer, getTextBufferSize(), format, args);
		ExceptionHandler::getInstance()->write(buffer);
		OutputDebugStringA(buffer);
	}

	// 0x4BD050
	void Win32Application::setMousePosition(Point& position)
	{
		int maxX = getWidth() - 1;
		position.x = position.x < 0 ? 0 : (position.x > maxX ? maxX : position.x);
		int maxY = getHeight() - 1;
		position.y = position.y < 0 ? 0 : (position.y > maxY ? maxY : position.y);
		m_mousePosition = position;
	}

	// No return statement, as in the original: without a display it returns the low byte of this, else whatever
	// DDrawDisplay::setFullScreen (void) leaves in eax.
#pragma warning(push)
#pragma warning(disable: 4716)
	// 0x4BD120
	bool Win32Application::setFullScreen(bool fullScreen)
	{
		if (m_display)
		{
			m_display->setFullScreen(fullScreen && !m_embedded);
		}
	}
#pragma warning(pop)

	// 0x4BD160
	bool Win32Application::isFullScreen()
	{
		if (m_display)
		{
			return m_display->isFullScreen();
		}
		return false;
	}

	// 0x4BD210
	__int64 Win32Application::tick()
	{
		UpdateContext context;
		__int64 now = getTicks();
		__int64 interval = now - m_frameStartTime;
		m_frameIntervals.add(interval);
		context.elapsed = (float)interval / (float)getTicksPerSecond();
		if (context.elapsed > 0.33f)
		{
			context.elapsed = 0.33f;
		}
		m_frameStartTime = now;
		m_updateSignal.emit(context);
		SoftwareGraphics graphics(m_display);
		resetFilteredBlitStats();
		if (m_display->isReady())
		{
			m_drawSignal.emit(graphics);
			m_display->present();
		}
		m_frameDurations.add(getTicks() - m_frameStartTime);
		return now + m_ticksPerFrame;
	}

	// 0x4BD3B0
	LRESULT Win32Application::handleMessage(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		Point position((short)LOWORD(lParam), (short)HIWORD(lParam));
		switch (message)
		{
		case WM_SIZE:
			m_minimized = wParam == SIZE_MINIMIZED;
			break;
		case WM_ACTIVATE:
			m_activateSignal.emit(LOWORD(wParam) != WA_INACTIVE);
			return 0;
		case WM_PAINT:
			if (m_display && !IsIconic(hwnd))
			{
				m_display->restoreSurfaces();
				m_display->present();
			}
			break;
		case WM_CLOSE:
			quit(0);
			return 0;
		case WM_KEYDOWN:
			if (!(lParam & 0x40000000))
			{
				m_keyDownSignal.emit(wParam);
			}
			else
			{
				m_keyRepeatSignal.emit(wParam, lParam & 0xFFFF);
			}
			return 0;
		case WM_KEYUP:
			m_keyUpSignal.emit(wParam);
			return 0;
		case WM_CHAR:
			m_charSignal.emit((char)wParam);
			return 0;
		case WM_SYSCOMMAND:
			switch (wParam & 0xFFF0)
			{
			case SC_CLOSE:
				m_closeSignal.emit();
				return 0;
			case SC_MINIMIZE:
				m_minimizeSignal.emit();
				return 0;
			case SC_MAXIMIZE:
				m_maximizeSignal.emit();
				return 0;
			case SC_RESTORE:
				m_restoreSignal.emit();
				return 0;
			case SC_SCREENSAVE:
				return 0;
			}
			break;
		case WM_TIMER:
			if (wParam == 1)
			{
				m_modalTimerSignal.emit(100);
			}
			break;
		case WM_MOUSEMOVE:
			setMousePosition(position);
			m_mouseMoveSignal.emit(position);
			return 0;
		case WM_LBUTTONDOWN:
		case WM_LBUTTONDBLCLK:
			captureMouse(hwnd, 1);
			setMousePosition(position);
			m_mouseDownSignal.emit(position);
			return 0;
		case WM_LBUTTONUP:
			releaseMouse(1);
			setMousePosition(position);
			m_mouseUpSignal.emit(position);
			return 0;
		case WM_RBUTTONDOWN:
			captureMouse(hwnd, 2);
			setMousePosition(position);
			m_rightMouseDownSignal.emit(position);
			return 0;
		case WM_RBUTTONUP:
			releaseMouse(2);
			setMousePosition(position);
			m_rightMouseUpSignal.emit(position);
			return 0;
		case WM_ENTERSIZEMOVE:
			SetTimer(hwnd, 1, 100, 0);
			break;
		case WM_EXITSIZEMOVE:
			KillTimer(hwnd, 1);
			break;
		}
		return DefWindowProcA(hwnd, message, wParam, lParam);
	}

	// 0x4BD790
	std::string Win32Application::loadString(unsigned int id)
	{
		char* buffer = getTextBuffer();
		std::string text(buffer, LoadStringA(0, id, buffer, getTextBufferSize()));
		return text;
	}

	// 0x4BD850
	std::string Win32Application::formatString(unsigned int id, ...)
	{
		std::string format = loadString(id);
		if (format.empty())
		{
			return format;
		}
		char* buffer = getTextBuffer();
		va_list args;
		va_start(args, id);
		DWORD length = FormatMessageA(FORMAT_MESSAGE_FROM_STRING, format.c_str(), 0, 0, buffer, getTextBufferSize(), &args);
		va_end(args);
		if (length > 0)
		{
			return std::string(buffer, length);
		}
		std::string error = getErrorMessage(GetLastError());
		log("Failed to format message: %s\n", error.c_str());
		return format;
	}

	// 0x4BDA00
	std::string Win32Application::getErrorMessage(unsigned long errorCode)
	{
		char* buffer = getTextBuffer();
		DWORD length = FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, 0, errorCode, 0, buffer, getTextBufferSize(), 0);
		if (length > 0)
		{
			return std::string(buffer, length);
		}
		return std::string();
	}

	// 0x4BDA70
	Game* Win32Application::createGame()
	{
		m_game = ::createGame();
		m_game->getSettings(m_config);
		if (!m_config.title.empty() && m_config.frameRate != 0)
		{
			return m_game;
		}
		throw Exception("Invalid configuration.");
	}

	// 0x4BDB20
	std::string Win32Application::getVersion()
	{
		UINT valueLength = 0;		// zeroed on entry, before the test (VerQueryValueA's length, unused)
		if (m_version.empty())
		{
			std::string version("no version info");
			char* buffer = getTextBuffer();
			int size = getTextBufferSize();
			char* value = 0;
			DWORD handle;
			GetModuleFileNameA(0, buffer, size);
			DWORD infoSize = GetFileVersionInfoSizeA(buffer, &handle);
			if (infoSize != 0)
			{
				void* info = malloc(infoSize + 1);
				if (GetFileVersionInfoA(buffer, handle, infoSize, info))
				{
					StringCchPrintfA(buffer, size, "\\StringFileInfo\\%04X04B0\\FileVersion", GetUserDefaultLangID());
					if (VerQueryValueA(info, buffer, (LPVOID*)&value, &valueLength))
					{
						version = value;
					}
					StringCchPrintfA(buffer, size, "\\StringFileInfo\\%04X04B0\\SpecialBuild", GetUserDefaultLangID());
					if (VerQueryValueA(info, buffer, (LPVOID*)&value, &valueLength))
					{
						version += " ";
						version += value;
					}
				}
				free(info);
			}
			m_version = version;
		}
		return m_version;
	}

	// 0x4BDCF0
	bool Win32Application::nativeFileExists(const std::string& path)
	{
		_finddata_t data;
		memset(&data, 0, sizeof(data));
		bool exists = false;
		std::string fullPath;
		if (PathIsRelativeA(path.c_str()))
		{
			fullPath = m_baseDirectory;
			fullPath += path;
		}
		else
		{
			fullPath = path;
		}
		std::replace(fullPath.begin(), fullPath.end(), '\\', '/');
		intptr_t handle = _findfirst(fullPath.c_str(), &data);
		if (handle != -1)
		{
			exists = true;
		}
		_findclose(handle);
		return exists;
	}

	// 0x4BDE40
	SeekableInputStream* Win32Application::openNativeFile(const std::string& path)
	{
		FileInputStream* stream = new FileInputStream();
		if (PathIsRelativeA(path.c_str()))
		{
			stream->open(m_baseDirectory, path);
		}
		else
		{
			stream->open("", path);
		}
		return stream;
	}

	// 0x4BDF20
	int Win32Application::run(HINSTANCE instance, LPSTR commandLine)
	{
		m_embedded = false;
		m_instance = instance;
		m_commandLine.parse(commandLine);
		createGame();
		Window window;
		if (m_commandLine.hasOption("s"))
		{
			Window::registerClass(instance, "Gatsu", (WORD)m_config.iconId);
			window.create(instance, m_config.title, m_config.width, m_config.height);
			setWindowHandle(window.getHandle());
			m_game->startScreenSaver(this);
		}
		else if (m_commandLine.hasOption("p") || m_commandLine.hasOption("l"))
		{
			// the preview window is not supported: its parent is read and ignored
			if (m_commandLine.hasValue("p"))
			{
				HWND parent = (HWND)atoi(m_commandLine.getValue("p"));
			}
			return 0;
		}
		else if (m_commandLine.hasOption("c"))
		{
			if (m_commandLine.hasValue("c"))
			{
				m_game->configureScreenSaver(instance, (HWND)atoi(m_commandLine.getValue("c")));
			}
			else
			{
				m_game->configureScreenSaver(instance, GetForegroundWindow());
			}
			return 0;
		}
		else
		{
			Window::registerClass(instance, "Gatsu", (WORD)m_config.iconId);
			window.create(instance, m_config.title, m_config.width, m_config.height);
			setWindowHandle(window.getHandle());
			m_game->start(this);
		}
		initTimer();
		__int64 nextFrame = getTicks();
		MSG msg;
		bool running = true;
		do
		{
			while (running && PeekMessageA(&msg, 0, 0, 0, PM_REMOVE))
			{
				running &= msg.message != WM_QUIT;
				TranslateMessage(&msg);
				DispatchMessageA(&msg);
			}
			if (!m_minimized && getTicks() >= nextFrame)
			{
				nextFrame = tick();
			}
			else
			{
				Sleep(1);
			}
		} while (running);
		m_game->shutdown();
		m_display->shutdown();
		delete m_game;
		m_game = 0;
		return msg.wParam;
	}

	// 0x4BE220
	Win32Application::Win32Application()
		: m_instance(0), m_commandLine(0), m_embedded(true), m_hwnd(0), m_internet(0), m_frameIntervals(30), m_frameDurations(30),
		m_mouseCapture(0), m_mousePosition(0, 0), m_hostCallback(0), m_hostCallbackData(0), m_minimized(false)
	{
	}

	// 0x4BE370
	Win32Application::~Win32Application()
	{
	}

	// 0x4BE570
	Win32Application* createApplication()
	{
		g_win32Application = new Win32Application();
		g_application = g_win32Application;
		return g_win32Application;
	}

	// No return statement, as in the original: it returns whatever DDrawDisplay::setSize (void) leaves in eax.
#pragma warning(push)
#pragma warning(disable: 4716)
	// 0x4BE5F0
	bool Win32Application::createDisplay(bool fullScreen)
	{
		m_closeSignal.connect(this, &Win32Application::onClose);
		Window::setClientSize(m_hwnd, m_config.width, m_config.height);
		m_display = new DDrawDisplay(this, m_hwnd);
		m_display->setFullScreen(fullScreen && !m_embedded);
		m_display->setSize(m_config.width, m_config.height);
	}
#pragma warning(pop)
}

// 0x4BE700
int WINAPI WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR commandLine, int showCommand)
{
	int result = engine::createApplication()->run(instance, commandLine);
	if (g_win32Application != 0)
	{
		delete g_win32Application;
		g_win32Application = 0;
		engine::g_application = 0;
	}
	return result;
}
