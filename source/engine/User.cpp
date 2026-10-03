#include "User.h"

namespace engine
{
	// 0x411480 (folded)
	Properties& User::getAttributes()
	{
		return m_attributes;
	}

	// 0x476BF0
	const std::string& User::getName() const
	{
		return m_name;
	}

	// 0x476C00
	void User::setName(const std::string& name)
	{
		m_name = name;
	}
}
