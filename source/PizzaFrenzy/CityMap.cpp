#include "CityMap.h"

#include <algorithm>

#include "engine/Range.h"
#include "engine/StringUtil.h"

#include "BoatSpawner.h"
#include "BridgeTile.h"
#include "CustomerTile.h"
#include "Decoration.h"
#include "KitchenTile.h"
#include "PizzaPopup.h"
#include "Tile.h"
#include "TileManager.h"
#include "Tip.h"
#include "Vehicle.h"

engine::Point g_zeroPoint(0, 0);		// 0x532930

// 0x411960 (folded)
int CityMap::getRows() const
{
	return m_rows;
}

// 0x411970 (folded)
int CityMap::getCols() const
{
	return m_cols;
}

// 0x411980
int CityMap::getCellSize() const
{
	return m_cellSize;
}

// 0x411990
Path* CityMap::findPath(const engine::Point& start, const engine::Point& goal, bool (*isPassable)(Tile* tile, const engine::Point& cell))
{
	return m_pathFinder.findPath(start, goal, isPassable);
}

// 0x4119A0
std::vector<engine::RefPtr<Topping> >& CityMap::getKitchenPizzas()
{
	return m_kitchenPizzas;
}

// 0x411A80: the cell's bottom-right pixel, (0,0) outside the grid
engine::Point CityMap::cellToScreen(const engine::Point& cell) const
{
	engine::Point pos(0, 0);
	if (cell.x >= 0 && cell.x < m_cols && cell.y >= 0 && cell.y < m_rows)
	{
		pos.x = (cell.x + 1) * m_cellSize - 1;
		pos.y = (cell.y + 1) * m_cellSize - 1;
		pos += m_gridOffset;
	}
	return pos;
}

// 0x411AF0: (-1,-1) outside the grid
engine::Point CityMap::screenToCell(engine::Point pos) const
{
	pos -= m_gridOffset;
	engine::Point cell(pos.x / m_cellSize, pos.y / m_cellSize);
	if (cell.x >= 0 && cell.x < m_cols && cell.y >= 0 && cell.y < m_rows)
		return cell;
	return engine::Point(-1, -1);
}

// 0x411C00
Tile* CityMap::getTile(const engine::Point& cell) const
{
	if (cell.x >= 0 && cell.x < m_cols && cell.y >= 0 && cell.y < m_rows)
		return m_cells.m_rows[cell.x][cell.y];
	return 0;
}

// 0x411C30
bool CityMap::isRoadAt(const engine::Point& cell) const
{
	Tile* tile = getTile(cell);
	return tile && tile->isDrivable(cell);
}

// 0x411C80
bool CityMap::isBlockedAt(const engine::Point& cell) const
{
	Tile* tile = getTile(cell);
	return tile && tile->isBlocked(cell);
}

// 0x411CD0: a tile showing a popup to the right, two cells to the left, two cells up or below
bool CityMap::isNextToBusyTile(Tile* tile) const
{
	engine::Point cell = tile->getCell();
	Tile* neighbour = getTile(engine::Point(cell.x + 1, cell.y));
	if (neighbour && !neighbour->isAvailable())
		return true;
	neighbour = getTile(engine::Point(cell.x - 2, cell.y));
	if (neighbour && !neighbour->isAvailable())
		return true;
	neighbour = getTile(engine::Point(cell.x, cell.y - 2));
	if (neighbour && !neighbour->isAvailable())
		return true;
	neighbour = getTile(engine::Point(cell.x, cell.y + 1));
	if (neighbour && !neighbour->isAvailable())
		return true;
	return false;
}

// 0x412020: the road tile nearest to the cell, if it is nearer than range
Tile* CityMap::findNearestRoad(const engine::Point& cell, const engine::Point& range) const
{
	Tile* nearest = 0;
	int nearestDistance = range.lengthSquared();
	for (std::vector<engine::RefPtr<Tile> >::const_iterator it = m_roads.begin(); it != m_roads.end(); ++it)
	{
		Tile* road = *it;
		int distance = (road->getCell() - cell).lengthSquared();
		if (distance < nearestDistance)
		{
			nearest = road;
			nearestDistance = distance;
		}
	}
	return nearest;
}

// 0x4120A0
Topping* CityMap::getRandomKitchenPizza() const
{
	if (m_kitchenPizzas.empty())
		return 0;
	return m_kitchenPizzas[engine::randomInt(0, m_kitchenPizzas.size() - 1)];
}

// 0x4120F0: no customer waits for a delivery (police popups aside) and no tip is left to collect
bool CityMap::allOrdersDone() const
{
	bool done = true;
	for (std::vector<engine::RefPtr<CustomerTile> >::const_iterator it = m_customers.begin(); it != m_customers.end(); ++it)
	{
		CustomerTile* customer = *it;
		if (!customer->isAvailable() && !customer->isPopupFinished() && customer->getPopup()->getPopupType() != 2)
			done = false;
	}
	for (std::vector<engine::RefPtr<Tip> >::const_iterator tip = m_tips.begin(); tip != m_tips.end(); ++tip)
	{
		if ((*tip)->m_clickable)
			done = false;
	}
	return done;
}

// 0x412170: the combo on the signs of the kitchens that make the topping, the others off
void CityMap::setKitchenSigns(Topping* topping, int combo, int unused)
{
	for (std::vector<engine::RefPtr<KitchenTile> >::iterator it = m_kitchens.begin(); it != m_kitchens.end(); ++it)
	{
		KitchenTile* kitchen = *it;
		if (kitchen->getTopping() == topping)
			kitchen->showCombo(combo, unused);
		else
			kitchen->showCombo(0, 0);
	}
}

// 0x4122E0
void CityMap::update(engine::UpdateContext& context)
{
	if (!m_frozen)
	{
		for (std::map<std::string, engine::RefPtr<Tile> >::iterator it = m_tilesByName.begin(); it != m_tilesByName.end(); ++it)
			it->second->update(context);
		for (std::vector<engine::RefPtr<Tip> >::iterator tip = m_tips.begin(); tip != m_tips.end(); ++tip)
			(*tip)->updateOnMap(context);
	}
	for (std::vector<engine::RefPtr<Vehicle> >::iterator vehicle = m_vehicles.begin(); vehicle != m_vehicles.end(); ++vehicle)
		(*vehicle)->updateOnMap(context);
	for (std::vector<engine::RefPtr<BoatSpawner> >::iterator spawner = m_boatSpawners.begin(); spawner != m_boatSpawners.end(); ++spawner)
		(*spawner)->update(context);
}

// 0x4124C0
std::string CityMap::makeTileName()
{
	std::string name;
	engine::format(name, "Tile%d", m_uniqueId++);
	return name;
}

// 0x412710
void CityMap::setBackground(const std::string& background, const std::string& hsvShift)
{
	m_background = background;
	m_hsvShift = hsvShift;
}

// 0x412740
void CityMap::removeDecoration(Decoration* decoration)
{
	m_decorations.erase(std::remove(m_decorations.begin(), m_decorations.end(), decoration));
}

// 0x4127B0
void CityMap::removeTip(Tip* tip)
{
	tip->deactivate();
	m_tips.erase(std::remove(m_tips.begin(), m_tips.end(), tip));
}

// 0x412820
void CityMap::removeVehicle(Vehicle* vehicle)
{
	m_vehicles.erase(std::remove(m_vehicles.begin(), m_vehicles.end(), vehicle));
}

// 0x412940
void CityMap::clearBoatSpawners()
{
	m_boatSpawners.clear();
}

// 0x413270: the kitchen pizzas stay
void CityMap::clear()
{
	m_cells.resize(0, 0);
	m_tilesByName.clear();
	m_kitchens.clear();
	m_customers.clear();
	m_roads.clear();
	m_decorations.clear();
	m_bridges.clear();
	m_tips.clear();
	m_boatSpawners.clear();
	m_vehicles.clear();
	m_uniqueId = 0;
	m_frozen = false;
}

// 0x413670
CityMap::~CityMap()
{
	clear();
}

// 0x4139D0
void CityMap::create(int cols, int rows, int cellSize, const engine::Point& gridOffset, int uniqueId)
{
	clear();
	m_cellSize = cellSize;
	m_rows = rows;
	m_cols = cols;
	m_gridOffset = gridOffset;
	m_uniqueId = uniqueId;
	m_cells.resize(cols, rows);
	for (int x = 0; x < m_cols; x++)
	{
		for (int y = 0; y < m_rows; y++)
			m_cells.m_rows[x][y] = 0;
	}
	m_pathFinder.setMap(this);
}

// 0x413A80
void CityMap::addDecoration(Decoration* decoration)
{
	m_decorations.push_back(decoration);
}

// 0x413B00
void CityMap::addTip(Tip* tip)
{
	m_tips.push_back(tip);
}

// 0x413B80
void CityMap::addVehicle(Vehicle* vehicle)
{
	m_vehicles.push_back(vehicle);
}

// 0x413C00: a customer without a popup, not next to a busy tile, and not the police station
CustomerTile* CityMap::getRandomCustomer() const
{
	std::vector<engine::RefPtr<CustomerTile> > candidates;
	for (std::vector<engine::RefPtr<CustomerTile> >::const_iterator it = m_customers.begin(); it != m_customers.end(); ++it)
	{
		CustomerTile* customer = *it;
		if (customer->isAvailable() && !isNextToBusyTile(customer) && customer->getCharacter()->m_special != 2)
			candidates.push_back(customer);
	}
	if (candidates.empty())
		return 0;
	return candidates[engine::randomInt(0, candidates.size() - 1)];
}

// 0x413D80: a customer whose character is special in that way (1 chameleon, 2 police)
CustomerTile* CityMap::getRandomCustomerOfType(int type) const
{
	std::vector<engine::RefPtr<CustomerTile> > candidates;
	for (std::vector<engine::RefPtr<CustomerTile> >::const_iterator it = m_customers.begin(); it != m_customers.end(); ++it)
	{
		CustomerTile* customer = *it;
		if (customer->getCharacter()->m_special == type)
			candidates.push_back(customer);
	}
	if (candidates.empty())
		return 0;
	return candidates[engine::randomInt(0, candidates.size() - 1)];
}

// 0x413F40
void CityMap::addBoatSpawner(BoatSpawner* spawner)
{
	m_boatSpawners.push_back(spawner);
}

// 0x413FC0: each kitchen's topping once
void CityMap::updateKitchenPizzas()
{
	m_kitchenPizzas.clear();
	for (std::vector<engine::RefPtr<KitchenTile> >::iterator it = m_kitchens.begin(); it != m_kitchens.end(); ++it)
	{
		Topping* topping = (*it)->getTopping();
		if (topping && std::find(m_kitchenPizzas.begin(), m_kitchenPizzas.end(), topping) == m_kitchenPizzas.end())
			m_kitchenPizzas.push_back(topping);
	}
}

// 0x414260
CityMap::CityMap()
{
	m_rows = 0;
	m_cols = 0;
	m_cellSize = 0;
	m_uniqueId = 0;
	setBackground("res\\Themes\\Suburbs\\bgPizzaSuburbs.jpg", "0,0,0");
	m_frozen = false;
}

// 0x4144B0
void CityMap::unregisterTile(Tile* tile)
{
	m_tilesByName.erase(tile->getId());
	if (tile->getClassName() == "kitchen")
	{
		m_kitchens.erase(std::remove(m_kitchens.begin(), m_kitchens.end(), tile));
		updateKitchenPizzas();
	}
	else if (tile->getClassName() == "roads")
		m_roads.erase(std::remove(m_roads.begin(), m_roads.end(), tile));
	else if (tile->getClassName() == "customer")
		m_customers.erase(std::remove(m_customers.begin(), m_customers.end(), tile));
	else if (tile->getClassName() == "bridge")
		m_bridges.erase(std::remove(m_bridges.begin(), m_bridges.end(), tile));
}

// 0x414720: empties the tile's footprint, which extends up and left from its cell. The loop conditions copy the
// dimensions with Point's copy constructor each time (the original read them through an inline by-value getter)
void CityMap::removeTile(Tile* tile)
{
	for (int y = 0; y < engine::Point(tile->m_dimensions).y; y++)
	{
		for (int x = 0; x < engine::Point(tile->m_dimensions).x; x++)
		{
			engine::Point cell = tile->getCell() - engine::Point(x, y);
			if (cell.x >= 0 && cell.x < m_cols && cell.y >= 0 && cell.y < m_rows)
				m_cells.m_rows[cell.x][cell.y] = 0;
		}
	}
	unregisterTile(tile);
}

// 0x414830
void CityMap::registerTile(Tile* tile)
{
	m_tilesByName[tile->getId()] = tile;
	if (tile->getClassName() == "kitchen")
		m_kitchens.push_back(static_cast<KitchenTile*>(tile));
	else if (tile->getClassName() == "roads")
		m_roads.push_back(tile);
	else if (tile->getClassName() == "customer")
		m_customers.push_back(static_cast<CustomerTile*>(tile));
	else if (tile->getClassName() == "bridge")
		m_bridges.push_back(static_cast<BridgeTile*>(tile));
}

// 0x414A30: the tile covers its footprint (removing the tiles it overlaps) and is registered
void CityMap::placeTile(const engine::Point& cell, Tile* tile)
{
	for (int y = 0; y < engine::Point(tile->m_dimensions).y; y++)
	{
		for (int x = 0; x < engine::Point(tile->m_dimensions).x; x++)
		{
			engine::Point covered = cell - engine::Point(x, y);
			if (covered.x >= 0 && covered.x < m_cols && covered.y >= 0 && covered.y < m_rows)
			{
				if (m_cells.m_rows[covered.x][covered.y])
					removeTile(m_cells.m_rows[covered.x][covered.y]);
				m_cells.m_rows[covered.x][covered.y] = tile;
			}
		}
	}
	if (tile)
		registerTile(tile);
}
