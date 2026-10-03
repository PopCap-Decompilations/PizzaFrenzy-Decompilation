// engine::User: a user profile (name and property bag), kept by engine::UserManager.
#pragma once

#include <string>

#include "Object.h"
#include "Properties.h"

namespace engine
{
	// Object +0x00, m_name +0x0C, m_attributes +0x28, then the vtordisp (+0x48) and the Interface subobject
	// (+0x4C): 0x50 bytes. The destructor (0x40D370) is implicit: it stores no vtables.
	class User : public Object
	{
	public:
		// 0x40D280 (copy emitted in another object: the constructor is inline)
		User()
		{
		}

		Properties& getAttributes();
		const std::string& getName() const;
		void setName(const std::string& name);

		std::string m_name;						// +0x0C user name (the registry key of the profile)
		Properties m_attributes;				// +0x28 the profile's properties (foodBank, maxCity, ...)
	};
}
