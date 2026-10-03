#include "StringUtil.h"

#include <stdarg.h>
#include <stdlib.h>
#include <string>

#include <windows.h>
#define STRSAFE_NO_DEPRECATE
#include <strsafe.h>

#include "Application.h"

namespace engine
{
	// formatNumber's locale format: the separators and flags are read once (lpDecimalSep doubles as the initialised
	// flag), NumDigits is set per call
	static NUMBERFMTA s_numberFormat;			// 0x532F70

	// 0x46DA80
	int compareNoCase(const std::string& a, const std::string& b)
	{
		int result = CompareStringA(LOCALE_USER_DEFAULT, NORM_IGNORECASE, a.c_str(), -1, b.c_str(), -1);
		if (result == 0)
			return 0;
		return result - 2;
	}

	// 0x46DAD0
	int toInt(const std::string& s)
	{
		return atoi(s.c_str());
	}

	// 0x46DB00
	std::string& format(std::string& out, const char* fmt, ...)
	{
		char* buffer = getTextBuffer();
		int size = getTextBufferSize();
		va_list args;
		va_start(args, fmt);
		StringCchVPrintfA(buffer, size, fmt, args);
		va_end(args);
		buffer[size - 1] = 0;
		return out = buffer;
	}

	// 0x46DB50
	std::string& formatNumber(std::string& out, const std::string& number, int decimals)
	{
		char* buffer = getTextBuffer();
		int size = getTextBufferSize();
		if (!s_numberFormat.lpDecimalSep)
		{
			int length = GetLocaleInfoA(LOCALE_USER_DEFAULT, LOCALE_SDECIMAL, buffer, size);
			s_numberFormat.lpDecimalSep = new char[length];
			StringCchCopyA(s_numberFormat.lpDecimalSep, length, buffer);
			length = GetLocaleInfoA(LOCALE_USER_DEFAULT, LOCALE_STHOUSAND, buffer, size);
			s_numberFormat.lpThousandSep = new char[length];
			StringCchCopyA(s_numberFormat.lpThousandSep, length, buffer);
			GetLocaleInfoA(LOCALE_USER_DEFAULT, LOCALE_ILZERO, buffer, size);
			s_numberFormat.LeadingZero = atoi(buffer);
			GetLocaleInfoA(LOCALE_USER_DEFAULT, LOCALE_SGROUPING, buffer, size);
			s_numberFormat.Grouping = atoi(buffer);
			GetLocaleInfoA(LOCALE_USER_DEFAULT, LOCALE_INEGNUMBER, buffer, size);
			s_numberFormat.NegativeOrder = atoi(buffer);
		}
		s_numberFormat.NumDigits = decimals;
		GetNumberFormatA(LOCALE_USER_DEFAULT, 0, number.c_str(), &s_numberFormat, buffer, size);
		return out = buffer;
	}

	// 0x46DC60
	std::string& toUpper(std::string& s)
	{
		char* buffer = getTextBuffer();
		int size = getTextBufferSize();
		StringCchCopyA(buffer, size, s.c_str());
		CharUpperA(buffer);
		return s = buffer;
	}

	// 0x46DCB0
	std::string& formatNumber(std::string& out, int value)
	{
		format(out, "%d", value);
		return formatNumber(out, out, 0);
	}
}
