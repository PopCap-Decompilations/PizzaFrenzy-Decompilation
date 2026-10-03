// engine::WaveBufferSource: a sound data source decoded completely into memory.
#pragma once

#include <windows.h>
#include <mmsystem.h>

#include "SoundDataSource.h"

namespace engine
{
	// Decodes the whole sound at open() (.\WaveBufferSource.cpp); made by SimpleSoundDX::loadStaticSound. Layout:
	// SoundDataSource +0x00, members +0x14 (0x30 bytes).
	class WaveBufferSource : public SoundDataSource
	{
	public:
		WaveBufferSource();
		virtual ~WaveBufferSource();

		// SoundDataSource
		// decodes everything into m_data, then closes and deletes the decoder
		virtual bool open(SoundStream* stream);
		virtual bool getBytes(int bytes, void* dest, bool loop, int* position);
		// empty (body folded, 0x4D0470)
		virtual void reset();
		// delete[] m_data
		virtual bool close();
		// m_dataSize (body folded with MemoryImage::getWidth, 0x4BE750)
		virtual int getLength();
		// &m_format (body folded with Settings::getILink, 0x479850)
		virtual const WAVEFORMATEX* getFormat();
		virtual float getDuration();
		// false (body folded, 0x4529D0)
		virtual bool isStreaming();

		WAVEFORMATEX m_format;					// +0x14 PCM, from the decoder
		int m_dataSize;							// +0x28
		char* m_data;							// +0x2C new[] buffer of decoded PCM
	};
}
