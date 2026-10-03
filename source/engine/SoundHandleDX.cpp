#include "SoundHandleDX.h"

#include <math.h>
#include <string.h>

#include <dsound.h>

#include "MemoryTracker.h"
#include "SimpleSoundDX.h"
#include "SoundDataSource.h"

namespace engine
{
	// 0x4966B0
	void SoundHandleDX::play()
	{
		if (m_dataEnded)
			stop();
		if (!m_isPlaying && m_buffer)
		{
			if (m_fadeInTime > 0.0f)
			{
				m_fadeTimer = m_fadeInTime;
				m_fadeState = 0;
				applyVolume(0.0f);
			}
			else
			{
				applyVolume(m_volume);
				m_fadeState = 1;
			}
			m_buffer->Play(0, 0, DSBPLAY_LOOPING);
			debugLog(3, "Play Sound\n");
			m_isPlaying = true;
		}
	}

	// 0x496730
	void SoundHandleDX::pause()
	{
		if (m_buffer)
		{
			m_buffer->Stop();
			m_isPlaying = false;
		}
	}

	// 0x496750
	void SoundHandleDX::stop()
	{
		if (m_fadeOutTime > 0.0f)
		{
			m_fadeState = 2;
			m_fadeTimer = m_fadeOutTime;
		}
		else if (m_buffer)
		{
			m_buffer->Stop();
			m_isPlaying = false;
			rewind();
			if (m_listener)
				m_listener->onSoundStopped(this);
		}
	}

	// 0x4967A0
	void SoundHandleDX::rewind()
	{
		if (m_isPlaying)
		{
			m_buffer->Stop();
			m_isPlaying = false;
			if (m_listener)
				m_listener->onSoundStopped(this);
		}
		if (m_buffer)
			m_buffer->SetCurrentPosition(0);
		if (m_source)
			m_source->reset();
		m_sourcePosition = 0;
		m_nextFillOffset = 0;
		m_dataEnded = false;
		m_playPosition = 0;
		m_lastPlayCursor = 0;
		fillBuffer(0, m_halfBufferBytes);
	}

	// 0x496800
	void SoundHandleDX::setPitch(float pitch)
	{
		if (pitch < 0.0f)
			return;
		m_pitch = pitch;
		if (m_buffer)
		{
			DWORD frequency = (DWORD)(m_frequency * pitch);
			// DSBFREQUENCY_MIN and the DirectX 9 DSBFREQUENCY_MAX
			if (frequency < 100)
				frequency = 100;
			else if (frequency > 200000)
				frequency = 200000;
			m_buffer->SetFrequency(frequency);
		}
	}

	// 0x450470 (folded)
	float SoundHandleDX::getPitch()
	{
		return m_pitch;
	}

	// 0x496870
	void SoundHandleDX::setPan(float pan)
	{
		m_pan = pan;
		if (pan < -1.0f)
			m_pan = -1.0f;
		if (m_pan > 1.0f)
			m_pan = 1.0f;
		if (m_buffer)
			m_buffer->SetPan((LONG)(m_pan * -10000.0f));
	}

	// 0x450490 (folded)
	float SoundHandleDX::getPan()
	{
		return m_pan;
	}

	// 0x4968D0
	void SoundHandleDX::setVolume(float volume)
	{
		m_volume = volume;
		if (volume < 0.0f)
			m_volume = 0.0f;
		m_appliedVolume = -1.0f;
		if (m_volume > 1.0f)
			m_volume = 1.0f;
	}

	// 0x450480 (folded)
	float SoundHandleDX::getVolume()
	{
		return m_volume;
	}

	// 0x496910
	void SoundHandleDX::setLooping(bool looping)
	{
		m_looping = looping;
	}

	// 0x4504A0 (folded)
	bool SoundHandleDX::isLooping()
	{
		return m_looping;
	}

	// 0x496920
	SoundHandleDX::SoundHandleDX()
	{
		m_source = NULL;
		m_buffer = NULL;
		m_pan = 0.0f;
		m_looping = false;
		m_isPlaying = false;
		m_dataEnded = false;
		m_endOffset = 0;
		m_frequency = 0;
		m_name = NULL;
		m_fadeInTime = 0.0f;
		m_fadeOutTime = 0.0f;
		m_pitch = 1.0f;
		m_volume = 1.0f;
		m_fadeTimer = -1.0f;
		m_fadeState = 1;
	}

	// 0x496970
	SoundHandleDX::~SoundHandleDX()
	{
		delete[] m_name;
		m_name = NULL;
	}

	// 0x496990
	void SoundHandleDX::zeroBuffer(int offset, int bytes)
	{
		void* ptr1 = NULL;
		void* ptr2 = NULL;
		DWORD bytes1 = 0;
		DWORD bytes2 = 0;
		m_buffer->Lock(offset, bytes, &ptr1, &bytes1, &ptr2, &bytes2, 0);
		debugLog(3, "zeroBuffer called - waiting for stop (%ld, %ld, %ld, %ld)", ptr1, bytes1, ptr2, bytes2);
		memset(ptr1, 0, bytes1);
		memset(ptr2, 0, bytes2);
		m_buffer->Unlock(ptr1, bytes1, ptr2, bytes2);
	}

	// 0x496A50
	void SoundHandleDX::fillBuffer(int offset, int bytes)
	{
		debugLog(3, " ");
		debugLog(3, "\tfillBuffer for sound handle (%s): offset:%d, bytes:%d", m_name, offset, bytes);
		bool reachedEnd = false;
		void* ptr1 = NULL;
		void* ptr2 = NULL;
		DWORD bytes1 = 0;
		DWORD bytes2 = 0;
		if (m_dataEnded)
		{
			zeroBuffer(offset, bytes);
			return;
		}
		m_buffer->Lock(offset, bytes, &ptr1, &bytes1, &ptr2, &bytes2, 0);
		if (bytes1 > 0)
			reachedEnd = m_source->getBytes(bytes1, ptr1, m_looping, &m_sourcePosition);
		if (!m_looping && reachedEnd)
		{
			debugLog(3, "LAST FILL BUFFER CALL W/ REAL DATA");
			m_dataEnded = true;
			m_endOffset = offset;
			memset(ptr2, 0, bytes2);
		}
		else if (bytes2 > 0)
		{
			reachedEnd = m_source->getBytes(bytes2, ptr2, m_looping, &m_sourcePosition);
			if (!m_looping && reachedEnd)
			{
				m_dataEnded = true;
				m_endOffset = offset;
			}
		}
		if (m_listener && m_looping && reachedEnd)
			m_listener->onSoundLooped(this);
		m_buffer->Unlock(ptr1, bytes1, ptr2, bytes2);
	}

	// 0x496BA0
	void SoundHandleDX::update(int elapsedMs)
	{
		if (m_buffer && m_isPlaying)
		{
			updateData();
			updateFade(elapsedMs);
			updateStatus();
		}
	}

	// 0x4D0470 (folded)
	void SoundHandleDX::updateStatus()
	{
	}

	// 0x496BD0
	void SoundHandleDX::updateData()
	{
		DWORD playCursor;
		DWORD writeCursor;
		m_buffer->GetCurrentPosition(&playCursor, &writeCursor);
		if (playCursor > m_lastPlayCursor)
			m_playPosition += playCursor - m_lastPlayCursor;
		else
			m_playPosition += m_bufferBytes - m_lastPlayCursor + playCursor;
		if (m_playPosition > (DWORD)m_source->getLength())
			m_playPosition -= m_source->getLength();
		m_lastPlayCursor = playCursor;
		if (m_nextFillOffset == 0 && playCursor < (DWORD)m_halfBufferBytes)
		{
			if (m_dataEnded && m_endOffset == m_halfBufferBytes)
			{
				debugLog(2, "Stopping sound in updateData() (%s)", m_name);
				fillBuffer(m_halfBufferBytes, m_halfBufferBytes);
				stop();
			}
			else
			{
				fillBuffer(m_halfBufferBytes, m_halfBufferBytes);
				m_nextFillOffset = m_halfBufferBytes;
			}
		}
		else if (m_nextFillOffset == m_halfBufferBytes && playCursor > (DWORD)m_halfBufferBytes)
		{
			if (m_dataEnded && m_endOffset == 0)
			{
				fillBuffer(0, m_halfBufferBytes);
				debugLog(2, "Stopping sound in updateData() (%s)", m_name);
				stop();
			}
			else
			{
				fillBuffer(0, m_halfBufferBytes);
				m_nextFillOffset = 0;
			}
		}
	}

	// 0x496CE0
	void SoundHandleDX::updateFade(int elapsedMs)
	{
		float seconds = elapsedMs * 0.001f;
		float volume = m_volume;
		switch (m_fadeState)
		{
		case 0:
			m_fadeTimer -= seconds;
			if (m_fadeTimer < 0.0f)
			{
				m_fadeTimer = 0.0f;
				m_fadeState = 1;
			}
			volume = (m_fadeInTime - m_fadeTimer) / m_fadeInTime * m_volume;
			break;
		case 2:
		case 3:
			m_fadeTimer -= seconds;
			if (m_fadeTimer < 0.0f)
				m_fadeTimer = 0.0f;
			volume = (1.0f - (m_fadeOutTime - m_fadeTimer) / m_fadeOutTime) * m_volume;
			break;
		case 1:
			if (m_fadeOutTime > 0.0f && !m_looping && m_playPosition >= (DWORD)getFadeOutStart())
			{
				m_fadeTimer = m_fadeOutTime;
				m_fadeState = 2;
			}
			break;
		}
		applyVolume(volume);
		if (volume <= 0.0f && m_fadeState == 2 && m_buffer)
		{
			m_buffer->Stop();
			m_isPlaying = false;
			if (m_listener)
				m_listener->onSoundStopped(this);
			rewind();
		}
	}

	// 0x496E10
	bool SoundHandleDX::isPlaying()
	{
		return m_isPlaying;
	}

	// 0x496E20
	void SoundHandleDX::setFadeInTime(float seconds)
	{
		if (seconds >= 0.0f)
			m_fadeInTime = seconds;
		else
			m_fadeInTime = 0.0f;
	}

	// 0x496E50
	void SoundHandleDX::setFadeOutTime(float seconds)
	{
		if (seconds >= 0.0f)
			m_fadeOutTime = seconds;
		else
			m_fadeOutTime = 0.0f;
	}

	// 0x469DC0 (folded)
	const char* SoundHandleDX::getName()
	{
		return m_name;
	}

	// 0x4529D0 (folded)
	bool SoundHandleDX::isStatic()
	{
		return false;
	}

	// 0x496E80
	int SoundHandleDX::getFadeOutStart()
	{
		SoundDataSource* source = m_source;
		float length = (float)source->getLength();
		return (int)(length - source->getFormat()->nAvgBytesPerSec * m_fadeOutTime);
	}

	// 0x496ED0
	bool SoundHandleDX::create(const char* soundName, bool softwareBuffer)
	{
		if (m_source)
			return false;
		SoundDataSource* source = SimpleSoundDX::s_instance->findSource(soundName);
		if (!source)
			return false;
		source->reset();
		if (source->addHandle(this) < 0)
			return false;
		DSBUFFERDESC desc;
		memset(&desc, 0, sizeof(desc));
		desc.dwSize = sizeof(DSBUFFERDESC);
		desc.dwFlags = DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLPOSITIONNOTIFY | DSBCAPS_GETCURRENTPOSITION2;
		if (softwareBuffer)
			desc.dwFlags = DSBCAPS_LOCSOFTWARE | DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME |
				DSBCAPS_CTRLPOSITIONNOTIFY | DSBCAPS_GETCURRENTPOSITION2;
		const WAVEFORMATEX* format = source->getFormat();
		desc.dwBufferBytes = (DWORD)(format->nAvgBytesPerSec * 2.0f);
		desc.lpwfxFormat = (WAVEFORMATEX*)format;
		if (FAILED(SimpleSoundDX::s_instance->m_directSound->CreateSoundBuffer(&desc, &m_buffer, NULL)))
		{
			source->removeHandle(this);
			return false;
		}
		int length = strlen(soundName) + 1;
		MemoryTracker::setSource(".\\SoundHandleDX.cpp", 298);
		m_name = new char[length];
		strncpy(m_name, soundName, length);
		m_bufferBytes = desc.dwBufferBytes;
		m_source = source;
		m_halfBufferBytes = m_bufferBytes / 2;
		m_frequency = source->getFormat()->nSamplesPerSec;
		m_sourcePosition = 0;
		fillBuffer(0, desc.dwBufferBytes);
		m_nextFillOffset = m_halfBufferBytes;
		return true;
	}

	// 0x497070
	void SoundHandleDX::applyVolume(float volume)
	{
		SimpleSoundDX* sound = SimpleSoundDX::s_instance;
		volume *= sound->getCategoryVolume(m_category) * sound->getMasterVolume();
		if (volume != m_appliedVolume)
		{
			m_appliedVolume = volume;
			float attenuation;
			if (volume != 0.0f)
			{
				attenuation = 2000.0f * logf(volume);
				if (attenuation < -10000.0f)
					attenuation = -10000.0f;
			}
			else
			{
				attenuation = -10000.0f;
			}
			if (m_buffer)
			{
				if (m_buffer->SetVolume((LONG)attenuation) != DS_OK)
					debugLog(3, "Error setting volume\n");
			}
		}
	}

	// 0x497120
	void SoundHandleDX::destroy()
	{
		if (m_buffer)
		{
			SimpleSoundDX::s_instance->removeHandle(this);
			m_buffer->Stop();
			m_buffer->Release();
			m_buffer = NULL;
		}
		if (m_source)
			m_source->removeHandle(this);
		delete this;
	}
}
