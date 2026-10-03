// CustomerTile: a customer's house on the map (tile class "customer").
#pragma once

#include <string>

#include "BuildingTile.h"

namespace engine
{
	class Properties;
	class XmlWriter;
}

class CityMap;
class Character;
class Order;
class Topping;

// Places orders (order popups), receives deliveries; its character (m_character) is looked up with the city's
// theme.
class CustomerTile : public BuildingTile
{
public:
	CustomerTile(const std::string& typeName);
	virtual ~CustomerTile();

	virtual void placeOrder(Order* order, bool flag);						// slot 107
	virtual void showSpecialPopup(int type);								// slot 108
	virtual void onVehicleDispatched();									// slot 109
	virtual Order* getActiveOrder();										// slot 110
	virtual Order* getOrder();												// slot 111
	virtual void deliverOrder();											// slot 112
	virtual void setOrderWaiting(bool waiting);							// slot 113
	virtual void refreshOrder();											// slot 114

	// overrides
	virtual bool init(CityMap* map, const std::string& id);				// Tile slot 76

	// 0x426FF0 (folded): Tile slot 78; the identical overrides of CustomerTile, BridgeTile and RoadTile share one body
	virtual bool load(const engine::Properties& attrs)
	{
		return Tile::load(attrs);
	}

	virtual void save(engine::XmlWriter& writer);							// Tile slot 83
	virtual bool matchesTopping(Topping* topping);							// BuildingTile slot 94

	Character* getCharacter();

	std::string m_character;							// +0x1B8 CustomerTileType character name
};
