// engine::Display and engine::DDrawDisplay: the DirectDraw display (ddraw.dll loaded at run time, IDirectDraw2 with
// IDirectDrawSurface surfaces, GDI BitBlt fallback).
#include <string>

#include <windows.h>
#include <ddraw.h>

#include "DDrawDisplay.h"

#include "Application.h"
#include "Exception.h"
#include "Rect.h"
#include "Win32Application.h"

namespace engine
{
	typedef HRESULT (WINAPI* DirectDrawCreateFunc)(GUID* guid, LPDIRECTDRAW* directDraw, IUnknown* outer);

	static HMODULE s_ddrawDll = NULL;							// 0x5404F8 ddraw.dll, loaded by loadDirectDraw
	static DirectDrawCreateFunc s_directDrawCreate = NULL;		// 0x5404FC its DirectDrawCreate
	static GUID s_iidDirectDraw2 = IID_IDirectDraw2;			// 0x540500 (copied by the dynamic initializer 0x4F8AA0)

	// Zeroes a DirectDraw structure a DWORD at a time: the original's loop (rep stosd behind a begin < end test for
	// the member, single stores for locals), not a memset.
	template <class T>
	static void zeroStruct(T& s)
	{
		for (DWORD* p = (DWORD*)&s; p < (DWORD*)(&s + 1); p++)
			*p = 0;
	}

	// 0x4D1510 (folded)
	Bitmap* Display::copyFilteredScaled(float scale, PixelFilter* filter)
	{
		return NULL;
	}

	// 0x4D1500 (folded)
	Bitmap* Display::copyFiltered(PixelFilter* filter)
	{
		return NULL;
	}

	// 0x4D1510 (folded)
	Bitmap* Display::copyScaled(float scaleX, float scaleY)
	{
		return NULL;
	}

	// 0x4D1500 (folded)
	Bitmap* Display::copyScaledUniform(float scale)
	{
		return NULL;
	}

	// 0x4D14F0 (folded)
	Bitmap* Display::copy()
	{
		return NULL;
	}

	// 0x4D1520 (folded)
	void Display::scale(float scaleX, float scaleY)
	{
	}

	// 0x4CCA20
	bool DDrawDisplay::loadDirectDraw()
	{
		if (s_ddrawDll != NULL)
			return true;
		s_ddrawDll = LoadLibrary("ddraw.dll");
		if (s_ddrawDll != NULL)
		{
			s_directDrawCreate = (DirectDrawCreateFunc)GetProcAddress(s_ddrawDll, "DirectDrawCreate");
			if (s_directDrawCreate != NULL)
			{
				getApplication()->logSystem("ddraw.dll loaded\n");
				return true;
			}
			FreeLibrary(s_ddrawDll);	// s_ddrawDll is left set
		}
		return false;
	}

	// 0x4CCA90
	bool DDrawDisplay::isFullScreen() const
	{
		return m_fullScreen;
	}

	// 0x4CCAA0
	bool DDrawDisplay::isReady() const
	{
		return m_directDraw != NULL && m_primarySurface != NULL && m_primarySurface->IsLost() == DD_OK;
	}

	// 0x4CCAD0
	void DDrawDisplay::onActivate(bool active)
	{
		m_active = active;
	}

	// 0x4CCAE0
	bool DDrawDisplay::samePixelFormat(const DDPIXELFORMAT& a, const DDPIXELFORMAT& b)
	{
		return a.dwRGBBitCount == b.dwRGBBitCount && a.dwRBitMask == b.dwRBitMask && a.dwGBitMask == b.dwGBitMask
			&& a.dwBBitMask == b.dwBBitMask;
	}

	// 0x4CCB20
	void DDrawDisplay::present()
	{
		unlock();
		POINT origin = { 0, 0 };
		ClientToScreen(m_hwnd, &origin);
		RECT srcRect = { 0, 0, getWidth(), getHeight() };
		if (m_useGdiBlt)
		{
			HDC srcDC;
			m_backSurface->GetDC(&srcDC);
			HDC dstDC = GetDC(m_hwnd);
			BitBlt(dstDC, 0, 0, getWidth(), getHeight(), srcDC, 0, 0, SRCCOPY);
			m_backSurface->ReleaseDC(srcDC);
			ReleaseDC(m_hwnd, dstDC);
		}
		else
		{
			if (m_fullScreen)
				m_result = m_primarySurface->BltFast(origin.x, origin.y, m_backSurface, &srcRect, DDBLTFAST_WAIT);
			else
			{
				RECT dstRect;
				GetClientRect(m_hwnd, &dstRect);
				dstRect.left += origin.x;
				dstRect.right += origin.x;
				dstRect.top += origin.y;
				dstRect.bottom += origin.y;
				m_result = m_primarySurface->Blt(&dstRect, m_backSurface, &srcRect, DDBLT_WAIT, NULL);
			}
			if (FAILED(m_result))
			{
				if (m_result != DDERR_SURFACEBUSY && m_result != DDERR_SURFACELOST)
				{
					m_useGdiBlt = true;
					getApplication()->logSystem("DDraw blt failed.  Forcing GDI blt.\n( %s )\n", getErrorString());
				}
				else
					getApplication()->logSystem("DDrawDisplay::present - BltFast failed %s.\n", getErrorString());
			}
		}
	}

	// 0x4CCCC0
	int DDrawDisplay::getWidth() const
	{
		return m_width;
	}

	// 0x4CCCD0
	int DDrawDisplay::getHeight() const
	{
		return m_height;
	}

	// 0x4CCCE0
	PixelBuffer* DDrawDisplay::lock()
	{
		return lock(IntRect(0, 0, getWidth(), getHeight()));
	}

	// 0x4CCD60
	void DDrawDisplay::unlock()
	{
		if (m_buffer.m_pixels != NULL)
		{
			m_result = m_backSurface->Unlock(m_buffer.m_pixels);
			if (FAILED(m_result))
				getApplication()->logSystem("DDrawDisplay::unlock - Unlock failed %s.\n", getErrorString());
			m_buffer.m_pixels = NULL;
		}
	}

	// 0x4CCDB0
	const char* DDrawDisplay::getErrorString()
	{
		return "DirectX error (text not available)";
	}

	// 0x4CCDC0
	HRESULT WINAPI DDrawDisplay::enumModesCallback(LPDDSURFACEDESC desc, LPVOID context)
	{
		DDrawDisplay* display = (DDrawDisplay*)context;
		if ((desc->ddpfPixelFormat.dwFlags & DDPF_RGB) && desc->dwWidth == display->m_width
			&& desc->dwHeight == display->m_height)
			display->m_bitDepth = max(display->m_bitDepth, (int)desc->ddpfPixelFormat.dwRGBBitCount);
		return DDENUMRET_OK;
	}

	// 0x4CCE30
	void DDrawDisplay::cleanup()
	{
		if (m_backSurface != NULL)
		{
			m_backSurface->Release();
			m_backSurface = NULL;
		}
		if (m_primarySurface != NULL)
		{
			m_primarySurface->Release();
			m_primarySurface = NULL;
		}
		m_directDraw->Release();
		m_directDraw = NULL;
		if (m_fullScreen)
		{
			SetWindowLong(m_hwnd, GWL_STYLE, m_windowStyle);
			SetWindowLong(m_hwnd, GWL_EXSTYLE, m_windowExStyle);
			// HWND_TOPMOST (-1) when WS_EX_TOPMOST (bit 3) is set, else HWND_NOTOPMOST (-2): a shift and an or, no test
			SetWindowPos(m_hwnd, (HWND)(((DWORD)m_windowExStyle >> 3) | 0xFFFFFFFE), m_windowRect.left,
				m_windowRect.top, m_windowRect.right - m_windowRect.left, m_windowRect.bottom - m_windowRect.top,
				SWP_SHOWWINDOW);
		}
	}

	// 0x4CCEE0
	PixelBuffer* DDrawDisplay::lock(const IntRect& rect)
	{
		unlock();
		RECT lockRect = { rect.left, rect.top, rect.right, rect.bottom };
		zeroStruct(m_surfaceDesc);
		m_surfaceDesc.dwSize = sizeof(m_surfaceDesc);
		m_result = m_backSurface->Lock(&lockRect, &m_surfaceDesc, DDLOCK_WAIT, NULL);
		if (m_result == DD_OK)
		{
			m_buffer.m_width = rect.getWidth();
			m_buffer.m_height = rect.getHeight();
			m_buffer.m_pitch = m_surfaceDesc.lPitch / sizeof(Pixel);
			m_buffer.m_pixels = (Pixel*)m_surfaceDesc.lpSurface;
			return &m_buffer;
		}
		getApplication()->logSystem("DDSurface::Lock - lock failed %s.\n", getErrorString());
		switch (m_result)
		{
		case DD_OK:
			getApplication()->logSystem("The method succeeded.");
			break;
		case DDERR_INVALIDOBJECT:
			getApplication()->logSystem("DirectDraw received a pointer that was an invalid DIRECTDRAW object.\n");
			break;
		case DDERR_INVALIDPARAMS:
			getApplication()->logSystem("One or more of the input parameters is invalid.\n");
			break;
		case DDERR_OUTOFMEMORY:
			getApplication()->logSystem("DirectDraw does not have enough memory to perform the operation.\n");
			break;
		case DDERR_SURFACEBUSY:
			getApplication()->logSystem("Access to this surface is being refused because the surface is already locked by "
				"another thread.\n");
			break;
		case DDERR_SURFACELOST:
			getApplication()->logSystem("Access to this surface is being refused because the surface memory is gone. The "
				"DirectDrawSurface object representing this surface should have Restore called on it.\n");
			break;
		case DDERR_WASSTILLDRAWING:
			getApplication()->logSystem("Informs DirectDraw that the previous Blt which is transfering information to or "
				"from this Surface is incomplete.\n");
			break;
		}
		m_buffer.m_pitch = 0;
		m_buffer.m_pixels = NULL;
		return NULL;
	}

	// 0x4CD120
	void DDrawDisplay::shutdown()
	{
		if (m_directDraw != NULL)
			cleanup();
	}

	// 0x4CD130
	void DDrawDisplay::onMinimize()
	{
		if (m_directDraw != NULL)
			cleanup();
		CloseWindow(m_hwnd);
	}

	// 0x4CD1D0
	bool DDrawDisplay::createDirectDraw()
	{
		if (loadDirectDraw())
		{
			IDirectDraw* directDraw;
			m_result = s_directDrawCreate(NULL, &directDraw, NULL);
			if (SUCCEEDED(m_result))
			{
				m_result = directDraw->QueryInterface(s_iidDirectDraw2, (LPVOID*)&m_directDraw);
				if (SUCCEEDED(m_result))
				{
					directDraw->Release();
					getApplication()->logSystem("using primary DirectDraw device.\n");
					return true;
				}
				m_errorMessage = "IDirectDraw::QueryInterface : ";
				m_errorMessage += getErrorString();
				directDraw->Release();
				return false;
			}
			m_errorMessage = "DirectDrawCreate :";
			m_errorMessage += getErrorString();
			return false;
		}
		m_errorMessage = "could not load DirectDraw DLL";
		return false;
	}

	// 0x4CD2E0
	IDirectDrawSurface* DDrawDisplay::createSurface(DDSURFACEDESC* desc)
	{
		IDirectDrawSurface* surface;
		m_result = m_directDraw->CreateSurface(desc, &surface, NULL);
		if (SUCCEEDED(m_result))
			return surface;
		m_errorMessage = "IDirectDraw::CreateSurface :";
		m_errorMessage += getErrorString();
		return NULL;
	}

	// 0x4CD350
	bool DDrawDisplay::createSurfaces(bool windowed)
	{
		if (m_backSurface != NULL)
		{
			m_backSurface->Release();
			m_backSurface = NULL;
		}
		if (m_primarySurface != NULL)
		{
			m_primarySurface->Release();
			m_primarySurface = NULL;
		}
		DDSURFACEDESC desc;
		zeroStruct(desc);
		desc.dwSize = sizeof(desc);
		desc.dwFlags = DDSD_CAPS;
		desc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
		m_primarySurface = createSurface(&desc);
		if (m_primarySurface != NULL)
		{
			if (windowed)
			{
				IDirectDrawClipper* clipper;	// never released
				m_result = m_directDraw->CreateClipper(0, &clipper, NULL);
				if (SUCCEEDED(m_result))
				{
					m_result = clipper->SetHWnd(0, m_hwnd);
					if (FAILED(m_result))
						getApplication()->logSystem("DDrawDisplay::createSurfaces - clipper->SetHwnd - %s\n", getErrorString());
					m_result = m_primarySurface->SetClipper(clipper);
					if (FAILED(m_result))
						getApplication()->logSystem("DDrawDisplay::createSurfaces - primarySurface->SetClipper - %s\n",
							getErrorString());
				}
				else
				{
					m_errorMessage = "CreateClipper : ";
					m_errorMessage += getErrorString();
				}
			}
			if (SUCCEEDED(m_result))
			{
				desc.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
				desc.dwWidth = m_width;
				desc.dwHeight = m_height;
				desc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
				desc.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
				desc.ddpfPixelFormat.dwFlags = DDPF_RGB;
				desc.ddpfPixelFormat.dwRGBBitCount = 32;
				desc.ddpfPixelFormat.dwRBitMask = 0xFF0000;
				desc.ddpfPixelFormat.dwGBitMask = 0xFF00;
				desc.ddpfPixelFormat.dwBBitMask = 0xFF;
				m_backSurface = createSurface(&desc);
				if (m_backSurface != NULL)
				{
					DDPIXELFORMAT format;
					zeroStruct(format);
					format.dwSize = sizeof(format);
					m_result = m_primarySurface->GetPixelFormat(&format);
					m_useGdiBlt = !samePixelFormat(format, desc.ddpfPixelFormat);
					if (m_useGdiBlt)
						getApplication()->logSystem("Incompatible primary surface.  Forcing GDI blt.\n");
					else
						getApplication()->logSystem("Compatible primary surface.  Using ddraw blt.\n");
					return true;
				}
				m_errorMessage.insert(0, "(back surface) ");
			}
			m_primarySurface->Release();
			m_primarySurface = NULL;
			return false;
		}
		m_errorMessage.insert(0, "(front surface) ");
		return false;
	}

	// 0x4CD670
	DDrawDisplay::~DDrawDisplay()
	{
		if (m_directDraw != NULL)
			cleanup();
		if (s_ddrawDll != NULL)
		{
			FreeLibrary(s_ddrawDll);
			s_ddrawDll = NULL;
			s_directDrawCreate = NULL;
		}
	}

	// 0x4CD7A0
	bool DDrawDisplay::setupWindow()
	{
		if (createDirectDraw())
		{
			m_result = m_directDraw->SetCooperativeLevel(m_hwnd, DDSCL_NORMAL);
			if (SUCCEEDED(m_result))
			{
				createSurfaces(true);
				if (m_backSurface != NULL)
				{
					DDSURFACEDESC desc;
					zeroStruct(desc);
					desc.dwSize = sizeof(desc);
					m_result = m_directDraw->GetDisplayMode(&desc);
					m_fullScreen = false;
					return true;
				}
			}
			else
			{
				m_errorMessage = "SetCooperativeLevel : ";
				m_errorMessage += getErrorString();
			}
			m_directDraw->Release();
			m_directDraw = NULL;
		}
		m_errorMessage.insert(0, "setupWindow : ");
		return false;
	}

	// 0x4CD8E0
	bool DDrawDisplay::setupFullScreen()
	{
		if (createDirectDraw())
		{
			SetWindowLong(m_hwnd, GWL_STYLE, WS_POPUP);
			SetWindowLong(m_hwnd, GWL_EXSTYLE, WS_EX_TOPMOST);
			SetMenu(m_hwnd, NULL);
			ShowWindow(m_hwnd, SW_SHOW);
			m_bitDepth = 0;
			m_result = m_directDraw->EnumDisplayModes(0, NULL, this, enumModesCallback);
			if (FAILED(m_result))
			{
				// the original gets the error text before the application
				const char* error = getErrorString();
				getApplication()->logSystem(error);
			}
			m_result = m_directDraw->SetCooperativeLevel(m_hwnd, DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN);
			if (SUCCEEDED(m_result))
			{
				m_result = m_directDraw->SetDisplayMode(m_width, m_height, m_bitDepth, 0, 0);
				if (SUCCEEDED(m_result))
				{
					createSurfaces(false);
					if (m_backSurface != NULL)
					{
						m_fullScreen = true;
						return true;
					}
				}
				else
				{
					m_errorMessage = "IDirectDraw::SetDisplayMode : ";
					m_errorMessage += getErrorString();
				}
				m_directDraw->SetCooperativeLevel(m_hwnd, DDSCL_NORMAL);
			}
			else
			{
				m_errorMessage = "IDirectDraw::SetCooperativeLevel : ";
				m_errorMessage += getErrorString();
			}
			SetWindowLong(m_hwnd, GWL_STYLE, m_windowStyle);
			SetWindowLong(m_hwnd, GWL_EXSTYLE, m_windowExStyle);
			MoveWindow(m_hwnd, m_windowRect.left, m_windowRect.top, m_windowRect.right - m_windowRect.left,
				m_windowRect.bottom - m_windowRect.top, TRUE);
			m_directDraw->Release();
			m_directDraw = NULL;
		}
		m_errorMessage.insert(0, "setupFullScreen : ");
		return false;
	}

	// 0x4CDC20
	void DDrawDisplay::setup(bool fullScreen)
	{
		if (m_directDraw != NULL && fullScreen != m_fullScreen)
			cleanup();
		if (!IsWindowVisible(m_hwnd))
			ShowWindow(m_hwnd, SW_SHOW);
		if (m_directDraw == NULL)
		{
			if (fullScreen && setupFullScreen())
				return;
			if (!setupWindow())
				throw Exception(m_errorMessage);
		}
	}

	// 0x4CDCB0
	void DDrawDisplay::setFullScreen(bool fullScreen)
	{
		if (m_directDraw != NULL)
			setup(fullScreen);
		else
			m_fullScreen = fullScreen;
	}

	// 0x4CDCD0
	void DDrawDisplay::restoreSurfaces()
	{
		if (m_directDraw != NULL && (m_active || !m_fullScreen))
		{
			if (m_primarySurface != NULL && m_primarySurface->IsLost() != DD_OK)
				cleanup();
			if (m_primarySurface == NULL)
				setup(m_fullScreen);
		}
	}

	// 0x4CDD20
	void DDrawDisplay::setSize(int width, int height)
	{
		m_width = width;
		m_height = height;
		setup(m_fullScreen);
	}

	// 0x4CDD50
	void DDrawDisplay::onMaximize()
	{
		if (IsWindowEnabled(m_hwnd))
		{
			if (IsIconic(m_hwnd))
				OpenIcon(m_hwnd);
			setup(true);
		}
	}

	// 0x4CDD90
	void DDrawDisplay::onRestore()
	{
		if (IsWindowEnabled(m_hwnd))
		{
			if (IsIconic(m_hwnd))
				OpenIcon(m_hwnd);
			setup(m_fullScreen);
		}
	}

	// 0x4CDDD0
	DDrawDisplay::DDrawDisplay(Win32Application* app, HWND hwnd)
		: m_hwnd(hwnd), m_directDraw(NULL), m_primarySurface(NULL), m_backSurface(NULL), m_useGdiBlt(false),
		m_active(false), m_result(DD_OK)
	{
		m_buffer.m_pixels = NULL;
		app->m_maximizeSignal.connect(this, &DDrawDisplay::onMaximize);
		app->m_minimizeSignal.connect(this, &DDrawDisplay::onMinimize);
		app->m_restoreSignal.connect(this, &DDrawDisplay::onRestore);
		app->m_activateSignal.connect(this, &DDrawDisplay::onActivate);
		m_windowStyle = GetWindowLong(hwnd, GWL_STYLE);
		m_windowExStyle = GetWindowLong(hwnd, GWL_EXSTYLE);
		GetWindowRect(hwnd, &m_windowRect);
		m_windowStyle |= WS_VISIBLE;
	}
}
