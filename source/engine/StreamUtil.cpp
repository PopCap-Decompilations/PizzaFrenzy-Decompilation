#include "StreamUtil.h"

#include "InputStream.h"

namespace engine
{
	// 0x499CF0
	void readInt16(InputStream* stream, unsigned short* value)
	{
		unsigned char buffer[2];
		stream->read(buffer, 2);
		*value = (buffer[1] << 8) | buffer[0];
	}

	// 0x499D20
	void readInt32(InputStream* stream, int* value)
	{
		unsigned char buffer[4];
		stream->read(buffer, 4);
		*value = (buffer[3] << 24) | (buffer[2] << 16) | (buffer[1] << 8) | buffer[0];
	}
}
