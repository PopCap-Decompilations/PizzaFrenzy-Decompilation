// engine::Window: the main window (window class "Gatsu" registered with windowProc, CreateWindowEx with a fixed
// style, DestroyWindow in the destructor); a local of Win32Application::run.
#pragma once

#include <string>

#include <windows.h>

#include "Object.h"
#include "Point.h"

namespace engine
{
	// Layout: Object +0x00, members +0x0C..+0x1B, then the vtordisp and Interface (0x24 bytes).
	class Window : public Object
	{
	public:
		Window();
		virtual ~Window();

		// Creates the window with a client area of width x height (not shown: the display's setup shows it).
		void create(HINSTANCE instance, const std::string& title, int width, int height);
		HWND getHandle() const;					// 0x411960 (folded): m_hwnd

		// the registered class's WNDPROC: Win32Application::handleMessage on engine::getApplication()
		static LRESULT CALLBACK windowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
		static void setClientSize(HWND hwnd, int width, int height);
		static bool registerClass(HINSTANCE instance, const char* className, WORD iconId);

		HWND m_hwnd;							// +0x0C made by create(); DestroyWindow'ed and cleared by the destructor
		DWORD m_style;							// +0x10 WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX (0xCB0000)
		Point m_clientSize;						// +0x14 width and height given to create()

		static std::string s_className;			// 0x5313C4 the class name registerClass registered ("Gatsu"), used by create()
	};
}
