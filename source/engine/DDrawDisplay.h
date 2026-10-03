// engine::Display (a surface that can be shown on screen) and engine::DDrawDisplay, the DirectDraw display the
// application draws every frame into: windowed or full screen, with a GDI BitBlt fallback.
#pragma once

#include <string>

#include <windows.h>
#include <ddraw.h>

#include "Surface.h"
#include "sigslot.h"

namespace engine
{
	class Bitmap;
	class PixelFilter;
	class Win32Application;

	// Abstract: adds present(). Its implicit constructor is inlined in DDrawDisplay's, its implicit destructor (0x4CCA80)
	// only jumps to Surface's. As an image it cannot be copied or scaled: those Bitmap slots return NULL or do nothing.
	// Layout as Surface (0x3C bytes).
	class Display : public Surface
	{
	public:
		virtual void present() = 0;												// slot 5: shows the drawn frame

		// Bitmap
		virtual Bitmap* copyFilteredScaled(float scale, PixelFilter* filter);	// 0x4D1510 (folded): NULL
		virtual Bitmap* copyFiltered(PixelFilter* filter);						// 0x4D1500 (folded): NULL
		virtual Bitmap* copyScaled(float scaleX, float scaleY);				// 0x4D1510 (folded): NULL
		virtual Bitmap* copyScaledUniform(float scale);						// 0x4D1500 (folded): NULL
		virtual Bitmap* copy();													// 0x4D14F0 (folded): NULL
		virtual void scale(float scaleX, float scaleY);						// 0x4D1520 (folded): empty
	};

	// ddraw.dll is loaded at run time (loadDirectDraw) and used through IDirectDraw2 with IDirectDrawSurface (v1)
	// surfaces. Everything is drawn into a 32-bit system-memory back surface; present() copies it to the primary
	// surface (Blt to the client area when windowed, BltFast when full screen) or, when the primary's pixel format
	// differs or a blit failed, with GDI. Follows the application's activate/minimize/maximize/restore signals.
	// Layout: Display +0x00, has_slots<> +0x34 (vfptr, m_senders +0x38), members +0x44..+0x10B, then the vtordisp
	// and Interface (0x114 bytes).
	class DDrawDisplay : public Display, public sigslot::has_slots<>
	{
	public:
		DDrawDisplay(Win32Application* app, HWND hwnd);
		virtual ~DDrawDisplay();

		// Surface
		virtual PixelBuffer* lock();
		virtual PixelBuffer* lock(const IntRect& rect);
		virtual void unlock();

		// Display
		virtual void present();

		virtual const char* getErrorString();									// slot 6: "DirectX error (text not available)"

		// Bitmap
		virtual int getWidth() const;
		virtual int getHeight() const;

		bool isFullScreen() const;
		bool isReady() const;						// DirectDraw and a primary surface that is not lost
		void setFullScreen(bool fullScreen);		// setup(fullScreen) once DirectDraw exists, else only m_fullScreen
		void setSize(int width, int height);		// then setup(m_fullScreen)
		void setup(bool fullScreen);				// full screen falls back to windowed; both failing throws Exception(m_errorMessage)
		bool setupWindow();
		bool setupFullScreen();
		bool createDirectDraw();
		bool createSurfaces(bool windowed);
		IDirectDrawSurface* createSurface(DDSURFACEDESC* desc);
		void restoreSurfaces();						// WM_PAINT
		void cleanup();								// releases the surfaces and DirectDraw; leaves full screen
		void shutdown();

		// slots of the application's signals
		void onActivate(bool active);
		void onMinimize();
		void onMaximize();
		void onRestore();

		static bool loadDirectDraw();
		static bool samePixelFormat(const DDPIXELFORMAT& a, const DDPIXELFORMAT& b);
		static HRESULT WINAPI enumModesCallback(LPDDSURFACEDESC desc, LPVOID context);

		HWND m_hwnd;								// +0x44 the game window
		IDirectDraw2* m_directDraw;					// +0x48 NULL when released
		IDirectDrawSurface* m_primarySurface;		// +0x4C the front surface; a clipper for m_hwnd is attached when windowed
		IDirectDrawSurface* m_backSurface;			// +0x50 m_width x m_height 32-bit (FF0000/FF00/FF) system-memory surface everything is drawn into
		DDSURFACEDESC m_surfaceDesc;				// +0x54 filled by Lock (dwSize 0x6C); lPitch and lpSurface feed m_buffer
		bool m_fullScreen;							// +0xC0 not set by the constructor
													// +0xC1 (padding)
		int m_width;								// +0xC4 not set by the constructor
		int m_height;								// +0xC8 not set by the constructor
		int m_bitDepth;								// +0xCC highest RGB bit count EnumDisplayModes reports for m_width x m_height
		LONG m_windowStyle;							// +0xD0 GWL_STYLE saved by the constructor (WS_VISIBLE added), restored after full screen
		LONG m_windowExStyle;						// +0xD4 GWL_EXSTYLE saved by the constructor
		RECT m_windowRect;							// +0xD8 window rectangle saved by the constructor
		bool m_useGdiBlt;							// +0xE8 present() uses GetDC + BitBlt
		bool m_active;								// +0xE9 WM_ACTIVATE state; lost surfaces are restored only when active or windowed
													// +0xEA (padding)
		HRESULT m_result;							// +0xEC result of the last DirectDraw call
		std::string m_errorMessage;					// +0xF0 failure text of the setup functions
	};
}
