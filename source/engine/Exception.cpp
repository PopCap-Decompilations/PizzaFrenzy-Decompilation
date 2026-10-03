#include "Exception.h"

namespace engine
{
	// 0x4040F0 (folded: identical to std::logic_error::what, which the linker folded it with)
	const char* Exception::what() const
	{
		return m_message.c_str();
	}

	// 0x4D0FE0
	IOException::IOException(int code)
		: m_code(code)
	{
	}
}
