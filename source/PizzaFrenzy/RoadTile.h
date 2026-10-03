// RoadTile: a road map tile (<road>, class "roads") that picks its road piece from its four neighbours.
#pragma once

#include <string>

#include "Tile.h"

namespace engine
{
	class Component;
	class Image;
	class Properties;
	class XmlWriter;
}

class CityMap;

// The road set "Water" (the boat lanes) is invisible except in the debug view.
class RoadTile : public Tile
{
public:
	RoadTile(const std::string& name);
	virtual ~RoadTile();

	// overrides
	virtual bool init(CityMap* map, const std::string& id);					// Tile slot 76
	virtual engine::Component* getLayerComponent(int layer);					// Tile slot 77

	// 0x426FF0 (folded): Tile slot 78; the identical overrides of CustomerTile, BridgeTile and RoadTile share one body
	virtual bool load(const engine::Properties& attrs)
	{
		return Tile::load(attrs);
	}

	virtual void addToMap(CityMap* map, const engine::Point& cell);			// Tile slot 79
	virtual void updateConnections();											// Tile slot 81
	virtual void save(engine::XmlWriter& writer);								// Tile slot 83

	// 0x426550 (folded): Tile slot 84, shared with BridgeTile
	virtual int getWeight(const engine::Point& cell) const
	{
		return 1;
	}

	// 0x41AFA0 (folded): Tile slot 85; one body with Tile::load and the other one-argument "return true" functions
	virtual bool isDrivable(const engine::Point& cell) const
	{
		return true;
	}

	engine::Image* m_imageItem;							// +0x194 child image of the current road piece (not counted)
	int m_weight;										// +0x198 1; saved as "weight"
};
