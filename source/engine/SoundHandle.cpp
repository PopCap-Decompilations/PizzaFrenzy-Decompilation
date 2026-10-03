#include "SoundHandle.h"

#include <stddef.h>

namespace engine
{
	// 0x497450
	SoundHandle::SoundHandle()
		: m_listener(NULL), m_category(0)
	{
	}

	// 0x497470 (folded)
	void SoundHandle::setListener(SoundListener* listener)
	{
		m_listener = listener;
	}

	// 0x497480
	void SoundHandle::removeListener(SoundListener* listener)
	{
		m_listener = NULL;
	}

	// 0x497490 (folded)
	void SoundHandle::setCategory(int category)
	{
		m_category = category;
	}

	// 0x4974A0
	int SoundHandle::getCategory(int unused)
	{
		return m_category;
	}
}
