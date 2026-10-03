#include "XmlElementHandler.h"

#include <string>

namespace engine
{
	// 0x478D20: XmlHandlerStack pops the handler when this element ends
	const std::string& XmlElementHandler::getElementName() const
	{
		return m_elementName;
	}

	// 0x478D30
	XmlElementHandler::~XmlElementHandler()
	{
		m_handlerStack = 0;
	}

	// 0x478DE0
	XmlElementHandler::XmlElementHandler(const std::string& elementName, XmlHandlerStack* handlerStack)
		: m_elementName(elementName)
	{
		m_handlerStack = handlerStack;
	}
}
