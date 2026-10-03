// CustomerTile: a customer's house on the map (tile class "customer").
#include <string>

#include "engine/Container.h"
#include "engine/Image.h"
#include "engine/XmlWriter.h"

#include "CustomerTile.h"
#include "GameLogic.h"
#include "Order.h"
#include "PizzaFrenzy.h"
#include "PizzaOrderPopup.h"
#include "PolicePopup.h"
#include "TileManager.h"

// 0x41B6A0
void CustomerTile::placeOrder(Order* order, bool flag)
{
	PizzaOrderPopup* popup = new PizzaOrderPopup;
	popup->init(this);
	popup->setOrder(order, flag);
	showPopup(popup);
}

// 0x41B730
void CustomerTile::showSpecialPopup(int type)
{
	switch (type)
	{
	case 2:
		{
			PolicePopup* popup = new PolicePopup;
			popup->init(this);
			showPopup(popup);
		}
		break;
	}
}

// 0x41B7B0
Character* CustomerTile::getCharacter()
{
	return PizzaFrenzy::getTileManifest()->getThemedCharacter(m_character);
}

// 0x41B7D0
bool CustomerTile::matchesTopping(Topping* topping)
{
	if (m_popup && m_popup->getPopupType() == 0)
		return topping == static_cast<PizzaOrderPopup*>(getPopup())->getOrder()->m_topping;
	return false;
}

// 0x41B820
void CustomerTile::onVehicleDispatched()
{
	if (m_popup)
		m_popup->onDispatched();
}

// 0x41B840
void CustomerTile::deliverOrder()
{
	if (m_popup && m_popup->getPopupType() == 0)
	{
		static_cast<PizzaOrderPopup*>(getPopup())->onDelivered();
		if (getCharacter()->m_special == 2 && PizzaFrenzy::getGameLogic()->hasCriminals())
			showSpecialPopup(2);
	}
}

// 0x41B8B0
void CustomerTile::setOrderWaiting(bool waiting)
{
	if (m_popup && m_popup->getPopupType() == 0)
		static_cast<PizzaOrderPopup*>(getPopup())->losePatience(waiting);
}

// 0x41B8F0
Order* CustomerTile::getActiveOrder()
{
	if (m_popup && isPopupActive() && m_popup->getPopupType() == 0)
		return static_cast<PizzaOrderPopup*>(getPopup())->getOrder();
	return NULL;
}

// 0x41B940
Order* CustomerTile::getOrder()
{
	if (m_popup && m_popup->getPopupType() == 0)
		return static_cast<PizzaOrderPopup*>(getPopup())->getOrder();
	return NULL;
}

// 0x41B980
void CustomerTile::refreshOrder()
{
	if (m_popup && m_popup->getPopupType() == 0)
		static_cast<PizzaOrderPopup*>(getPopup())->flashOrder();
}

// 0x41B9B0
CustomerTile::~CustomerTile()
{
}

// 0x41BA60
CustomerTile::CustomerTile(const std::string& typeName)
	: BuildingTile(typeName)
{
	setClassName("customer");
}

// 0x41BB30
void CustomerTile::save(engine::XmlWriter& writer)
{
	writer.startElement("customer");
	Tile::save(writer);
	writer.endElement();
}

// 0x41BBE0
bool CustomerTile::init(CityMap* map, const std::string& id)
{
	CustomerTileType* type = static_cast<CustomerTileType*>(PizzaFrenzy::getTileManifest()->getTile(getName(), m_className));
	if (type == NULL)
		return false;
	addChild(new engine::Image(type->m_image));
	Tile::setType(type, id);
	m_parkingSpot = type->m_parkingSpot;
	m_overlay = new engine::Container;
	m_character = type->m_character;
	return true;
}
