// BuildingTile: a map tile that shows a popup (order, kitchen or police bubble) and owns vehicles.
#pragma once

#include <string>
#include <vector>

#include "engine/Point.h"
#include "engine/RefPtr.h"

#include "Tile.h"

namespace engine
{
	class Image;
}

class PizzaPopup;
class Topping;
class Vehicle;

// Base of CustomerTile and KitchenTile: one popup shown above the building in the tile's overlay, a queued one
// shown when it is gone, and the vehicles parked at the building.
class BuildingTile : public Tile
{
public:
	BuildingTile(const std::string& typeName);
	virtual ~BuildingTile();

	virtual void popupRelease();											// slot 91
	virtual void popupMouseOut();											// slot 92
	virtual void popupReleaseIfHovered();									// slot 93
	virtual bool matchesTopping(Topping* topping) = 0;					// slot 94
	virtual bool isPopupActive() const;									// slot 95
	virtual void showHighlight(bool visible);								// slot 96

	// 0x492310 (folded): slot 97; the shared empty body with one argument (no override, no caller)
	virtual void onVehicleArrived(Vehicle* vehicle)
	{
	}

	virtual void showPopup(PizzaPopup* popup);								// slot 98
	virtual PizzaPopup* getPopup() const;									// slot 99
	virtual void clearPopup();												// slot 100
	virtual void removePopup();											// slot 101
	virtual void dismissPopup();											// slot 102
	virtual bool isPopupFinished() const;									// slot 103
	virtual Vehicle* addVehicle(const engine::Point& cell, Vehicle* vehicle);	// slot 104
	virtual engine::Point getParkingCell();								// slot 105
	virtual void removeVehicle(Vehicle* vehicle);							// slot 106

	// overrides
	virtual void onMouseDown();											// engine::Component slot 42
	virtual void update(const engine::UpdateContext& time);				// Tile slot 89
	virtual bool isAvailable() const;										// Tile slot 90

	void detachVehicle(Vehicle* vehicle);
	void removeAllVehicles();

	engine::RefPtr<engine::Image> m_highlight;			// +0x194 footprint highlight (showHighlight)
	engine::Point m_parkingSpot;						// +0x198 tile type "parkingspot", relative to the cell
	std::vector<engine::RefPtr<Vehicle> > m_vehicles;	// +0x1A0
	engine::RefPtr<PizzaPopup> m_popup;					// +0x1B0 popup shown above the building
	engine::RefPtr<PizzaPopup> m_nextPopup;				// +0x1B4 queued popup, shown by update()
};
