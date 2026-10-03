#include "Thread.h"

namespace engine
{
	// 0x4D11A0
	Thread::~Thread()
	{
	}

	// 0x4D1230
	Thread::Thread(Runnable* target)
	{
		m_target = target;
	}
}
