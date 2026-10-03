#include "CommandLine.h"

#include <ctype.h>
#include <mbstring.h>
#include <string.h>

#include <algorithm>

namespace engine
{
	// 0x4CE620
	std::map<std::string, std::string>::iterator CommandLine::find(const char* name)
	{
		std::string key(name);
		if (!m_caseSensitive)
			std::transform(key.begin(), key.end(), key.begin(), tolower);
		return m_options.find(key);
	}

	// 0x4CE710
	bool CommandLine::hasOption(const char* name)
	{
		return find(name) != m_options.end();
	}

	// 0x4CE740
	bool CommandLine::hasValue(const char* name)
	{
		std::map<std::string, std::string>::iterator it = find(name);
		if (it == m_options.end())
			return false;
		return it->second.length() != 0;
	}

	// 0x4CE770
	const char* CommandLine::getValue(const char* name)
	{
		std::map<std::string, std::string>::iterator it = find(name);
		if (it == m_options.end())
			return NULL;
		return it->second.c_str();
	}

	// 0x4CE7B0
	bool CommandLine::parse(const char* commandLine)
	{
		if (commandLine != NULL)
		{
			m_commandLine = commandLine;
			m_options.clear();
			std::string emptyValue;
			const unsigned char* p = (const unsigned char*)commandLine;
			while (strlen((const char*)p) != 0)
			{
				// the next switch: '-' or '/', then its name
				p = _mbspbrk(p, (const unsigned char*)"-/");
				if (p == NULL)
					break;
				p = _mbsinc(p);
				if (strlen((const char*)p) == 0)
					break;

				const unsigned char* separator = _mbspbrk(p, (const unsigned char*)" :");
				if (separator == NULL)
				{
					// the last switch, without a value
					std::string name((const char*)p);
					if (!m_caseSensitive)
						std::transform(name.begin(), name.end(), name.begin(), tolower);
					m_options.insert(std::map<std::string, std::string>::value_type(name, emptyValue));
					break;
				}

				if (*separator != ' ' && strlen((const char*)separator) != 1)
				{
					// name:value or name:"quoted value"
					std::string name((const char*)p, separator - p);
					if (!m_caseSensitive)
						std::transform(name.begin(), name.end(), name.begin(), tolower);
					const unsigned char* value = _mbsinc(separator);
					const unsigned char* end;
					if (_mbspbrk(value, (const unsigned char*)"\"") == value)
					{
						value = _mbsinc(value);
						end = _mbspbrk(value, (const unsigned char*)"\"");
					}
					else
					{
						end = _mbschr(value, ' ');
					}
					if (end == NULL)
					{
						std::string valueString((const char*)value);
						if (name.length() != 0)
							m_options.insert(std::map<std::string, std::string>::value_type(name, valueString));
						break;
					}
					if (name.length() != 0)
					{
						std::string valueString((const char*)value, end - value);
						m_options.insert(std::map<std::string, std::string>::value_type(name, valueString));
					}
					p = _mbsinc(end);
				}
				else
				{
					// a switch without a value: followed by a space, or by a final ':'
					std::string name((const char*)p, separator - p);
					if (name.length() != 0)
					{
						if (!m_caseSensitive)
							std::transform(name.begin(), name.end(), name.begin(), tolower);
						m_options.insert(std::map<std::string, std::string>::value_type(name, emptyValue));
					}
					p = _mbsinc(separator);
				}
			}
		}
		return false;
	}

	// 0x4CEEE0
	CommandLine::~CommandLine()
	{
		m_options.clear();
	}

	// 0x4CEFA0
	CommandLine::CommandLine(const char* commandLine)
		: m_caseSensitive(false)
	{
		if (commandLine)
			parse(commandLine);
	}
}
