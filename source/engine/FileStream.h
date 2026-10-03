// engine::FileStream: a sound data source decoded on demand (streamed music).
#pragma once

#include <windows.h>
#include <mmsystem.h>

#include "SoundDataSource.h"

namespace engine
{
	class SoundDecoder;

	// Reads through its decoder as the handle plays; only one handle may use it. Made by
	// SimpleSoundDX::loadStreamingSound. Layout: SoundDataSource +0x00, members +0x14 (0x2C bytes).
	class FileStream : public SoundDataSource
	{
	public:
		FileStream();
		// close() (inlined)
		virtual ~FileStream();

		// SoundDataSource
		// closes and deletes a previous decoder, creates one, fills m_format
		virtual bool open(SoundStream* stream);
		// on a short read rewinds (loop) or fills the rest with silence; position is not used
		virtual bool getBytes(int bytes, void* dest, bool loop, int* position);
		// m_decoder->rewind()
		virtual void reset();
		// closes and deletes the decoder (no null check), zeroes m_format
		virtual bool close();
		// m_decoder->getDecodedSize()
		virtual int getLength();
		// &m_format (body folded with Settings::getILink, 0x479850)
		virtual const WAVEFORMATEX* getFormat();
		virtual float getDuration();
		// true (body folded, 0x451610)
		virtual bool isStreaming();
		// -1 if a handle already uses the stream
		virtual int addHandle(SoundHandle* handle);

		WAVEFORMATEX m_format;					// +0x14
		SoundDecoder* m_decoder;				// +0x28 an OggDecoder
	};
}
