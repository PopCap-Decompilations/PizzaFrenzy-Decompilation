#include "Window.h"

#include "Application.h"
#include "Win32Application.h"

namespace engine
{
	std::string Window::s_className;			// 0x5313C4

	// 0x4CE2E0
	LRESULT CALLBACK Window::windowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		return static_cast<Win32Application*>(getApplication())->handleMessage(hwnd, message, wParam, lParam);
	}

	// 0x4CE2F0
	void Window::setClientSize(HWND hwnd, int width, int height)
	{
		RECT rect = {0, 0, width - 1, height - 1};
		LONG style = GetWindowLongA(hwnd, GWL_STYLE);
		AdjustWindowRect(&rect, style, FALSE);
		int windowWidth = rect.right - rect.left + 1;
		int windowHeight = rect.bottom - rect.top + 1;

		RECT windowRect;
		GetWindowRect(hwnd, &windowRect);
		POINT position;
		position.x = windowRect.left;
		position.y = windowRect.top;
		if (style | WS_CHILD)
		{
			HWND parent = GetParent(hwnd);
			if (parent)
				ScreenToClient(parent, &position);
		}
		MoveWindow(hwnd, position.x, position.y, windowWidth, windowHeight, TRUE);
	}

	// 0x4CE3A0
	Window::Window()
		: m_hwnd(NULL)
	{
		m_style = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
	}

	// 0x4CE430
	Window::~Window()
	{
		if (m_hwnd)
		{
			DestroyWindow(m_hwnd);
			m_hwnd = NULL;
		}
	}

	// 0x4CE490
	void Window::create(HINSTANCE instance, const std::string& title, int width, int height)
	{
		RECT rect = {0, 0, width - 1, height - 1};
		AdjustWindowRect(&rect, m_style, FALSE);
		m_clientSize.x = width;
		m_clientSize.y = height;
		m_hwnd = CreateWindowExA(0, s_className.c_str(), title.c_str(), m_style, CW_USEDEFAULT, CW_USEDEFAULT,
			rect.right - rect.left + 1, rect.bottom - rect.top + 1, NULL, NULL, instance, this);
		SetFocus(m_hwnd);
		UpdateWindow(m_hwnd);
	}

	// 0x411960 (folded)
	HWND Window::getHandle() const
	{
		return m_hwnd;
	}

	// 0x4CE560
	bool Window::registerClass(HINSTANCE instance, const char* className, WORD iconId)
	{
		WNDCLASSA windowClass = {0};
		windowClass.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
		windowClass.lpfnWndProc = windowProc;
		windowClass.cbClsExtra = 0;
		windowClass.cbWndExtra = 0;
		windowClass.hInstance = instance;
		windowClass.hIcon = LoadIconA(instance, MAKEINTRESOURCEA(iconId));
		windowClass.hCursor = LoadCursorA(NULL, IDC_ARROW);
		windowClass.hbrBackground = NULL;
		windowClass.lpszMenuName = NULL;
		windowClass.lpszClassName = className;
		if (!RegisterClassA(&windowClass))
			return false;
		s_className = className;
		return true;
	}
}
