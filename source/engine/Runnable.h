// engine::Runnable: the Java-style body of an engine::Thread (run() returns the thread's exit code).
#pragma once

#include "Interface.h"

namespace engine
{
	// Implemented by the high-score table (global scores downloaded on a thread) and by Win32Thread; engine::Thread
	// holds its target in a RefPtr. One pure slot and no destructor: its vtable {_purecall} was merged by /OPT:ICF
	// with Animator's, PixelFilter's and ActionListener's (0x503A70). {vfptr, vbptr} before the virtual Interface.
	class Runnable : public virtual Interface
	{
	public:
		virtual int run() = 0;					// slot 0: the thread body; its result is the thread's exit code
	};
}
