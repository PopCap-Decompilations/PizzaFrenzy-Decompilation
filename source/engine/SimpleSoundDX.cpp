#include "SimpleSoundDX.h"

#include <algorithm>

#include "FileStream.h"
#include "MemoryTracker.h"
#include "OggDecoder.h"
#include "SoundDataSource.h"
#include "SoundHandleDX.h"
#include "StaticSoundHandleDX.h"
#include "WaveBufferSource.h"

namespace engine
{
	SimpleSoundDX* SimpleSoundDX::s_instance = 0;		// 0x533288
	SimpleSoundDX* g_simpleSoundDX = 0;				// 0x53F72C

	// 0x48D8B0
	bool SimpleSoundDX::init()
	{
		if (FAILED(DirectSoundCreate(0, &m_directSound, 0)))
			return false;
		if (FAILED(m_directSound->SetCooperativeLevel(m_hwnd, DSSCL_PRIORITY)))
			return false;

		DSBUFFERDESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.dwSize = sizeof(DSBUFFERDESC);
		desc.dwFlags = DSBCAPS_PRIMARYBUFFER;
		if (FAILED(m_directSound->CreateSoundBuffer(&desc, &m_primaryBuffer, 0)))
			return false;

		WAVEFORMATEX format;
		ZeroMemory(&format, sizeof(format));
		format.wFormatTag = WAVE_FORMAT_PCM;
		format.nChannels = 2;
		format.nSamplesPerSec = 44100;
		format.wBitsPerSample = 16;
		format.nBlockAlign = 4;
		format.nAvgBytesPerSec = 176400;
		if (FAILED(m_primaryBuffer->SetFormat(&format)))
			return false;

		m_primaryBuffer->Play(0, 0, DSBPLAY_LOOPING);
		m_masterVolume = 1.0f;
		return true;
	}

	// 0x48D9D0
	void SimpleSoundDX::setMasterVolume(float volume)
	{
		m_masterVolume = volume;
		if (m_masterVolume < 0.0f)
			m_masterVolume = 0.0f;
		if (m_masterVolume > 1.0f)
			m_masterVolume = 1.0f;
	}

	// 0x48DA20
	OggDecoder* SimpleSoundDX::createDecoder(SoundStream* stream)
	{
		MemoryTracker::setSource(".\\SimpleSoundDX.cpp", 376);
		OggDecoder* decoder = new OggDecoder;
		if (!decoder->open(stream))
		{
			delete decoder;
			return 0;
		}
		return decoder;
	}

	// 0x48DAB0 (folded)
	int SimpleSoundDX::getSoundCount()
	{
		return (int)m_sources.size();
	}

	// 0x48DAC0
	void SimpleSoundDX::stopAll()
	{
		for (std::vector<SoundHandleDX*>::iterator it = m_handles.begin(); it != m_handles.end(); ++it)
			(*it)->stop();
	}

	// 0x48DAF0
	void SimpleSoundDX::update(int elapsedMs)
	{
		for (std::vector<SoundHandleDX*>::iterator it = m_handles.begin(); it != m_handles.end(); ++it)
			(*it)->update(elapsedMs);
	}

	// 0x48DB30
	void SimpleSoundDX::dumpHandles()
	{
		for (std::vector<SoundHandleDX*>::iterator it = m_handles.begin(); it != m_handles.end(); ++it)
		{
			SoundHandleDX* handle = *it;
			// the listing's log call is compiled out in this build; only the evaluation of its arguments remains
			handle->isLooping();
			handle->isPlaying();
			handle->isStatic();
			handle->getPan();
			handle->getPitch();
			handle->getVolume();
			handle->getName();
		}
	}

	// 0x48DBC0
	void SimpleSoundDX::removeHandle(SoundHandleDX* handle)
	{
		std::vector<SoundHandleDX*>::iterator it = std::find(m_handles.begin(), m_handles.end(), handle);
		if (it != m_handles.end())
			m_handles.erase(it);
	}

	// 0x48DC20
	void SimpleSoundDX::dumpSounds()
	{
		for (std::map<std::string, SoundDataSource*>::iterator it = m_sources.begin(); it != m_sources.end(); ++it)
		{
			std::string name = it->first;
			SoundDataSource* source = it->second;
			// the listing's log call is compiled out in this build; only the evaluation of its arguments remains
			source->isStreaming();
			source->getFormat();
			source->getFormat();
			source->getFormat();
			source->getDuration();
			source->getHandleCount();
		}
	}

	// 0x48DD80
	SoundDataSource* SimpleSoundDX::findSource(const char* name)
	{
		std::string key(name);
		std::map<std::string, SoundDataSource*>::iterator it = m_sources.find(key);
		if (it == m_sources.end())
			return 0;
		return it->second;
	}

	// 0x48E3A0
	bool SimpleSoundDX::hasSound(const char* name)
	{
		return findSource(name) != 0;
	}

	// 0x48E3C0
	void SimpleSoundDX::freeUnusedSounds()
	{
		std::map<std::string, SoundDataSource*>::iterator it = m_sources.begin();
		while (it != m_sources.end())
		{
			SoundDataSource* source = it->second;
			if (source->getHandleCount() == 0)
			{
				delete source;
				it = m_sources.erase(it);
			}
			else
			{
				++it;
			}
		}
	}

	// 0x48E430
	void SimpleSoundDX::unloadSound(const char* name)
	{
		std::string key(name);
		std::map<std::string, SoundDataSource*>::iterator it = m_sources.find(key);
		if (it != m_sources.end() && it->second->getHandleCount() == 0)
		{
			// the original's bug: the node is erased before its source is deleted and cleared through it
			m_sources.erase(it);
			delete it->second;
			it->second = 0;
		}
	}

	// 0x48E890
	SoundHandle* SimpleSoundDX::createHandle(const char* name, bool softwareBuffer, int category)
	{
		SoundDataSource* source = findSource(name);
		if (source == 0)
			return 0;
		SoundHandleDX* handle;
		if (source->getDuration() > 2.0f)
		{
			MemoryTracker::setSource(".\\SimpleSoundDX.cpp", 226);
			handle = new SoundHandleDX;
		}
		else
		{
			MemoryTracker::setSource(".\\SimpleSoundDX.cpp", 231);
			handle = new StaticSoundHandleDX;
		}
		if (!handle->create(name, softwareBuffer))
		{
			delete handle;
			return 0;
		}
		m_handles.push_back(handle);
		handle->setCategory(category);
		return handle;
	}

	// 0x48EC30
	SimpleSoundDX::SimpleSoundDX()
	{
		m_directSound = 0;
		m_primaryBuffer = 0;
		m_hwnd = 0;
		m_listener = 0;
	}

	// 0x48ECD0
	float SimpleSoundDX::getMasterVolume()
	{
		return m_masterVolume;
	}

	// 0x48ECE0
	bool SimpleSoundDX::loadStaticSound(const char* name, SoundStream* stream)
	{
		if (hasSound(name))
			return true;
		MemoryTracker::setSource(".\\SimpleSoundDX.cpp", 137);
		WaveBufferSource* source = new WaveBufferSource;
		if (!source->open(stream))
			return false;	// the source is leaked
		m_sources[name] = source;
		return findSource(name) != 0;
	}

	// 0x48EE20
	bool SimpleSoundDX::loadStreamingSound(const char* name, SoundStream* stream)
	{
		if (hasSound(name))
			return true;
		MemoryTracker::setSource(".\\SimpleSoundDX.cpp", 178);
		FileStream* source = new FileStream;
		if (!source->open(stream))
			return false;	// the source is leaked
		m_sources[name] = source;
		return findSource(name) != 0;
	}

	// 0x48EF60
	SimpleSoundDX* SimpleSoundDX::create(HWND hwnd)
	{
		MemoryTracker::setSource(".\\SimpleSoundDX.cpp", 31);
		SimpleSoundDX* sound = new SimpleSoundDX;
		sound->m_hwnd = hwnd;
		if (sound->init())
		{
			s_instance = sound;
			g_simpleSoundDX = sound;
			return sound;
		}
		// the object is leaked
		s_instance = 0;
		g_simpleSoundDX = 0;
		return 0;
	}
}
