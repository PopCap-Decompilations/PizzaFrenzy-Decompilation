// engine::XmlHandlerStack (the handler stack the XML loaders parse with), engine::parseXml (the SAX parser) and
// engine::XmlParseScope.
#pragma once

#include <deque>
#include <string>

#include "Object.h"
#include "RefPtr.h"
#include "XmlHandlerBase.h"

namespace engine
{
	class Properties;
	class Reader;
	class XmlElementHandler;
	class XmlHandler;

	// Parses the characters read from input (slot 3, -1 at the end) and calls the handler's startDocument,
	// startElement, characters, endElement and endDocument; entities, comments, CDATA, <?..?> and DOCTYPE are
	// handled. Errors are thrown as new engine::Exception(message).
	void parseXml(XmlHandler* handler, Reader* input);

	// Object +0x00 with nothing added, then the vtordisp (+0x0C) and the Interface subobject (+0x10): 0x14 bytes.
	// Built on the stack around every parseXml call (Application::loadXml, Font, HighScoreTable::run,
	// PizzaFrenzy::loadPizzaManifest); what it did is not visible in this build. Implicit constructor (always
	// inlined) and destructor (the vtable, holding only the destructor, was merged into 0x4F9470).
	class XmlParseScope : public Object
	{
	};

	// XmlHandlerBase +0x00, m_handlers +0x14, then the vtordisp (+0x28) and the Interface subobject (+0x2C): 0x30
	// bytes. Forwards the parser's events to the handler on top of the stack and pops it (after its
	// endDocument) when the element it handles ends; the root handler is pushed by pushHandler().
	class XmlHandlerStack : public XmlHandlerBase
	{
	public:
		XmlHandlerStack();
		virtual ~XmlHandlerStack();

		virtual void startElement(const std::string& name, const Properties& attributes);	// slot 0 (engine::XmlHandler)
		virtual void endElement(const std::string& name);									// slot 1 (engine::XmlHandler)
		virtual void startDocument();														// slot 2 (engine::XmlHandler)
		virtual void characters(const std::string& text);									// slot 4 (engine::XmlHandler)

		void pushHandler(XmlElementHandler* handler);

		std::deque<RefPtr<XmlElementHandler> > m_handlers;	// +0x14 handler stack; back() receives the events
	};
}
