// Copter: the delivery helicopter of the VehicleType class "copter", and its states (lift, parked, flying, arrived).
#pragma once

#include <string>

#include "engine/State.h"
#include "Vehicle.h"

class CustomerTile;
class Path;

// Made by KitchenTile::getIdleVehicle for class "copter": takes off, flies straight to the customer, lands, delivers
// after 1 s and reappears parked at home (HoverPath and ParkedPath images). The members end at +0x188; the vtordisp
// (+0x188) and the Interface subobject (+0x18C) follow them (0x190 bytes).
class Copter : public Vehicle
{
public:
	Copter(const std::string& name);
	virtual ~Copter();

	virtual bool init();										// slot 80 (Vehicle)
	virtual void dispatchTo(CustomerTile* customer);			// slot 85 (Vehicle)
	virtual Path* findPath(const engine::Point& destTile);		// slot 86 (Vehicle): a straight line
	virtual void faceTowards(const engine::Point& tile);		// slot 89 (Vehicle): folded body 0x431E40 (Canoe's too)

	engine::Container* m_body;									// +0x184 holds m_selector and m_fx; raw (a child)
};

// The copter states: engine::State<Copter> (owner at +0x0C), own members from +0x10, then the vtordisp and the
// Interface subobject. No user-declared destructors.

// Take-off or landing: lifts or lowers m_body by g_copterLiftHeight over g_copterLiftTime, then sets m_next (0x24
// bytes).
class CopterLiftState : public engine::State<Copter>
{
public:
	CopterLiftState(Copter* copter, bool ascending, engine::StateBase* next, bool returning);

	virtual void enter();										// slot 1 (engine::StateBase)
	virtual void exit();										// slot 2 (engine::StateBase)
	virtual void update(engine::UpdateContext& context);		// slot 3 (engine::StateBase)

	float m_time;												// +0x10 remaining lift time
	bool m_ascending;											// +0x14 true take-off, false landing
	bool m_returning;											// +0x15 exit notifies the owner (every caller passes false)
	engine::StateBase* m_next;									// +0x18 state set when the lift ends; raw
};

// Parked at home: parked image, fx off (0x18 bytes).
class CopterParkedState : public engine::State<Copter>
{
public:
	CopterParkedState(Copter* copter);

	virtual void enter();										// slot 1 (engine::StateBase)
};

// Flying the straight path; on the action "arrived" lands, then CopterArrivedState (0x18 bytes).
class CopterFlyingState : public engine::State<Copter>
{
public:
	CopterFlyingState(Copter* copter);

	virtual void enter();										// slot 1 (engine::StateBase)
	virtual void update(engine::UpdateContext& context);		// slot 3 (engine::StateBase): folded body 0x430850
	virtual void onAction(const std::string& action);			// slot 4 (engine::StateBase)
};

// 1 s at the customer, then delivers and parks at home (0x1C bytes).
class CopterArrivedState : public engine::State<Copter>
{
public:
	CopterArrivedState(Copter* copter);

	virtual void enter();										// slot 1 (engine::StateBase): folded body 0x42FE60
	virtual void update(engine::UpdateContext& context);		// slot 3 (engine::StateBase)

	float m_time;												// +0x10 remaining hover (1 s)
};
