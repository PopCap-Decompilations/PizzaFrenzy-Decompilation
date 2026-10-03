// BoatSpawner: launches a Boat along one river route of the level at random intervals.
#pragma once

#include "engine/Object.h"

namespace engine
{
	struct UpdateContext;
}

class CityMap;
struct BoatInfo;

// One per <boat> of the level (Level::createBoatSpawners), kept and updated by the CityMap. The members end at +0x18;
// the vtordisp (+0x18) and the Interface subobject (+0x1C) follow them (0x20 bytes). No user-declared destructor.
class BoatSpawner : public engine::Object
{
public:
	BoatSpawner(CityMap* map, const BoatInfo* route);

	virtual void update(engine::UpdateContext& ctx);			// slot 1: called by CityMap::update
	virtual void spawn();										// slot 2: creates and launches one Boat

	const BoatInfo* m_route;									// +0x0C the level's route: name, pos, delay, direction
	float m_timer;												// +0x10 seconds until the next spawn
	CityMap* m_map;												// +0x14
};
