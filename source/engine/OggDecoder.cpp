#include "OggDecoder.h"

#include "SoundStream.h"

namespace engine
{
	// 0x4974B0
	size_t OggDecoder::readCallback(void* ptr, size_t size, size_t count, void* datasource)
	{
		return ((SoundStream*)datasource)->read(ptr, count);
	}

	// 0x4974D0
	int OggDecoder::seekCallback(void* datasource, ogg_int64_t offset, int whence)
	{
		return ((SoundStream*)datasource)->seek((long)offset, whence);
	}

	// 0x4974F0
	int OggDecoder::closeCallback(void* datasource)
	{
		return ((SoundStream*)datasource)->close();
	}

	// 0x497500
	long OggDecoder::tellCallback(void* datasource)
	{
		return ((SoundStream*)datasource)->tell();
	}

	// 0x497510
	bool OggDecoder::open(SoundStream* stream)
	{
		close();
		if (stream == NULL)
			return false;
		ov_callbacks callbacks;
		callbacks.read_func = readCallback;
		callbacks.seek_func = seekCallback;
		callbacks.close_func = closeCallback;
		callbacks.tell_func = tellCallback;
		if (ov_open_callbacks(stream, &m_file, NULL, 0, callbacks) != 0)
		{
			stream->close();
			return false;
		}
		vorbis_info* info = ov_info(&m_file, -1);
		m_sampleRate = info->rate;
		m_channels = info->channels;
		ogg_int64_t rawTotal = ov_raw_total(&m_file, -1);
		ogg_int64_t pcmTotal = ov_pcm_total(&m_file, -1);
		double timeTotal = ov_time_total(&m_file, -1);
		m_rawSize = (int)rawTotal;
		m_decodedSize = m_channels * (int)pcmTotal * 2;
		stream->addRef();
		return true;
	}

	// 0x4975F0
	unsigned int OggDecoder::read(void* buffer, unsigned int bytes)
	{
		unsigned int total = 0;
		char* dest = (char*)buffer;
		int section;
		while (bytes > 0)
		{
			int count = ov_read(&m_file, dest, bytes, 0, 2, 1, &section);
			if (count <= 0)
				break;
			bytes -= count;
			dest += count;
			total += count;
		}
		return total;
	}

	// 0x497640
	bool OggDecoder::close()
	{
		SoundStream* stream = (SoundStream*)m_file.datasource;
		if (stream)
		{
			ov_clear(&m_file);
			stream->release();
		}
		return true;
	}

	// 0x497660
	int OggDecoder::getSampleRate()
	{
		return m_sampleRate;
	}

	// 0x497670
	int OggDecoder::getBitsPerSample()
	{
		return 16;
	}

	// 0x497680
	int OggDecoder::getRawSize()
	{
		return m_rawSize;
	}

	// 0x497690
	int OggDecoder::getDecodedSize()
	{
		return m_decodedSize;
	}

	// 0x4976A0
	int OggDecoder::getChannels()
	{
		return m_channels;
	}

	// 0x4976B0
	int OggDecoder::rewind()
	{
		return ov_raw_seek(&m_file, 0);
	}

	// 0x4976D0
	OggDecoder::OggDecoder()
	{
		m_file.datasource = NULL;
	}

	// only inlined, in the scalar deleting destructor (0x4976E0)
	OggDecoder::~OggDecoder()
	{
		close();
	}
}
