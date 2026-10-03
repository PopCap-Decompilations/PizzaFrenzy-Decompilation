#include "InputStreamReader.h"

#include "InputStream.h"

namespace engine
{
	// 0x494C90
	InputStreamReader::InputStreamReader(InputStream* stream)
	{
		m_stream = stream;
	}

	// 0x494DF0
	bool InputStreamReader::close()
	{
		return m_stream->close();
	}

	// 0x494E00
	int InputStreamReader::read()
	{
		char c;
		if (m_stream->read(&c, 1) == 1)
			return c;
		return -1;
	}

	// 0x494E30
	int InputStreamReader::read(void* buffer, int size)
	{
		return m_stream->read(buffer, size);
	}
}
