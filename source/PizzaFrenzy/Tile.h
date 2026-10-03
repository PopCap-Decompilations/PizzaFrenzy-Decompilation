// Tile: the base of every map tile; DepthSortedContainer: the map layer that keeps its children in drawing order.
#pragma once

#include <string>
#include <vector>

#include "engine/Container.h"
#include "engine/Point.h"
#include "engine/Rect.h"
#include "engine/RefPtr.h"

namespace engine
{
	class Bitmap;
	class Properties;
	class XmlWriter;
}

class CityMap;
class TileType;

// The std::find_if predicate of DepthSortedContainer (its call is inlined into the find_if instantiation
// 0x41AB00): true for the first child whose bounds end lower than the point, or as low and further left.
struct DepthOrder
{
	float x;									// +0x00 right edge of the child being placed
	float y;									// +0x04 its bottom edge

	bool operator()(engine::Component* child) const
	{
		if (child->getBounds().bottom == y)
			return child->getBounds().right < x;
		return child->getBounds().bottom > y;
	}
};

// GameScreen's "GroundGroup": a container whose children stay sorted by the bottom (then right) edge of their
// bounds, so ground tiles and vehicles are drawn back to front. Constructed inline by GameScreen::init; implicit
// constructor and destructor (0x47BBF0 is the shared Container-derived deleting destructor).
class DepthSortedContainer : public engine::Container
{
public:
	virtual void updateChildOrder(engine::Component* child);			// slot 75

	// 0x436430: slot 76. An inline member: the original's copy was emitted in GameScreen.cpp, with the vtable of
	// the inline constructor.
	virtual std::vector<engine::Component*>& getChildren()
	{
		return m_children;
	}

	// overrides
	virtual void addChild(engine::Component* child);					// engine::Container slot 71
};

// Base of every map tile. The component's name is the tile type (TileManager key), m_className its manifest
// class; the tile draws itself in map layer 3 and puts popups, vehicles and signs in m_overlay (layer 5).
class Tile : public engine::Container
{
public:
	Tile(const std::string& typeName);
	virtual ~Tile();

	virtual bool setType(TileType* type, const std::string& id);			// slot 75
	virtual bool init(CityMap* map, const std::string& id) = 0;			// slot 76
	virtual engine::Component* getLayerComponent(int layer);				// slot 77

	// 0x41AFA0 (folded): slot 78; one body with the other one-argument "return true" functions
	virtual bool load(const engine::Properties& attrs)
	{
		return true;
	}

	virtual void addToMap(CityMap* map, const engine::Point& cell);		// slot 79
	virtual void setMapPosition(CityMap* map, const engine::Point& cell);	// slot 80

	// 0x4D0470 (folded): slot 81; the shared empty body
	virtual void updateConnections()
	{
	}

	virtual void removeFromMap();											// slot 82
	virtual void save(engine::XmlWriter& writer);							// slot 83
	virtual int getWeight(const engine::Point& cell) const;				// slot 84

	// 0x454E60 (folded): slot 85; one body with the other one-argument "return false" functions
	virtual bool isDrivable(const engine::Point& cell) const
	{
		return false;
	}

	// 0x454E60 (folded): slot 86
	virtual bool isBlocked(const engine::Point& cell) const
	{
		return false;
	}

	virtual engine::Point getCell() const;									// slot 87
	virtual engine::Point getFootprintOrigin() const;						// slot 88

	// 0x492310 (folded): slot 89; the shared empty body with one argument. A new virtual: it hides
	// engine::Container::update (slot 37) in Tile's scope, so tile->update(context) calls slot 89.
	virtual void update(const engine::UpdateContext& time)
	{
	}

	// 0x451610 (folded): slot 90; one body with the other argument-less "return true" functions
	virtual bool isAvailable() const
	{
		return true;
	}

	const std::string& getClassName() const;
	const std::string& getId() const;
	void setClassName(std::string className);

	std::string m_className;							// +0x128 customer, kitchen, bridge, roads...
	std::string m_id;									// +0x144 "name" in the map file, or "Tile%d"
	CityMap* m_map;										// +0x160 not reference counted
	engine::Point m_cell;								// +0x164 (-1,-1) until placed
	engine::Point m_offset;								// +0x16C TileType BROffset
	engine::Point m_base;								// +0x174 TileType base (footprint size)
	engine::Point m_dimensions;							// +0x17C TileType dimensions, (1,1) by default
	engine::RefPtr<engine::Bitmap> m_image;				// +0x184 TileType image
	engine::RefPtr<engine::Container> m_overlay;		// +0x188 component for map layer 5
	int m_width;										// +0x18C image width
	int m_height;										// +0x190 image height
};
