// Tile: the base of every map tile; DepthSortedContainer: the map layer that keeps its children in drawing order.
#include <algorithm>
#include <string>
#include <vector>

#include "engine/Rect.h"
#include "engine/Surface.h"
#include "engine/XmlWriter.h"

#include "CityMap.h"
#include "Tile.h"
#include "TileManager.h"

// 0x41ADA0
void DepthSortedContainer::addChild(engine::Component* child)
{
	if (std::find(m_children.begin(), m_children.end(), child) == m_children.end())
	{
		child->updateBounds();
		DepthOrder order = { child->getBounds().right, child->getBounds().bottom };
		m_children.insert(std::find_if(m_children.begin(), m_children.end(), order), child);
		addTreeFlags(child->getTreeFlags());
		addTreeFlags(8);
		child->onAddedTo(this);
	}
}

// 0x41AE70
void DepthSortedContainer::updateChildOrder(engine::Component* child)
{
	std::vector<engine::Component*>::iterator it = std::find(m_children.begin(), m_children.end(), child);
	DepthOrder order = { child->getBounds().right, child->getBounds().bottom };
	std::vector<engine::Component*>::iterator next = std::find_if(m_children.begin(), m_children.end(), order);
	if (it != m_children.end() && it + 1 != next)
	{
		m_children.erase(it);
		DepthOrder newOrder = { child->getBounds().right, child->getBounds().bottom };
		m_children.insert(std::find_if(m_children.begin(), m_children.end(), newOrder), child);
	}
}

// 0x41AF80
const std::string& Tile::getClassName() const
{
	return m_className;
}

// 0x41AF90
const std::string& Tile::getId() const
{
	return m_id;
}

// 0x41AFB0
void Tile::removeFromMap()
{
	setFlags(0x10);
	m_map->removeTile(this);
}

// 0x41AFD0
engine::Point Tile::getCell() const
{
	return m_cell;
}

// 0x41AFF0
engine::Point Tile::getFootprintOrigin() const
{
	engine::Vector2 position = getPosition();
	position.x += m_width * 0.5f + m_offset.x - m_base.x * 0.5f;
	return position;
}

// 0x41B050
engine::Component* Tile::getLayerComponent(int layer)
{
	if (layer == 3)
		return this;
	if (layer == 5)
		return m_overlay;
	return NULL;
}

// 0x41B070
void Tile::setMapPosition(CityMap* map, const engine::Point& cell)
{
	m_map = map;
	m_cell = cell;
	engine::Point screen = map->cellToScreen(cell);
	updateBounds();
	setPosition(screen.x - m_offset.x - m_width * 0.5f, screen.y - m_offset.y - m_height * 0.5f);
	if (m_overlay)
		m_overlay->setPosition(getPosition());
}

// 0x41B130
Tile::~Tile()
{
}

// 0x41B230
int Tile::getWeight(const engine::Point& cell) const
{
	return -1;
}

// 0x41B260
bool Tile::setType(TileType* type, const std::string& id)
{
	m_id = id;
	if (type == NULL)
		return false;
	m_width = type->m_image->getWidth();
	m_height = type->m_image->getHeight();
	m_dimensions = type->m_dimensions;
	m_offset = type->m_offset;
	m_base = type->m_base;
	m_image = type->m_image;
	return true;
}

// 0x41B320
void Tile::setClassName(std::string className)
{
	m_className = className;
}

// 0x41B380
void Tile::save(engine::XmlWriter& writer)
{
	writer.writeAttribute("name", m_id);
	writer.writeAttribute("tile", getName());
	writer.writeAttribute("cell", getCell());
}

// 0x41B4C0
void Tile::addToMap(CityMap* map, const engine::Point& cell)
{
	if (m_id.empty())
		m_id = map->makeTileName();
	setMapPosition(map, cell);
	m_map->placeTile(cell, this);
}

// 0x41B570
Tile::Tile(const std::string& typeName)
	: m_map(NULL)
	, m_cell(-1, -1)
	, m_offset(0, 0)
	, m_dimensions(1, 1)
{
	setName(typeName);
	m_overlay = NULL;
}
