// engine::XmlHandler: the SAX-style callback interface driven by the XML parser.
#pragma once

#include <string>

#include "Interface.h"

namespace engine
{
	class Properties;

	// The parser (parse, 0x46CA00) and Application::loadXml call it for the document; XmlDefaultHandler gives every
	// slot an empty default. Constructor always inlined. Slots 2-4 are named from their use by XmlHandlerStack
	// (startDocument/endDocument take no arguments, characters one).
	class XmlHandler : public virtual Interface
	{
	public:
		virtual void startElement(const std::string& name, const Properties& attributes) = 0;	// slot 0
		virtual void endElement(const std::string& name) = 0;									// slot 1
		virtual void startDocument() = 0;														// slot 2
		virtual void endDocument() = 0;															// slot 3
		virtual void characters(const std::string& text) = 0;									// slot 4
	};
}
