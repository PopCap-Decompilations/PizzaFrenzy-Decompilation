// engine::SoundDataSource: the abstract PCM source that sound handles play from.
#pragma once

#include <vector>

#include <windows.h>
#include <mmsystem.h>

namespace engine
{
	class SoundHandle;
	class SoundStream;

	// Not an engine::Object (no vbptr, no reference count): it keeps the handles that use it and free() deletes it
	// once none does. Implemented by WaveBufferSource (decoded into memory) and FileStream (decoded on demand).
	// Layout: vfptr +0x00, m_handles +0x04 (0x14 bytes).
	class SoundDataSource
	{
	public:
		SoundDataSource();

		// slot 0: creates the decoder and fills the format
		virtual bool open(SoundStream* stream) = 0;
		// slot 1: copies PCM from *position; at the end wraps (loop) or fills with silence; true when the end was
		// reached
		virtual bool getBytes(int bytes, void* dest, bool loop, int* position) = 0;
		// slot 2: back to the start
		virtual void reset() = 0;
		// slot 3: releases the decoder or the data
		virtual bool close() = 0;
		// slot 4: PCM bytes
		virtual int getLength() = 0;
		// slot 5
		virtual const WAVEFORMATEX* getFormat() = 0;
		// slot 6: seconds (length / nAvgBytesPerSec)
		virtual float getDuration() = 0;
		// slot 7: FileStream true, WaveBufferSource false
		virtual bool isStreaming() = 0;
		// slot 8: frees m_handles
		virtual ~SoundDataSource();
		// slot 9: appends the handle; returns the handle count
		virtual int addHandle(SoundHandle* handle);
		// slot 10: erases every occurrence; returns the handle count
		virtual int removeHandle(SoundHandle* handle);
		// slot 11
		virtual int getHandleCount();
		// slot 12: deletes this if no handle uses it, else logs "SoundDataSource free FAILED - still has %d open
		// references" and returns false
		virtual bool free();

		std::vector<SoundHandle*> m_handles;	// +0x04 the handles playing this source
	};
}
