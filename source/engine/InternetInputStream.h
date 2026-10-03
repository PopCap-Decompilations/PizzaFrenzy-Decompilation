// engine::InternetInputStream: an engine::InputStream over a WinInet request handle (WinInetUrl::getInputStream).
#pragma once

#include <windows.h>
#include <wininet.h>

#include "InputStream.h"

namespace engine
{
	// Layout: InputStream +0x00, m_handle +0x0C, then the vtordisp and Interface (0x18 bytes).
	class InternetInputStream : public InputStream
	{
	public:
		explicit InternetInputStream(HINTERNET handle);
		virtual ~InternetInputStream();

		// InputStream
		virtual int read(void* buffer, int size);			// InternetReadFile; failure throws IOException(GetLastError())
		virtual bool close();								// 0x451610 (folded): true, the handle stays open

		HINTERNET m_handle;						// +0x0C the connection's InternetOpenUrl handle (owned and closed by the WinInetUrl)
	};
}
