#include "Xml.h"

#include <cctype>
#include <cstdlib>
#include <deque>
#include <string>

#include "Exception.h"
#include "InputStreamReader.h"
#include "Properties.h"
#include "XmlElementHandler.h"
#include "XmlHandler.h"

namespace engine
{
	// 0x46BE70
	XmlHandlerStack::~XmlHandlerStack()
	{
	}

	// 0x46BF40
	void XmlHandlerStack::startElement(const std::string& name, const Properties& attributes)
	{
		m_handlers.back()->startElement(name, attributes);
	}

	// 0x46BF80: the handler on top is popped at the end of the element it handles; the last one gets its endDocument
	// first
	void XmlHandlerStack::endElement(const std::string& name)
	{
		m_handlers.back()->endElement(name);
		if (name == m_handlers.back()->getElementName())
		{
			if (m_handlers.size() == 1)
				m_handlers.back()->endDocument();
			m_handlers.pop_back();
		}
	}

	// 0x46C060
	void XmlHandlerStack::startDocument()
	{
		m_handlers.back()->startDocument();
	}

	// 0x46C0A0
	void XmlHandlerStack::characters(const std::string& text)
	{
		m_handlers.back()->characters(text);
	}

	// 0x46C0E0
	XmlHandlerStack::XmlHandlerStack()
	{
	}

	// 0x46C3E0
	void XmlHandlerStack::pushHandler(XmlElementHandler* handler)
	{
		m_handlers.push_back(handler);
	}

	// 0x46C460
	static bool startsWith(const std::string& s, const std::string& prefix)
	{
		if (prefix.size() > s.size())
			return false;
		return s.compare(0, prefix.size(), prefix) == 0;
	}

	// 0x46C4B0
	static bool endsWith(const std::string& s, const std::string& suffix)
	{
		if (suffix.size() > s.size())
			return false;
		return s.compare(s.size() - suffix.size(), suffix.size(), suffix) == 0;
	}

	// 0x46C500: the state to return to; the prolog state (15) when the stack is empty
	static int popState(std::deque<int>& states)
	{
		if (!states.empty())
		{
			int state = states.back();
			states.pop_back();
			return state;
		}
		return 15;
	}

	// 0x46C5F0: the line and column (counted by parseXml) are not used
	static void throwXmlError(const std::string& message, int line, int column)
	{
		throw new Exception(message);
	}

	// 0x46CA00: a state machine over the characters; the states to return to are kept on a stack (popState).
	// States: 1 text, 2 entity (after '&'), 3 tag name (also "!--", "![CDATA[" and "!DOCTYPE"), 4 end tag (after
	// "</"), 5 after '<', 6 attribute name, 7 attribute value (inside the quotes), 8 inside a tag, 9 after an
	// attribute name, 10 after '=', 11 the root element has ended, 12 after '/' in a tag, 13 comment, 14 <?...?> or
	// DOCTYPE (skipped up to '>'), 15 prolog, 16 CDATA. Tabs and line breaks inside attribute values are dropped;
	// parsing ends at the first character after the root element.
	void parseXml(XmlHandler* handler, Reader* input)
	{
		std::deque<int> states;
		int state = 15;
		int depth = 0;
		int quote = '"';
		std::string text;
		std::string entity;
		std::string tagName;
		std::string attrName;
		std::string attrValue;
		Properties attributes;
		handler->startDocument();
		int line = 1;
		int column = 0;
		bool afterCR = false;
		int c;
		while ((c = input->read()) != -1)
		{
			if (c == '\n')
			{
				if (afterCR)
				{
					afterCR = false;
					continue;
				}
				++line;
				column = 0;
			}
			else if (afterCR)
			{
				afterCR = false;
			}
			else if (c == '\r')
			{
				afterCR = true;
				c = '\n';
				++line;
				column = 0;
			}
			else
			{
				++column;
			}

			if (state == 11)
			{
				handler->endDocument();
				return;
			}
			else if (state == 1)
			{
				if (c == '<')
				{
					states.push_back(state);
					state = 5;
					if (text.size() > 0)
					{
						handler->characters(text);
						text.clear();
					}
				}
				else if (c == '&')
				{
					states.push_back(state);
					state = 2;
					entity.clear();
				}
				else
				{
					text += (char)c;
				}
			}
			else if (state == 4)
			{
				if (c == '>')
				{
					state = popState(states);
					tagName = text;
					text.clear();
					if (--depth == 0)
						state = 11;
					handler->endElement(tagName);
				}
				else
				{
					text += (char)c;
				}
			}
			else if (state == 16)
			{
				if (c == '>' && endsWith(text, "]]"))
				{
					text.resize(text.size() - 2);
					handler->characters(text);
					text.erase();
					state = popState(states);
				}
				else
				{
					text += (char)c;
				}
			}
			else if (state == 13)
			{
				if (c == '>' && endsWith(text, "--"))
				{
					text.clear();
					state = popState(states);
				}
				else
				{
					text += (char)c;
				}
			}
			else if (state == 15)
			{
				if (c == '<')
				{
					state = 1;
					states.push_back(state);
					state = 5;
				}
			}
			else if (state == 14)
			{
				if (c == '>')
				{
					state = popState(states);
					if (state == 1)
						state = 15;
				}
			}
			else if (state == 5)
			{
				state = popState(states);
				if (c == '/')
				{
					states.push_back(state);
					state = 4;
				}
				else if (c == '?')
				{
					state = 14;
				}
				else
				{
					states.push_back(state);
					state = 3;
					tagName.clear();
					attributes.clear();
					text += (char)c;
				}
			}
			else if (state == 2)
			{
				if (c == ';')
				{
					state = popState(states);
					std::string name(entity);
					entity.clear();
					if (name == "lt")
						text += '<';
					else if (name == "gt")
						text += '>';
					else if (name == "amp")
						text += '&';
					else if (name == "quot")
						text += '"';
					else if (name == "apos")
						text += '\'';
					else
					{
						if (!startsWith(name, "#"))
							throwXmlError("Unknown entity: &" + name + ";", line, column);
						text += (char)atoi(name.substr(1).c_str());
					}
				}
				else
				{
					entity += (char)c;
				}
			}
			else if (state == 12)
			{
				if (tagName.empty())
					tagName = text;
				if (c != '>')
					throwXmlError("Expected >\tfor\ttag: <" + tagName + "/>", line, column);
				handler->startElement(tagName, attributes);
				handler->endElement(tagName);
				if (depth == 0)
				{
					handler->endDocument();
					return;
				}
				text.clear();
				attributes.clear();
				tagName.clear();
				state = popState(states);
			}
			else if (state == 3)
			{
				if (c == '>')
				{
					if (tagName.empty())
						tagName = text;
					text.clear();
					++depth;
					handler->startElement(tagName, attributes);
					tagName.clear();
					attributes.clear();
					state = popState(states);
				}
				else if (c == '/')
				{
					state = 12;
				}
				else if (c == '-' && text == "!-")
				{
					state = 13;
				}
				else if (c == '[' && text == "![CDATA")
				{
					state = 16;
					text.clear();
				}
				else if (c == 'E' && text == "!DOCTYP")
				{
					text.clear();
					state = 14;
				}
				else if (isspace((char)c))
				{
					tagName = text;
					text.clear();
					state = 8;
				}
				else
				{
					text += (char)c;
				}
			}
			else if (state == 7)
			{
				if (c == quote)
				{
					attrValue = text;
					text.clear();
					attributes.setString(attrName, attrValue);
					state = 8;
				}
				else if ((int)std::string("\t\r\n").find((char)c) >= 0)
				{
					// the original discards the sum (text + '\t'), so the character is dropped
					(void)(text + '\t');
				}
				else if (c == '&')
				{
					states.push_back(state);
					state = 2;
					entity.clear();
				}
				else
				{
					text += (char)c;
				}
			}
			else if (state == 10)
			{
				if (c == '"' || c == '\'')
				{
					state = 7;
					quote = c;
				}
				else if (!isspace((char)c))
				{
					throwXmlError("Error in attribute\tprocessing", line, column);
				}
			}
			else if (state == 6)
			{
				if (isspace((char)c))
				{
					attrName = text;
					text.clear();
					state = 9;
				}
				else if (c == '=')
				{
					attrName = text;
					text.clear();
					state = 10;
				}
				else
				{
					text += (char)c;
				}
			}
			else if (state == 9)
			{
				if (c == '=')
					state = 10;
				else if (!isspace((char)c))
					throwXmlError("Error in attribute\tprocessing.", line, column);
			}
			else if (state == 8)
			{
				if (c == '>')
				{
					state = popState(states);
					handler->startElement(tagName, attributes);
					++depth;
					tagName.clear();
					attributes.clear();
				}
				else if (c == '/')
				{
					state = 12;
				}
				else if (!isspace((char)c))
				{
					state = 6;
					text += (char)c;
				}
			}
		}
		if (state != 11)
			throwXmlError("missing end tag", line, column);
		handler->endDocument();
	}
}
