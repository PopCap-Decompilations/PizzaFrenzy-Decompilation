// CityHandler: the XML handler of a city file (res\cities\*.xml).
#pragma once

#include <string>

#include "engine/RefPtr.h"
#include "engine/XmlElementHandler.h"

namespace engine
{
	class Properties;
	class XmlHandlerStack;
}

class CityMap;

// Handles <city> and its <road>, <customer>, <kitchen>, <bridge> and <decoration> elements: sets up the CityMap
// (created by the game's level loader with the game's map) and creates its tiles and decorations. The members end
// at +0x38; the vtordisp (+0x38) and the Interface subobject (+0x3C) follow them (0x40 bytes).
class CityHandler : public engine::XmlElementHandler
{
public:
	CityHandler(CityMap* city, const std::string& elementName, engine::XmlHandlerStack* parser);
	virtual ~CityHandler();

	virtual void startElement(const std::string& name, const engine::Properties& attrs);	// slot 0 (engine::XmlHandler)

	engine::RefPtr<CityMap> m_city;						// +0x34 the map being loaded
};
