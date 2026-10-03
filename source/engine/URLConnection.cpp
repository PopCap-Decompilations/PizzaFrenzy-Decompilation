#include "URLConnection.h"

namespace engine
{
	// 0x4D1130
	void URLConnection::setUseCaches(bool useCaches)
	{
		m_useCaches = useCaches;
	}

	// 0x4D1140
	bool URLConnection::getUseCaches() const
	{
		return m_useCaches;
	}

	// 0x4D1150
	URLConnection::URLConnection()
		: m_useCaches(true)
	{
	}
}
