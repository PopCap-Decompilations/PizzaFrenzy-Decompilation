#include "WaveBufferSource.h"

#include <string.h>

#include "MemoryTracker.h"
#include "OggDecoder.h"
#include "SimpleSoundDX.h"

namespace engine
{
	// 0x497890
	WaveBufferSource::WaveBufferSource()
	{
		memset(&m_format, 0, sizeof(m_format));
		m_data = NULL;
		m_dataSize = 0;
	}

	// 0x4978C0
	bool WaveBufferSource::open(SoundStream* stream)
	{
		SoundDecoder* decoder = SimpleSoundDX::createDecoder(stream);
		if (!decoder)
			return false;
		m_format.cbSize = 0;
		m_format.wFormatTag = WAVE_FORMAT_PCM;
		m_format.wBitsPerSample = decoder->getBitsPerSample();
		m_format.nSamplesPerSec = decoder->getSampleRate();
		m_format.nChannels = decoder->getChannels();
		m_format.nBlockAlign = m_format.wBitsPerSample * m_format.nChannels / 8;
		m_format.nAvgBytesPerSec = m_format.nBlockAlign * m_format.nSamplesPerSec;
		m_dataSize = decoder->getDecodedSize();
		MemoryTracker::setSource(".\\WaveBufferSource.cpp", 59);
		m_data = new char[m_dataSize];
		if (!m_data)
		{
			m_dataSize = 0;
			decoder->close();
			delete decoder;
			return false;
		}
		memset(m_data, 0, m_dataSize);
		unsigned int bytesRead = decoder->read(m_data, m_dataSize);
		if (bytesRead <= 0)
		{
			decoder->close();
			delete decoder;
			delete[] m_data;
			m_data = NULL;
			m_dataSize = 0;
			return false;
		}
		m_dataSize = bytesRead;
		decoder->close();
		delete decoder;
		return true;
	}

	// 0x4979E0
	bool WaveBufferSource::getBytes(int bytes, void* dest, bool loop, int* position)
	{
		debugLog(3, "\tgetBytes: off=%d,bytes=%d, destPtr = %x", *position, bytes, dest);
		int offset = *position;
		int count = m_dataSize - offset;
		if (bytes < count)
			count = bytes;
		memcpy(dest, m_data + offset, count);
		offset += count;
		if (count < bytes)
		{
			int remaining = bytes - count;
			offset = 0;
			if (loop)
			{
				memcpy((char*)dest + count, m_data, remaining);
				offset = remaining;
			}
			else
			{
				memset((char*)dest + count, 0, remaining);
			}
		}
		*position = offset;
		return offset == 0 && bytes > 0;
	}

	// 0x4D0470 (folded)
	void WaveBufferSource::reset()
	{
	}

	// 0x497AD0
	bool WaveBufferSource::close()
	{
		if (m_data)
		{
			delete[] m_data;
			m_data = NULL;
		}
		m_dataSize = 0;
		return true;
	}

	// 0x4BE750 (folded)
	int WaveBufferSource::getLength()
	{
		return m_dataSize;
	}

	// 0x479850 (folded)
	const WAVEFORMATEX* WaveBufferSource::getFormat()
	{
		return &m_format;
	}

	// 0x497AB0
	float WaveBufferSource::getDuration()
	{
		return (float)m_dataSize / m_format.nAvgBytesPerSec;
	}

	// 0x4529D0 (folded)
	bool WaveBufferSource::isStreaming()
	{
		return false;
	}

	// 0x497B00
	WaveBufferSource::~WaveBufferSource()
	{
		close();
	}
}
