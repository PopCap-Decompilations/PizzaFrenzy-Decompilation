// engine::Reader and engine::InputStreamReader: Java-style character readers (the XML parser reads through them).
#pragma once

#include "Object.h"
#include "RefPtr.h"

namespace engine
{
	class InputStream;

	// Abstract character reader; its implicit constructor is only inlined by InputStreamReader's. Implicit
	// destructor. MSVC places overloaded virtuals in the vtable in reverse declaration order, so read() is declared
	// before read(buffer, size) to give slots 2 read(buffer, size) and 3 read(). Layout: Object +0x00, then the
	// vtordisp and Interface (0x14 bytes).
	class Reader : public Object
	{
	public:
		// slot 1: closes the underlying stream
		virtual bool close() = 0;
		// slot 3: one character (sign-extended char), -1 at the end
		virtual int read() = 0;
		// slot 2: block read; returns the byte count
		virtual int read(void* buffer, int size) = 0;
	};

	// Reader over an InputStream (FileSystem::getReader/openDiskReader, HighScoreTable::run). Implicit destructor
	// (0x494D90). Layout: Reader +0x00, m_stream +0x0C, then the vtordisp and Interface (0x18 bytes).
	class InputStreamReader : public Reader
	{
	public:
		InputStreamReader(InputStream* stream);

		// Reader
		virtual bool close();
		virtual int read();
		virtual int read(void* buffer, int size);

		RefPtr<InputStream> m_stream;			// +0x0C
	};
}
