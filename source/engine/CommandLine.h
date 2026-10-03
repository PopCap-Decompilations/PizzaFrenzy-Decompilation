// engine::CommandLine: the command-line switch parser ('-' or '/' switches, "name:value" or name:"quoted value")
// the application queries for the screensaver switches s, p, c and l.
#pragma once

#include <map>
#include <string>

namespace engine
{
	// Not an engine::Object: a plain class with a virtual destructor, held by value in Win32Application (+0x160).
	// VS2003's string/map are bigger than today's: the offsets below are the original's.
	class CommandLine
	{
	public:
		explicit CommandLine(const char* commandLine);
		virtual ~CommandLine();									// slot 0

		std::map<std::string, std::string>::iterator find(const char* name);
		bool hasOption(const char* name);
		bool hasValue(const char* name);
		const char* getValue(const char* name);					// the switch's value, NULL when absent
		bool parse(const char* commandLine);					// always false

		std::string m_commandLine;								// +0x04 text of the last parse()
		std::map<std::string, std::string> m_options;			// +0x20 switch name -> value (empty for plain switches)
		bool m_caseSensitive;									// +0x2C always false: names are lower-cased (tolower) when parsed and looked up
																// +0x2D (padding)
	};
}
