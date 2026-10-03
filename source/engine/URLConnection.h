// engine::URLConnection: a Java-style URL connection (connect, getInputStream, useCaches); WinInetUrl implements it.
#pragma once

#include "Object.h"

namespace engine
{
	class InputStream;

	// Abstract; implicit destructor (slot 0 is the folded 0x44A0A0). Layout: Object +0x00, m_useCaches +0x0C, then
	// the vtordisp and Interface (0x18 bytes).
	class URLConnection : public Object
	{
	public:
		URLConnection();

		virtual void setUseCaches(bool useCaches);			// slot 1
		virtual bool getUseCaches() const;					// slot 2
		virtual bool connect() = 0;							// slot 3: opens the URL; true on success
		virtual InputStream* getInputStream() = 0;			// slot 4: connects and returns a stream over the response

		bool m_useCaches;						// +0x0C true after construction; false makes WinInetUrl's connect bypass the cache
												// +0x0D (padding)
	};
}
