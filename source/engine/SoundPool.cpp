#include "SoundPool.h"

#include "SimpleSound.h"
#include "SoundHandle.h"

namespace engine
{
	// 0x490AB0: nothing is played when every handle is busy
	void SoundPool::play(float volume, float pitch)
	{
		SoundHandle* handle = getHandle(false);
		if (handle)
		{
			handle->setVolume(volume * m_volume);
			handle->setPitch(pitch * m_pitch);
			handle->play();
		}
	}

	// 0x490AF0
	int SoundPool::getPlayingCount()
	{
		int count = 0;
		for (std::vector<SoundHandle*>::iterator it = m_handles.begin(); it != m_handles.end(); ++it)
		{
			if ((*it)->isPlaying())
				count++;
		}
		return count;
	}

	// 0x490B20
	SoundPool::~SoundPool()
	{
		for (unsigned int i = 0; i < m_handles.size(); i++)
			m_handles[i]->destroy();
		m_handles.clear();
	}

	// 0x490E10: count handles made up front, each in the given volume category
	SoundPool::SoundPool(SimpleSound* soundSystem, const char* name, int count, float volume, float pitch, int channel)
	{
		m_name = name;
		m_volume = volume;
		m_soundSystem = soundSystem;
		m_pitch = pitch;
		for (int i = 0; i < count; i++)
		{
			SoundHandle* handle = m_soundSystem->createHandle(m_name, true, 0);
			handle->setCategory(channel);
			m_handles.push_back(handle);
		}
	}

	// 0x490F00: a handle made here keeps category 0
	SoundHandle* SoundPool::getHandle(bool create)
	{
		for (std::vector<SoundHandle*>::iterator it = m_handles.begin(); it != m_handles.end(); ++it)
		{
			SoundHandle* handle = *it;
			if (!handle->isPlaying())
			{
				handle->setVolume(m_volume);
				handle->setPitch(m_pitch);
				return handle;
			}
		}
		if (create)
		{
			SoundHandle* handle = m_soundSystem->createHandle(m_name, true, 0);
			m_handles.push_back(handle);
			return handle;
		}
		return 0;
	}

	// 0x497470 (folded)
	void SoundPool::setVolume(float volume)
	{
		m_volume = volume;
	}

	// 0x497490 (folded)
	void SoundPool::setPitch(float pitch)
	{
		m_pitch = pitch;
	}
}
