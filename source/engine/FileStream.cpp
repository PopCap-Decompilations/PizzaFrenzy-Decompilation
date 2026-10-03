#include "FileStream.h"

#include <string.h>

#include "MemoryTracker.h"
#include "OggDecoder.h"
#include "SimpleSoundDX.h"

namespace engine
{
	// 0x497B50
	FileStream::FileStream()
		: m_decoder(NULL)
	{
	}

	// 0x497B70
	bool FileStream::open(SoundStream* stream)
	{
		if (m_decoder)
		{
			m_decoder->close();
			delete m_decoder;
			m_decoder = NULL;
		}
		SoundDecoder* decoder = SimpleSoundDX::createDecoder(stream);
		if (!decoder)
			return false;
		m_decoder = decoder;
		m_format.cbSize = 0;
		m_format.wFormatTag = WAVE_FORMAT_PCM;
		m_format.wBitsPerSample = decoder->getBitsPerSample();
		m_format.nSamplesPerSec = decoder->getSampleRate();
		m_format.nChannels = decoder->getChannels();
		m_format.nBlockAlign = m_format.wBitsPerSample * m_format.nChannels / 8;
		m_format.nAvgBytesPerSec = m_format.nBlockAlign * m_format.nSamplesPerSec;
		return true;
	}

	// 0x497C10
	bool FileStream::getBytes(int bytes, void* dest, bool loop, int* position)
	{
		bool reachedEnd = false;
		int total = 0;
		debugLog(2, "FileStream - getBytes(%d)", bytes);
		while (bytes > 0)
		{
			debugLog(2, "FileStream calls read as (%ld, %d)", (char*)dest + total, bytes);
			int count = m_decoder->read((char*)dest + total, bytes);
			debugLog(2, "\tFileStream - read %d bytes from source", count);
			total += count;
			if (count < bytes)
			{
				if (loop)
				{
					m_decoder->rewind();
					reachedEnd = true;
					debugLog(2, "\tFileStream - reset data source to beginning");
				}
				else
				{
					memset((char*)dest + total, 0, bytes - count);
					return true;
				}
			}
			bytes -= count;
		}
		debugLog(2, "FileStream - gotBytes(%d)", total);
		return reachedEnd;
	}

	// 0x497CE0
	void FileStream::reset()
	{
		m_decoder->rewind();
	}

	// 0x497CF0
	bool FileStream::close()
	{
		m_decoder->close();
		delete m_decoder;
		m_decoder = NULL;
		memset(&m_format, 0, sizeof(m_format));
		return true;
	}

	// 0x497D30
	int FileStream::getLength()
	{
		return m_decoder->getDecodedSize();
	}

	// 0x479850 (folded)
	const WAVEFORMATEX* FileStream::getFormat()
	{
		return &m_format;
	}

	// 0x497D40
	float FileStream::getDuration()
	{
		return (float)m_decoder->getDecodedSize() / m_format.nAvgBytesPerSec;
	}

	// 0x451610 (folded)
	bool FileStream::isStreaming()
	{
		return true;
	}

	// 0x497D70
	int FileStream::addHandle(SoundHandle* handle)
	{
		if (getHandleCount() > 0)
			return -1;
		return SoundDataSource::addHandle(handle);
	}

	// 0x497DA0
	FileStream::~FileStream()
	{
		close();
	}
}
