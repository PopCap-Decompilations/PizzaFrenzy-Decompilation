// KitchenTile: a pizza kitchen on the map (tile class "kitchen").
#pragma once

#include <string>

#include "engine/Point.h"
#include "engine/RefPtr.h"

#include "BuildingTile.h"

namespace engine
{
	class Bitmap;
	class Image;
	class Selector;
	class XmlWriter;
}

class CityMap;
class CustomerTile;
class Topping;
class Vehicle;

// Makes the topping of its kitchen popup, sends its vehicles (m_vehicleType, "PizzaVan" by default) to the
// customers, and shows the combo count with the marquee lights of its sign.
class KitchenTile : public BuildingTile
{
public:
	KitchenTile(const std::string& typeName);
	virtual ~KitchenTile();

	virtual void dispatchVehicle(CustomerTile* customer);					// slot 107

	// overrides
	virtual bool init(CityMap* map, const std::string& id);				// Tile slot 76
	virtual void addToMap(CityMap* map, const engine::Point& cell);		// Tile slot 79
	virtual void removeFromMap();											// Tile slot 82
	virtual void save(engine::XmlWriter& writer);							// Tile slot 83
	virtual void update(const engine::UpdateContext& time);				// Tile slot 89
	virtual bool matchesTopping(Topping* topping);							// BuildingTile slot 94

	// 0x492310 (folded): BuildingTile slot 106; the shared empty body with one argument (the kitchen keeps its
	// vehicles)
	virtual void removeVehicle(Vehicle* vehicle)
	{
	}

	void setTopping(Topping* topping);
	Topping* getTopping();
	Vehicle* getIdleVehicle();
	void showCombo(int combo, int unused);
	void setVehicleType(const std::string& vehicleType);

	std::string m_vehicleType;							// +0x1B8 vehicle type name
	engine::RefPtr<engine::Selector> m_marqueeLights;	// +0x1D4 frame = number of lights lit (0-5)
	engine::RefPtr<engine::Image> m_marqueeSign;		// +0x1D8 sign image at (4,-38)
	engine::RefPtr<engine::Image> m_lightImages[5];		// +0x1DC frames 1-5 of m_marqueeLights
	engine::RefPtr<engine::Bitmap> m_lightBitmaps[5];	// +0x1F0 res\fx\kitchenMarquis%d.jpg, hue-shifted
	engine::RefPtr<engine::Bitmap> m_signBitmap;		// +0x204 hue-shifted kitchenMarquis4.jpg
	float m_flashTime;									// +0x208 1.5 s after a combo change
	int m_lastCombo;									// +0x20C -1 after init, never read
};
