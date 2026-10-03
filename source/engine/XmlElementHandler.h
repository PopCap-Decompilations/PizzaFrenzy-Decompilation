// engine::XmlElementHandler: the handler of one XML element, popped by its handler stack at the element's end tag.
#pragma once

#include <string>

#include "XmlHandlerBase.h"

namespace engine
{
	class XmlHandlerStack;

	// XmlHandlerBase +0x00 (its XmlHandler at +0x0C keeps XmlDefaultHandler's vtable 0x502E88: no overrides), own
	// members from +0x14, then the vtordisp (+0x34) and the Interface subobject (+0x38): 0x3C bytes. Base of the
	// game's element handlers, which XmlHandlerStack (itself an XmlHandlerBase) holds and pops when getElementName()
	// matches the end tag. Its destructor is 0x478D30 (0x46BD50 is XmlHandlerBase's).
	class XmlElementHandler : public XmlHandlerBase
	{
	public:
		XmlElementHandler(const std::string& elementName, XmlHandlerStack* handlerStack);
		virtual ~XmlElementHandler();

		const std::string& getElementName() const;

		XmlHandlerStack* m_handlerStack;		// +0x14 owning handler stack (not reference counted; cleared by the destructor)
		std::string m_elementName;				// +0x18 element this handler handles
	};
}
