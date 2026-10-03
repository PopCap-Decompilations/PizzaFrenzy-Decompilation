// engine::SoundStream: the byte stream the sound system decodes from (the Ogg Vorbis callbacks read through it).
#pragma once

namespace engine
{
	// Not an engine::Interface: it has its own reference counting slots. No constructor or destructor is declared
	// (its vtable is stored only by the constructor of SoundStreamAdapter, inlined).
	class SoundStream
	{
	public:
		// slot 0: returns the bytes read (the Ogg read callback passes nmemb as the byte count)
		virtual unsigned int read(void* buffer, unsigned int bytes) = 0;
		// slot 1: 0 on success, -1 on failure (the Ogg seek callback passes the low 32 bits of the offset)
		virtual int seek(long offset, int origin) = 0;
		// slot 2: current position
		virtual long tell() = 0;
		// slot 3: 0 on success, -1 on failure
		virtual int close() = 0;
		// slot 4: taken by the Ogg decoder after a successful open
		virtual void addRef() = 0;
		// slot 5
		virtual void release() = 0;
	};
}
