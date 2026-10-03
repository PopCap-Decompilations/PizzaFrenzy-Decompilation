// Canoe: the delivery vehicle of the VehicleType class "canoe".
#pragma once

#include <string>

#include "Vehicle.h"

class CustomerTile;

// Made by KitchenTile::getIdleVehicle for class "canoe": departs silently and heads along the dominant axis. Adds
// no members (0x18C bytes, Vehicle's layout).
class Canoe : public Vehicle
{
public:
	Canoe(const std::string& name);
	virtual ~Canoe();

	virtual bool init();										// slot 80 (Vehicle)
	virtual void dispatchTo(CustomerTile* customer);			// slot 85 (Vehicle)
	virtual void faceTowards(const engine::Point& tile);		// slot 89 (Vehicle): folded body 0x431E40 (Copter's)
};
