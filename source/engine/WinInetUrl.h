// engine::WinInetUrl: engine::URLConnection over WinInet (InternetOpenUrlA on the application's "Sprout Game"
// session); made by Win32Application::openUrl.
#pragma once

#include <string>

#include <windows.h>
#include <wininet.h>

#include "RefPtr.h"
#include "URLConnection.h"

namespace engine
{
	class InputStream;

	class WinInetUrl : public URLConnection
	{
	public:
		WinInetUrl(HINTERNET session, const std::string& url);
		virtual ~WinInetUrl();

		// URLConnection
		virtual bool connect();
		virtual InputStream* getInputStream();

		HINTERNET m_session;					// +0x10 the application's InternetOpenA session (not owned)
		std::string m_url;						// +0x14
		HINTERNET m_request;					// +0x30 InternetOpenUrlA handle (0 until connected); closed by the destructor
		RefPtr<InputStream> m_stream;			// +0x34 InternetInputStream over m_request, made by the first getInputStream
	};
}
