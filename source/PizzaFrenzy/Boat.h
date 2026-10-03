// Boat: an ambient river boat launched by a BoatSpawner.
#pragma once

#include <string>

#include "engine/Point.h"
#include "Vehicle.h"

class BuildingTile;
class CityMap;

// Sails in a straight line (no state machine), waits for drawbridges to open, and speeds up when clicked; removed
// once past the screen edge. The members end at +0x198; the vtordisp (+0x198) and the Interface subobject (+0x19C)
// follow them (0x1A0 bytes).
class Boat : public Vehicle
{
public:
	Boat(const std::string& name);
	virtual ~Boat();

	virtual void start(CityMap* map, const engine::Point& pos, BuildingTile* owner);	// slot 90: pos in pixels
	virtual void setDirection(const engine::Vector2& dir);		// slot 91
	virtual bool canSailTo(const engine::Vector2& pos);			// slot 92: asks a bridge there to open

	virtual void onMouseDown(const engine::Vector2& position);	// slot 41 (engine::Component)
	virtual bool init();										// slot 80 (Vehicle)
	virtual void updateOnMap(engine::UpdateContext& ctx);		// slot 81 (Vehicle)
	virtual void faceTowards(const engine::Point& tile);		// slot 89 (Vehicle)

	void sail(engine::UpdateContext& ctx);

	engine::Vector2 m_velocity;									// +0x184 pixels per second
	int m_boostFrames;											// +0x18C frames of triple speed left (+10 per click)
	engine::Vector2 m_lookAhead;								// +0x190 normalized velocity * 25 (canSailTo probe)
};
