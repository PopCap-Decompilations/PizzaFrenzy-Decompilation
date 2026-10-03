// BridgeTile: the drawbridge map tile (<bridge>, class "bridge") that boats raise.
#pragma once

#include <string>

#include "engine/RefPtr.h"

#include "Tile.h"

namespace engine
{
	class Component;
	class Properties;
	class Selector;
	class XmlWriter;
}

class CityMap;

// Two selectors switch between the raised (roadPath / path) and the lowered (unblockedRoadPath / unblockedPath)
// images: m_roadImages is the tile's map layer 2 component, m_groundImages a child of the tile. A boat raises the
// bridge for 2 s unless a vehicle stands on it.
class BridgeTile : public Tile
{
public:
	BridgeTile(const std::string& name);
	virtual ~BridgeTile();

	virtual bool isOccupied();													// slot 91
	virtual void setBlocked(bool blocked);										// slot 92
	virtual bool requestBlocked(bool blocked);									// slot 93

	// overrides
	virtual bool init(CityMap* map, const std::string& id);					// Tile slot 76
	virtual engine::Component* getLayerComponent(int layer);					// Tile slot 77

	// 0x426FF0 (folded): Tile slot 78; the identical overrides of CustomerTile, BridgeTile and RoadTile share one body
	virtual bool load(const engine::Properties& attrs)
	{
		return Tile::load(attrs);
	}

	virtual void addToMap(CityMap* map, const engine::Point& cell);			// Tile slot 79
	virtual void save(engine::XmlWriter& writer);								// Tile slot 83

	// 0x426550 (folded): Tile slot 84, shared with RoadTile
	virtual int getWeight(const engine::Point& cell) const
	{
		return 1;
	}

	// 0x41AFA0 (folded): Tile slot 85; one body with Tile::load and the other one-argument "return true" functions
	virtual bool isDrivable(const engine::Point& cell) const
	{
		return true;
	}

	virtual bool isBlocked(const engine::Point& cell) const;					// Tile slot 86
	virtual void update(const engine::UpdateContext& time);					// Tile slot 89

	engine::RefPtr<engine::Selector> m_roadImages;		// +0x194 "RoadImages": roadPath / unblockedRoadPath (not a child)
	engine::RefPtr<engine::Selector> m_groundImages;	// +0x198 "GroundImages": path / unblockedPath (a child)
	float m_blockTimer;									// +0x19C seconds left raised; update() lowers it at 0
	bool m_blocked;										// +0x1A0 raised; returned by isBlocked
};
