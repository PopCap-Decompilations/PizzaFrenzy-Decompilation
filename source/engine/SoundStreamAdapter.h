// engine::SoundStreamAdapter: an engine input stream exposed to the sound system as an engine::SoundStream.
#pragma once

#include "Object.h"
#include "RefPtr.h"
#include "SoundStream.h"

namespace engine
{
	class SeekableInputStream;

	// Created by the application's loadSound/loadSoundStream (slots 22/23) around the stream of the sound file.
	// Implicit destructor (0x484940 releases m_stream). Layout: Object +0x00, SoundStream +0x0C, m_stream +0x10,
	// then the vtordisp and Interface (0x1C bytes).
	class SoundStreamAdapter : public Object, public SoundStream
	{
	public:
		SoundStreamAdapter(SeekableInputStream* stream);

		// SoundStream (addRef and release are also the final overriders of Interface's)
		virtual unsigned int read(void* buffer, unsigned int bytes);
		virtual int seek(long offset, int origin);
		virtual long tell();
		virtual int close();
		virtual void addRef();
		virtual void release();

		RefPtr<SeekableInputStream> m_stream;	// +0x10 the wrapped input stream
	};
}
