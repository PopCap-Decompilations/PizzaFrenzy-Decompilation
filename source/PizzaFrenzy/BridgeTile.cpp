// BridgeTile: the drawbridge map tile that boats raise.
#include "BridgeTile.h"

#include "engine/Image.h"
#include "engine/Selector.h"
#include "engine/XmlWriter.h"
#include "CityMap.h"
#include "PizzaFrenzy.h"
#include "TileManager.h"
#include "Vehicle.h"

// 0x4263E0
bool BridgeTile::isBlocked(const engine::Point& cell) const
{
	return m_blocked;
}

// 0x4263F0
void BridgeTile::update(const engine::UpdateContext& time)
{
	if (m_blockTimer > 0.0f)
	{
		m_blockTimer -= time.elapsed;
		if (m_blockTimer <= 0.0f)
			setBlocked(false);
	}
}

// 0x426440
bool BridgeTile::requestBlocked(bool blocked)
{
	if (blocked)
	{
		if (isOccupied())
			return false;
		setBlocked(true);
	}
	else
	{
		setBlocked(false);
	}
	return true;
}

// 0x426480
BridgeTile::~BridgeTile()
{
}

// 0x426580
void BridgeTile::addToMap(CityMap* map, const engine::Point& cell)
{
	Tile::addToMap(map, cell);
	m_roadImages->setPosition(getPosition());
}

// 0x4265C0
engine::Component* BridgeTile::getLayerComponent(int layer)
{
	if (layer == 3)
		return this;
	if (layer == 2)
		return m_roadImages;
	return NULL;
}

// 0x4265E0
void BridgeTile::setBlocked(bool blocked)
{
	if (blocked)
	{
		m_roadImages->select(0, 0.0f, true);
		m_groundImages->select(0, 0.0f, true);
		m_blocked = true;
		m_blockTimer = 2.0f;
	}
	else
	{
		m_roadImages->select(1, 0.0f, true);
		m_groundImages->select(1, 0.0f, true);
		m_blocked = false;
		m_blockTimer = 0.0f;
	}
}

// 0x426660
bool BridgeTile::isOccupied()
{
	std::vector<engine::RefPtr<Vehicle> >& vehicles = m_map->m_vehicles;
	for (std::vector<engine::RefPtr<Vehicle> >::iterator it = vehicles.begin(); it != vehicles.end(); ++it)
	{
		engine::Point cell = (*it)->m_tile;
		if (m_map->getTile(cell) == this)
			return true;
	}
	return false;
}

// 0x4266E0
BridgeTile::BridgeTile(const std::string& name)
	: Tile(name)
{
	setClassName("bridge");
}

// 0x4267A0
bool BridgeTile::init(CityMap* map, const std::string& id)
{
	m_roadImages = new engine::Selector();
	m_roadImages->setName("RoadImages");
	m_groundImages = new engine::Selector();
	m_groundImages->setName("GroundImages");
	addChild(m_groundImages);
	BridgeTileType* type = static_cast<BridgeTileType*>(PizzaFrenzy::getTileManifest()->getTile(getName(), m_className));
	if (type)
	{
		m_roadImages->addChild(new engine::Image(type->m_roadImage));
		m_groundImages->addChild(new engine::Image(type->m_image));
		m_roadImages->addChild(new engine::Image(type->m_unblockedRoadImage));
		m_groundImages->addChild(new engine::Image(type->m_unblockedImage));
		m_roadImages->select(1, 0.0f, true);
		m_groundImages->select(1, 0.0f, true);
	}
	Tile::setType(type, id);
	m_blocked = false;
	m_blockTimer = 0.0f;
	return true;
}

// 0x426AE0
void BridgeTile::save(engine::XmlWriter& writer)
{
	writer.startElement("bridge");
	Tile::save(writer);
	writer.endElement();
}
