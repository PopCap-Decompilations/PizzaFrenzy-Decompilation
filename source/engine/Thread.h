// engine::Thread: a Java-style thread holding its engine::Runnable target (start() is Win32Thread's).
#pragma once

#include "Object.h"
#include "RefPtr.h"
#include "Runnable.h"

namespace engine
{
	// Abstract (made by the application's createThread as a Win32Thread, which also implements the Runnable
	// interface: its run() runs m_target). Layout: Object +0x00, Runnable +0x0C (vfptr, vbptr), m_target +0x14, then
	// the vtordisp and Interface (0x20 bytes).
	class Thread : public Object, public Runnable
	{
	public:
		explicit Thread(Runnable* target);
		virtual ~Thread();

		virtual bool start() = 0;				// slot 1: false when already started or CreateThread fails

		RefPtr<Runnable> m_target;				// +0x14 the Runnable given to the constructor
	};
}
