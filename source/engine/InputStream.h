// engine::InputStream and engine::SeekableInputStream: the engine's abstract byte input streams.
#pragma once

#include "Object.h"

namespace engine
{
	// Java-style byte input stream: base of the archive entry streams (ArchiveInputStream) and of the file and
	// internet streams. Implicit constructor and destructor (the constructor is inlined in each stream's). Its
	// vtable 0x5053EC is shared (/OPT:ICF) with ResampleFilter's. Layout: Object +0x00, then the vtordisp and
	// Interface (0x14 bytes).
	class InputStream : public Object
	{
	public:
		// slot 1: reads up to size bytes; returns the byte count
		virtual int read(void* buffer, int size) = 0;
		// slot 2
		virtual bool close() = 0;
	};

	// An input stream with a position: the archive file (Application slot 33 opens it), the disk files and the
	// archive entries. Implicit constructor and destructor. Layout as InputStream (0x14 bytes).
	class SeekableInputStream : public InputStream
	{
	public:
		// slot 3: origin 0 from the start, 1 from the current position, 2 from the end; false on failure
		virtual bool seek(int offset, int origin) = 0;
		// slot 4
		virtual int tell() = 0;
	};
}
