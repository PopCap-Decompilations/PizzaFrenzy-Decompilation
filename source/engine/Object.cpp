#include "Object.h"

#include <windows.h>

namespace engine
{
	// 0x4616C0
	Object::Object()
		: m_refCount(0)
	{
	}

	// 0x461680
	Object::~Object()
	{
	}

	// 0x461710
	void Object::addRef()
	{
		InterlockedIncrement(&m_refCount);
	}

	// 0x461720
	void Object::release()
	{
		if (InterlockedDecrement(&m_refCount) <= 0)
			delete this;
	}

	// 0x4616B0
	long Object::getRefCount()
	{
		return m_refCount;
	}
}
