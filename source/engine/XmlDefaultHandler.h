// engine::XmlDefaultHandler: an XmlHandler with empty default callbacks, the base of every XML handler.
#pragma once

#include <string>

#include "Object.h"
#include "XmlHandler.h"

namespace engine
{
	// Object +0x00, XmlHandler +0x0C (vtable 0x502E88: the empty defaults), then the vtordisp (+0x14) and the
	// Interface subobject (+0x18): 0x1C bytes. The defaults are folded bodies shared with many other classes
	// (0x4D1520, 0x492310, 0x4D0470). Base of XmlHandlerBase (and so of XmlElementHandler, XmlHandlerStack and the
	// manifest handlers) and of FontXmlHandler.
	class XmlDefaultHandler : public Object, public XmlHandler
	{
	public:
		XmlDefaultHandler();
		virtual ~XmlDefaultHandler();

		virtual void startElement(const std::string& name, const Properties& attributes);	// slot 0 (engine::XmlHandler)
		virtual void endElement(const std::string& name);									// slot 1 (engine::XmlHandler)
		virtual void startDocument();														// slot 2 (engine::XmlHandler)
		virtual void endDocument();															// slot 3 (engine::XmlHandler)
		virtual void characters(const std::string& text);									// slot 4 (engine::XmlHandler)
	};
}
