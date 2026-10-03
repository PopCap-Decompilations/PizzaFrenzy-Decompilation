#include "WinInetUrl.h"

#include "Application.h"
#include "InternetInputStream.h"

namespace engine
{
	// 0x4BFD80
	InputStream* WinInetUrl::getInputStream()
	{
		connect();
		if (m_request != 0 && m_stream == 0)
		{
			m_stream = new InternetInputStream(m_request);
		}
		return m_stream;
	}

	// 0x4BFE20
	WinInetUrl::~WinInetUrl()
	{
		if (m_request != 0)
		{
			InternetCloseHandle(m_request);
			m_request = 0;
		}
	}

	// 0x4BFEE0
	bool WinInetUrl::connect()
	{
		if (m_request != 0)
		{
			return false;
		}
		if (m_session != 0)
		{
			m_request = InternetOpenUrlA(m_session, m_url.c_str(), 0, 0,
				m_useCaches ? 0 : INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_PRAGMA_NOCACHE, 0);
			if (m_request != 0)
			{
				return true;
			}
		}
		Application* application = getApplication();
		application->logSystem("Failed to open URL [%s] : %s\n", m_url.c_str(),
			application->getErrorMessage(GetLastError()).c_str());
		return false;
	}

	// 0x4C0000
	WinInetUrl::WinInetUrl(HINTERNET session, const std::string& url)
		: m_session(session), m_url(url), m_request(0)
	{
	}
}
