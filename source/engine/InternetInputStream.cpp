#include "InternetInputStream.h"

#include "Exception.h"

namespace engine
{
	// 0x4D0F20
	InternetInputStream::~InternetInputStream()
	{
	}

	// 0x4D0F50
	InternetInputStream::InternetInputStream(HINTERNET handle)
		: m_handle(handle)
	{
	}

	// 0x4D1050
	int InternetInputStream::read(void* buffer, int size)
	{
		DWORD bytesRead;
		if (!InternetReadFile(m_handle, buffer, size, &bytesRead))
			throw IOException(GetLastError());
		return bytesRead;
	}

	// 0x451610 (folded)
	bool InternetInputStream::close()
	{
		return true;
	}
}
