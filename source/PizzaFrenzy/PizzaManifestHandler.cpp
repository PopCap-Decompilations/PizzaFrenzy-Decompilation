#include "PizzaManifestHandler.h"

#include <string>

#include "engine/Point.h"
#include "engine/Properties.h"

#include "PizzaDesign.h"
#include "PizzaFrenzy.h"
#include "TileManager.h"

// 0x417E70: m_maxId is left for the <pm> element
PizzaManifestHandler::PizzaManifestHandler(const std::string& name, engine::XmlHandlerStack* parser, bool userPizzas)
	: engine::XmlElementHandler(name, parser)
	, m_userPizzas(userPizzas)
{
}

// 0x417FC0
void PizzaManifestHandler::endElement(const std::string& name)
{
	if (name == "pm")
	{
		getGame()->setNextPizzaId(m_maxId, m_userPizzas);
	}
	else if (name == "pz")
	{
		getGame()->addPizza(m_pizza, m_userPizzas);
		m_pizza = 0;
	}
}

// 0x418050: <pm maxID>, <pz n ID> (a pizza without an ID takes the next free one), <t n p>
void PizzaManifestHandler::startElement(const std::string& name, const engine::Properties& attributes)
{
	if (name == "pm")
		m_maxId = attributes.getInt("maxID", 1);
	if (name == "pz")
	{
		std::string pizzaName = attributes.getString("n", "Untitled");
		int id = attributes.getInt("ID", -1);
		m_pizza = new PizzaDesign(pizzaName);
		if (id < 0)
			id = m_maxId++;
		else if (id > m_maxId)
			m_maxId = id + 1;
		m_pizza->m_id = id;
	}
	if (name == "t")
	{
		std::string toppingName = attributes.getString("n", "Topping");
		engine::Point position = attributes.getIntPoint("p", engine::Point(0, 0));
		m_pizza->addTopping(PizzaFrenzy::getTileManifest()->getTopping(toppingName), position);
	}
}
