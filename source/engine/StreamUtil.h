// engine::readInt16 and engine::readInt32: little-endian integers read from an input stream.
#pragma once

namespace engine
{
	class InputStream;

	// reads 2 bytes and assembles them little-endian byte by byte (unsigned: ArchiveToc::read zero-extends the
	// name length it reads)
	void readInt16(InputStream* stream, unsigned short* value);
	// reads 4 bytes and assembles them little-endian byte by byte
	void readInt32(InputStream* stream, int* value);
}
