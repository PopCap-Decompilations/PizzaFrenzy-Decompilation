// PizzaManifestHandler: the XML handler of res\manifests\pizzas.xml and userPizzas.xml.
#pragma once

#include <string>

#include "engine/RefPtr.h"
#include "engine/XmlElementHandler.h"

namespace engine
{
	class Properties;
	class XmlHandlerStack;
}

class PizzaDesign;

// Root handler ("pm"): <pm maxID>, <pz n ID> builds a PizzaDesign from its <t n p> children and hands it to the
// game on </pz>. Implicit destructor (0x417F60).
class PizzaManifestHandler : public engine::XmlElementHandler
{
public:
	PizzaManifestHandler(const std::string& name, engine::XmlHandlerStack* parser, bool userPizzas);

	// overrides
	virtual void startElement(const std::string& name, const engine::Properties& attributes);	// engine::XmlHandler slot 0
	virtual void endElement(const std::string& name);						// engine::XmlHandler slot 1

	engine::RefPtr<PizzaDesign> m_pizza;				// +0x34 pizza being read
	bool m_userPizzas;									// +0x38 reading userPizzas.xml
	int m_maxId;										// +0x3C "maxID": next free pizza ID (not initialised)
};
