// Vehicle: a vehicle driving tile paths on the city map, and its states (parked, driving, arrived).
#pragma once

#include <string>
#include <vector>

#include "engine/Container.h"
#include "engine/Point.h"
#include "engine/RefPtr.h"
#include "engine/State.h"
#include "engine/StateMachine.h"

namespace engine
{
	class Graphics;
	class ParticleSystem;
	class Selector;
}

class BuildingTile;
class CityMap;
class CustomerTile;
class Path;
class Tile;
class VehicleType;

// The kitchens' delivery van or car as is (KitchenTile::getIdleVehicle); base of Canoe, Copter, PoliceCar, Blimp
// and Boat. Its name selects the VehicleType (EW/N/S images, fx, speed). The members end at +0x184; the vtordisp
// (+0x184) and the Interface subobject (+0x188) follow them (0x18C bytes).
class Vehicle : public engine::Container
{
public:
	Vehicle(const std::string& name);
	virtual ~Vehicle();

	virtual bool canTravelOn(Tile* tile);						// slot 75: folded body 0x41AFA0 (return true)
	virtual void onEnterTile(Tile* tile);						// slot 76: folded body 0x492310 (empty)
	virtual void placeOnMap(CityMap* map, const engine::Point& homeTile, BuildingTile* owner);	// slot 77
	virtual void setTile(const engine::Point& tile);			// slot 78
	virtual void removeFromMap();								// slot 79: folded body 0x42F770
	virtual bool init();										// slot 80
	virtual void updateOnMap(engine::UpdateContext& ctx);		// slot 81: called by CityMap::update
	virtual void onArrived();									// slot 82
	virtual void completeDelivery();							// slot 83
	virtual void setRoute(const engine::Point& destTile);		// slot 84
	virtual void dispatchTo(CustomerTile* customer);			// slot 85
	virtual Path* findPath(const engine::Point& destTile);		// slot 86
	virtual void faceRoad();									// slot 87
	virtual void setDefinition(VehicleType* def);				// slot 88
	virtual void faceTowards(const engine::Point& tile);		// slot 89

	virtual void draw(engine::Graphics& g);						// slot 36 (engine::Component): folded body 0x471110
	virtual void onRemovedFrom(engine::Container* parent);		// slot 61 (engine::Component)

	void clearZOrderDirty();
	bool isZOrderDirty() const;
	void setSpeed(float speed);
	static bool isDrivableTile(Tile* tile, const engine::Point& pos);	// path finder predicate
	bool isIdle() const;
	void setFxActive(bool active);
	void driveAlongPath(engine::UpdateContext& ctx);

	CityMap* m_map;												// +0x128
	engine::Point m_tile;										// +0x12C current cell
	engine::Point m_homeTile;									// +0x134 parking cell at the owner (placeOnMap)
	engine::RefPtr<Path> m_path;								// +0x13C current route; null = idle
	float m_segmentProgress;									// +0x140 0..1 along the current segment
	float m_speed;												// +0x144 pixels per second (200, VehicleType speed)
	std::vector<engine::Point>::const_iterator m_nextWaypoint;	// +0x148 into m_path's cells; end() = arrived
	bool m_zOrderDirty;											// +0x14C (int)y changed or parked; GameScreen re-sorts
	engine::Point m_prevWaypoint;								// +0x150 start of the current segment
	int m_layer;												// +0x158 display layer: 3 ground, 4 air
	CustomerTile* m_target;										// +0x15C customer being served (dispatchTo)
	BuildingTile* m_owner;										// +0x160 kitchen or police station (placeOnMap)
	engine::RefPtr<engine::Selector> m_selector;				// +0x164 0 EW, 1 N, 2 S (Copter: 3 hover, 4 parked)
	engine::RefPtr<engine::ParticleSystem> m_fx;				// +0x168 exhaust/wake particles; may be null
	engine::StateMachine m_states;								// +0x16C parked, driving, arrived (0x18 bytes)
};

// The vehicle states: engine::State<Vehicle> (owner at +0x0C), own members from +0x10, then the vtordisp and the
// Interface subobject. No user-declared destructors.

// Idle at the home cell: fx off, parked image, faces the adjacent road (0x18 bytes).
class VehicleParkedState : public engine::State<Vehicle>
{
public:
	VehicleParkedState(Vehicle* vehicle);

	virtual void enter();										// slot 1 (engine::StateBase)
};

// Driving the path; on the action "arrived" switches to VehicleArrivedState (0x18 bytes).
class VehicleDrivingState : public engine::State<Vehicle>
{
public:
	VehicleDrivingState(Vehicle* vehicle);

	virtual void enter();										// slot 1 (engine::StateBase)
	virtual void update(engine::UpdateContext& context);		// slot 3 (engine::StateBase): folded body 0x430850
	virtual void onAction(const std::string& action);			// slot 4 (engine::StateBase)
};

// 1 s stop at the customer ("vehicle_arrive"), then delivers and reappears parked at home (0x1C bytes).
class VehicleArrivedState : public engine::State<Vehicle>
{
public:
	VehicleArrivedState(Vehicle* vehicle);

	virtual void enter();										// slot 1 (engine::StateBase): folded body 0x42FE60
	virtual void update(engine::UpdateContext& context);		// slot 3 (engine::StateBase)

	float m_time;												// +0x10 remaining stop (1 s)
};
