// engine::Exception and engine::IOException, the only classes the original's RTTI names (they are thrown).
// Exception: Object +0x00, std::exception +0x0C, m_message +0x18, then the vtordisp and Interface (0x3C bytes).
#pragma once

#include "Object.h"

#include <exception>
#include <string>

namespace engine
{
	class Exception : public Object, public std::exception
	{
	public:
		// 0x4CE140 (copy emitted in another object: the constructors are inline). The destructors of Exception
		// (0x404120) and IOException (0x4D10A0) store no vtable, so neither class declares one.
		Exception()
		{
		}

		// 0x403FF0
		Exception(const std::string& message)
			: m_message(message)
		{
		}

		virtual const char* what() const;

		std::string m_message;					// +0x18
	};

	class IOException : public Exception
	{
	public:
		IOException(int code);

		int m_code;								// +0x34
	};
}
