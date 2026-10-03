// engine::Win32Thread: engine::Thread over CreateThread (made by Win32Application::createThread). start() keeps a
// reference to the thread object until the thread body has run the Runnable; the thread handle is never closed.
#pragma once

#include <windows.h>

#include "Thread.h"

namespace engine
{
	class Runnable;

	// Implicit destructor (4C0AD0 only jumps to Thread's).
	class Win32Thread : public Thread
	{
	public:
		Win32Thread(Runnable* runnable);

		// Thread
		virtual bool start();

		// Runnable (the thread body, which threadProc calls through the Runnable interface at +0xC: runs m_target)
		virtual int run();

		static DWORD WINAPI threadProc(void* parameter);

		HANDLE m_thread;						// +0x18 CreateThread handle (0 until started); never closed
		DWORD m_threadId;						// +0x1C not initialised by the constructor
		int m_exitCode;							// +0x20 result of the target's run(); not initialised by the constructor
	};
}
