// The engine's string helpers (StringUtil.cpp): case-insensitive compare, printf-style and locale number formatting.
#pragma once

#include <string>

namespace engine
{
	// CompareStringA(LOCALE_USER_DEFAULT, NORM_IGNORECASE) - 2: <0, 0 or >0 (0 also when the call fails)
	int compareNoCase(const std::string& a, const std::string& b);
	// atol
	int toInt(const std::string& s);
	// _vsnprintf into the application's text buffer, assigned to out; returns out
	std::string& format(std::string& out, const char* fmt, ...);
	// GetNumberFormatA with the user locale's separators and the given number of decimals; returns out
	std::string& formatNumber(std::string& out, const std::string& number, int decimals);
	// CharUpperA in place; returns s
	std::string& toUpper(std::string& s);
	// format(out, "%d", value) then formatNumber(out, out, 0)
	std::string& formatNumber(std::string& out, int value);
}
