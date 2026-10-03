// RoadTile: a road map tile that picks its road piece from its four neighbours.
#include "RoadTile.h"

#include "engine/Image.h"
#include "engine/XmlWriter.h"
#include "CityMap.h"
#include "PizzaFrenzy.h"
#include "TileManager.h"

// 0x426B90
RoadTile::~RoadTile()
{
	m_imageItem->setFlags(0x10);
}

// 0x426C10
engine::Component* RoadTile::getLayerComponent(int layer)
{
	if (layer == 1)
		return this;
	return NULL;
}

// 0x426C30
void RoadTile::addToMap(CityMap* map, const engine::Point& cell)
{
	Tile::addToMap(map, cell);
}

// 0x426C90
bool RoadTile::init(CityMap* map, const std::string& id)
{
	TileType* type = PizzaFrenzy::getTileManifest()->getRoadTile(getName(), 0);
	if (type->m_image)
	{
		m_imageItem = new engine::Image(type->m_image);
		addChild(m_imageItem);
	}
	Tile::setType(type, id);
	return true;
}

// 0x426D30
void RoadTile::updateConnections()
{
	engine::Point cell = getCell();
	bool north = m_map->isRoadAt(engine::Point(cell.x, cell.y - 1));
	bool south = m_map->isRoadAt(engine::Point(cell.x, cell.y + 1));
	bool east = m_map->isRoadAt(engine::Point(cell.x + 1, cell.y));
	bool west = m_map->isRoadAt(engine::Point(cell.x - 1, cell.y));
	int connections = (north ? 4 : 0) | (south ? 1 : 0) | (east ? 8 : 0) | (west ? 2 : 0);
	TileType* type = PizzaFrenzy::getTileManifest()->getRoadTile(getName(), connections);
	m_image = type->m_image;
	m_imageItem->setImage(m_image);
	if (m_name == "Water" && !getGame()->m_inEditor)
	{
		m_imageItem->setVisible(false);
		m_image = NULL;
	}
	else if (m_name == "Water" && getGame()->m_inEditor)
	{
		m_imageItem->setVisible(true);
		m_imageItem->setAlpha(0.5f);
	}
	else
	{
		m_imageItem->setVisible(true);
		m_imageItem->setAlpha(1.0f);
	}
}

// 0x426F30
RoadTile::RoadTile(const std::string& name)
	: Tile(name)
{
	setClassName("roads");
	m_imageItem = NULL;
	m_weight = 1;
}

// 0x427000
void RoadTile::save(engine::XmlWriter& writer)
{
	writer.startElement("road");
	Tile::save(writer);
	writer.writeAttribute("weight", m_weight);
	writer.endElement();
}
