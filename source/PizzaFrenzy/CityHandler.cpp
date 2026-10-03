#include "CityHandler.h"

#include "engine/Point.h"
#include "engine/Properties.h"

#include "BridgeTile.h"
#include "CityMap.h"
#include "CustomerTile.h"
#include "Decoration.h"
#include "KitchenTile.h"
#include "RoadTile.h"

// 0x410080
CityHandler::CityHandler(CityMap* city, const std::string& elementName, engine::XmlHandlerStack* parser)
	: engine::XmlElementHandler(elementName, parser)
{
	m_city = city;
}

// 0x410180
CityHandler::~CityHandler()
{
	m_city = NULL;
}

// 0x410250
void CityHandler::startElement(const std::string& name, const engine::Properties& attrs)
{
	if (name == "city")
	{
		engine::Point gridSize = attrs.getIntPoint("gridSize", engine::Point(0, 0));
		int cellSize = attrs.getInt("cellSize", 0);
		engine::Point gridOffset = attrs.getIntPoint("gridOffset", g_zeroPoint);
		std::string background = attrs.getString("background", "");
		int uniqueId = attrs.getInt("uniqueID", 0);
		std::string hsvShift = attrs.getString("hsvShift", "0,0,0");
		m_city->create(gridSize.x, gridSize.y, cellSize, gridOffset, uniqueId);
		m_city->setBackground(background, hsvShift);
	}
	else if (name == "road")
	{
		std::string tileName = attrs.getString("tile", "NONAME");
		int weight = attrs.getInt("weight", 1);	// read but never used
		engine::RefPtr<Tile> tile = new RoadTile(tileName);
		tile->load(attrs);
		engine::Point cell = attrs.getIntPoint("cell", engine::Point(-1, -1));
		std::string id = attrs.getString("name", "");
		if (tile->init(m_city, id))
			tile->addToMap(m_city, cell);
	}
	else if (name == "customer")
	{
		std::string tileName = attrs.getString("tile", "NONAME");
		std::string id = attrs.getString("name", "");
		engine::RefPtr<Tile> tile = new CustomerTile(tileName);
		tile->load(attrs);
		engine::Point cell = attrs.getIntPoint("cell", engine::Point(-1, -1));
		if (tile->init(m_city, id))
			tile->addToMap(m_city, cell);
	}
	else if (name == "kitchen")
	{
		std::string tileName = attrs.getString("tile", "NONAME");
		engine::RefPtr<Tile> tile = new KitchenTile(tileName);
		tile->load(attrs);
		engine::Point cell = attrs.getIntPoint("cell", engine::Point(-1, -1));
		std::string id = attrs.getString("name", "");
		if (tile->init(m_city, id))
			tile->addToMap(m_city, cell);
	}
	else if (name == "bridge")
	{
		std::string tileName = attrs.getString("tile", "NONAME");
		engine::RefPtr<Tile> tile = new BridgeTile(tileName);
		tile->load(attrs);
		engine::Point cell = attrs.getIntPoint("cell", engine::Point(-1, -1));
		std::string id = attrs.getString("name", "");
		if (tile->init(m_city, id))
			tile->addToMap(m_city, cell);
	}
	else if (name == "decoration")
	{
		std::string tileName = attrs.getString("tile", "NONAME");
		engine::RefPtr<Decoration> decoration = new Decoration(tileName, 3);
		decoration->readAttributes(attrs);
		engine::Vector2 pos = attrs.getIntPoint("pos", engine::Point(-1, -1));
		if (decoration->load())
		{
			decoration->setPosition(pos);
			decoration->addToCity(m_city);
		}
	}
}
