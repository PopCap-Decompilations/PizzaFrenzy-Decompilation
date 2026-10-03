// engine::Object: base of every engine and game class. Layout: vfptr +0x00, vbptr +0x04, m_refCount +0x08,
// then the vtordisp (+0x0C) and the Interface subobject (+0x10) that the compiler places after the class's
// own members (in derived classes too).
#pragma once

#include "Interface.h"

namespace engine
{
	class Object : public virtual Interface
	{
	public:
		Object();
		virtual ~Object();

		virtual void addRef();
		virtual void release();
		virtual long getRefCount();

		long m_refCount;						// +0x08
	};
}
