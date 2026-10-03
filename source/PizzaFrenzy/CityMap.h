// CityMap: the playing field of a level (res\cities\*.xml), and g_zeroPoint.
#pragma once

#include <map>
#include <string>
#include <vector>

#include "engine/Object.h"
#include "engine/Point.h"
#include "engine/RefPtr.h"

#include "Array2D.h"
#include "PathFinder.h"

namespace engine
{
	struct UpdateContext;
}

class BoatSpawner;
class BridgeTile;
class CustomerTile;
class Decoration;
class KitchenTile;
class Path;
class Tile;
class Tip;
class Topping;
class Vehicle;

// Point(0, 0) at 0x532930 (dynamic initializer 0x4F8840): the default grid offset of a city (CityHandler,
// CityEditor::newCity)
extern engine::Point g_zeroPoint;

// The grid of tiles (multi-cell tiles fill their whole footprint), the tiles by name and by class, the tips,
// vehicles, boat spawners and decorations on the map, its background and the path finder. The game's instance is
// game+0x11C, filled by CityHandler. The members end at +0x13D; the vtordisp (+0x140) and the Interface subobject
// (+0x144) follow them (0x148 bytes).
class CityMap : public engine::Object
{
public:
	CityMap();
	virtual ~CityMap();

	virtual void update(engine::UpdateContext& context);	// slot 1: tiles and tips (unless frozen), vehicles, boat spawners

	// 0x40DDC0
	std::string getBackground() const
	{
		return m_background;
	}

	int getRows() const;
	int getCols() const;
	int getCellSize() const;
	Path* findPath(const engine::Point& start, const engine::Point& goal, bool (*isPassable)(Tile* tile, const engine::Point& cell));
	std::vector<engine::RefPtr<Topping> >& getKitchenPizzas();
	engine::Point cellToScreen(const engine::Point& cell) const;
	engine::Point screenToCell(engine::Point pos) const;
	Tile* getTile(const engine::Point& cell) const;
	bool isRoadAt(const engine::Point& cell) const;
	bool isBlockedAt(const engine::Point& cell) const;
	bool isNextToBusyTile(Tile* tile) const;
	Tile* findNearestRoad(const engine::Point& cell, const engine::Point& range) const;
	Topping* getRandomKitchenPizza() const;
	bool allOrdersDone() const;
	void setKitchenSigns(Topping* topping, int combo, int unused);
	std::string makeTileName();
	void setBackground(const std::string& background, const std::string& hsvShift);
	void removeDecoration(Decoration* decoration);
	void removeTip(Tip* tip);
	void removeVehicle(Vehicle* vehicle);
	void clearBoatSpawners();
	void clear();
	void create(int cols, int rows, int cellSize, const engine::Point& gridOffset, int uniqueId);
	void addDecoration(Decoration* decoration);
	void addTip(Tip* tip);
	void addVehicle(Vehicle* vehicle);
	CustomerTile* getRandomCustomer() const;
	CustomerTile* getRandomCustomerOfType(int type) const;
	void addBoatSpawner(BoatSpawner* spawner);
	void updateKitchenPizzas();
	void unregisterTile(Tile* tile);
	void removeTile(Tile* tile);
	void registerTile(Tile* tile);
	void placeTile(const engine::Point& cell, Tile* tile);

	// 0x435C20
	std::string getHsvShift() const
	{
		return m_hsvShift;
	}

	int m_rows;														// +0x0C "gridSize" y (24)
	int m_cols;														// +0x10 "gridSize" x (28)
	int m_cellSize;													// +0x14 "cellSize" (25)
	engine::Point m_gridOffset;										// +0x18 "gridOffset"
	Array2D<engine::RefPtr<Tile> > m_cells;							// +0x20 [x][y]: the tile covering each cell
	std::map<std::string, engine::RefPtr<Tile> > m_tilesByName;		// +0x2C by "name" ("Tile59")
	std::vector<engine::RefPtr<KitchenTile> > m_kitchens;			// +0x38 tiles of class "kitchen"
	std::vector<engine::RefPtr<CustomerTile> > m_customers;			// +0x48 tiles of class "customer"
	std::vector<engine::RefPtr<Tile> > m_roads;						// +0x58 tiles of class "roads"
	std::vector<engine::RefPtr<Tip> > m_tips;						// +0x68 tips lying on the map
	std::vector<engine::RefPtr<BridgeTile> > m_bridges;				// +0x78 tiles of class "bridge"
	std::vector<engine::RefPtr<BoatSpawner> > m_boatSpawners;		// +0x88 one per <boat> of the level
	int m_uniqueId;													// +0x98 "uniqueID": next number for "Tile%d"
	std::vector<engine::RefPtr<Topping> > m_kitchenPizzas;			// +0x9C the toppings the kitchens make, each once
	std::vector<engine::RefPtr<Decoration> > m_decorations;			// +0xAC
	std::vector<engine::RefPtr<Vehicle> > m_vehicles;				// +0xBC
	std::string m_background;										// +0xCC "background"
	std::string m_hsvShift;											// +0xE8 "hsvShift" ("%f,%f,%f")
	PathFinder m_pathFinder;										// +0x104
	bool m_frozen;													// +0x13C when set, tiles and tips are not updated; only ever cleared
};
