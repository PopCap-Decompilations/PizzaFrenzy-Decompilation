#include "Win32Thread.h"

namespace engine
{
	// 0x4C0A30
	DWORD WINAPI Win32Thread::threadProc(void* parameter)
	{
		return static_cast<Win32Thread*>(parameter)->run();
	}

	// 0x4C0A40
	Win32Thread::Win32Thread(Runnable* runnable)
		: Thread(runnable), m_thread(0)
	{
	}

	// 0x4C0AE0
	bool Win32Thread::start()
	{
		if (m_thread == 0)
		{
			m_thread = CreateThread(0, 0, threadProc, this, CREATE_SUSPENDED, &m_threadId);
			if (m_thread != 0)
			{
				addRef();
				ResumeThread(m_thread);
				return true;
			}
		}
		return false;
	}

	// 0x4C0B30
	int Win32Thread::run()
	{
		int exitCode;
		if (m_target)
		{
			exitCode = m_target->run();
		}
		else
		{
			exitCode = 0;
		}
		m_exitCode = exitCode;
		release();
		return exitCode;
	}
}
