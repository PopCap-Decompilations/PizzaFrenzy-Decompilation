#include "StaticSoundHandleDX.h"

#include <string.h>

#include <dsound.h>

#include "MemoryTracker.h"
#include "SimpleSoundDX.h"
#include "SoundDataSource.h"

namespace engine
{
	// 0x497180
	void StaticSoundHandleDX::play()
	{
		if (m_isPlaying)
		{
			debugLog(3, "ignoring play (%s) because m_isPlaying == true", m_name);
			return;
		}
		if (m_buffer)
		{
			if (m_fadeInTime > 0.0f)
			{
				m_fadeTimer = m_fadeInTime;
				m_fadeState = 0;
				applyVolume(0.0f);
			}
			else
			{
				m_fadeState = 1;
				applyVolume(m_volume);
			}
			m_buffer->Play(0, 0, m_looping);
			m_isPlaying = true;
		}
	}

	// 0x497200
	void StaticSoundHandleDX::rewind()
	{
		if (m_isPlaying)
		{
			m_buffer->Stop();
			m_isPlaying = false;
			if (m_listener)
				m_listener->onSoundStopped(this);
		}
		m_playPosition = 0;
		m_sourcePosition = 0;
		m_buffer->SetCurrentPosition(0);
	}

	// 0x451610 (folded)
	bool StaticSoundHandleDX::isStatic()
	{
		return true;
	}

	// 0x4972E0
	bool StaticSoundHandleDX::create(const char* soundName, bool softwareBuffer)
	{
		SoundDataSource* source = SimpleSoundDX::s_instance->findSource(soundName);
		if (source == 0)
			return false;
		source->reset();
		if (source->addHandle(this) < 0)
			return false;
		m_source = source;

		DSBUFFERDESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.dwSize = sizeof(DSBUFFERDESC);
		desc.dwFlags = DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLPOSITIONNOTIFY
			| DSBCAPS_GETCURRENTPOSITION2;
		if (softwareBuffer)
			desc.dwFlags = DSBCAPS_LOCSOFTWARE | DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME |
				DSBCAPS_CTRLPOSITIONNOTIFY | DSBCAPS_GETCURRENTPOSITION2;
		const WAVEFORMATEX* format = source->getFormat();
		desc.dwBufferBytes = source->getLength();
		desc.lpwfxFormat = (LPWAVEFORMATEX)format;
		m_bytesPerSecond = format->nAvgBytesPerSec;
		if (FAILED(SimpleSoundDX::s_instance->m_directSound->CreateSoundBuffer(&desc, &m_buffer, 0)))
		{
			m_source->removeHandle(this);
			m_source = 0;
			return false;
		}

		size_t length = strlen(soundName);
		MemoryTracker::setSource(".\\StaticSoundHandleDX.cpp", 106);
		m_name = new char[length + 1];
		strncpy(m_name, soundName, length + 1);
		m_bufferBytes = desc.dwBufferBytes;
		m_halfBufferBytes = m_bufferBytes / 2;
		m_sourcePosition = 0;
		m_frequency = m_source->getFormat()->nSamplesPerSec;
		m_sourcePosition = 0;
		fillBuffer(0, desc.dwBufferBytes);
		m_nextFillOffset = 0;
		m_source->removeHandle(this);
		m_source = 0;
		return true;
	}

	// 0x497290
	void StaticSoundHandleDX::updateStatus()
	{
		DWORD status;
		m_buffer->GetStatus(&status);
		m_isPlaying = (status & DSBSTATUS_PLAYING) != 0;
	}

	// 0x497240
	void StaticSoundHandleDX::updateData()
	{
		DWORD playCursor;
		DWORD writeCursor;
		m_buffer->GetCurrentPosition(&playCursor, &writeCursor);
		if (m_listener && playCursor < m_playPosition)
			m_listener->onSoundLooped(this);
		m_playPosition = playCursor;
	}

	// 0x4972B0
	int StaticSoundHandleDX::getFadeOutStart()
	{
		return (int)(m_bufferBytes - m_bytesPerSecond * m_fadeOutTime);
	}
}
