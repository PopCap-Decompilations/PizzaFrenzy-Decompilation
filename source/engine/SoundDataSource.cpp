#include "SoundDataSource.h"

#include "MemoryTracker.h"

namespace engine
{
	// 0x49B710
	bool SoundDataSource::free()
	{
		if (getHandleCount() > 0)
		{
			debugLog(1, "SoundDataSource free FAILED - still has %d open references", getHandleCount());
			return false;
		}
		delete this;
		return true;
	}

	// 0x49B750
	int SoundDataSource::getHandleCount()
	{
		return m_handles.size();
	}

	// 0x49B770
	SoundDataSource::~SoundDataSource()
	{
	}

	// 0x49B7A0
	int SoundDataSource::removeHandle(SoundHandle* handle)
	{
		std::vector<SoundHandle*>::iterator it = m_handles.begin();
		while (it != m_handles.end())
		{
			if (*it == handle)
				it = m_handles.erase(it);
			else
				++it;
		}
		return getHandleCount();
	}

	// 0x49B820
	SoundDataSource::SoundDataSource()
	{
	}

	// 0x49B840
	int SoundDataSource::addHandle(SoundHandle* handle)
	{
		m_handles.push_back(handle);
		return getHandleCount();
	}
}
