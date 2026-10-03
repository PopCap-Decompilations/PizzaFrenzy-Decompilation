// KitchenTile: a pizza kitchen on the map (tile class "kitchen").
#include <string>
#include <vector>

#include "engine/Application.h"
#include "engine/Blink.h"
#include "engine/Container.h"
#include "engine/HsvFilter.h"
#include "engine/Image.h"
#include "engine/Selector.h"
#include "engine/StringUtil.h"
#include "engine/Surface.h"
#include "engine/XmlWriter.h"

#include "Canoe.h"
#include "CityMap.h"
#include "Constants.h"
#include "Copter.h"
#include "CustomerTile.h"
#include "KitchenTile.h"
#include "PizzaFrenzy.h"
#include "PizzaKitchenPopup.h"
#include "TileManager.h"
#include "Vehicle.h"

// 0x41C610
void KitchenTile::removeFromMap()
{
	m_map->removeTile(this);
	removeAllVehicles();
	setFlags(0x10);
}

// 0x41C640
void KitchenTile::setTopping(Topping* topping)
{
	setVisible(true);
	removePopup();
	PizzaKitchenPopup* popup = new PizzaKitchenPopup;
	popup->init(this);
	popup->setTopping(topping);
	popup->setPosition(0.0f, 16.0f);
	showPopup(popup);
}

// 0x41C6F0
Topping* KitchenTile::getTopping()
{
	if (m_popup)
		return static_cast<PizzaKitchenPopup*>(getPopup())->getTopping();
	return NULL;
}

// 0x41C720
void KitchenTile::addToMap(CityMap* map, const engine::Point& cell)
{
	Tile::addToMap(map, cell);
	m_popup = NULL;
}

// 0x41C760
bool KitchenTile::matchesTopping(Topping* topping)
{
	if (m_popup && m_popup->getPopupType() == 1)
		return static_cast<PizzaKitchenPopup*>(getPopup())->getTopping() == topping;
	return false;
}

// 0x41C7B0
void KitchenTile::update(const engine::UpdateContext& time)
{
	BuildingTile::update(time);
	if (m_flashTime > 0.0f)
	{
		m_flashTime -= time.elapsed;
		if (m_flashTime <= 0.0f)
		{
			m_flashTime = 0.0f;
			m_marqueeLights->removeAllAnimators();
			m_marqueeSign->removeAllAnimators();
			m_marqueeLights->setVisible(true);
			m_marqueeSign->setVisible(true);
		}
	}
}

// 0x41C840
KitchenTile::~KitchenTile()
{
	m_marqueeLights = NULL;
	m_popup = NULL;
}

// 0x41C9E0
Vehicle* KitchenTile::getIdleVehicle()
{
	for (std::vector<engine::RefPtr<Vehicle> >::iterator it = m_vehicles.begin(); it != m_vehicles.end(); ++it)
	{
		Vehicle* vehicle = *it;
		if (vehicle->isIdle())
			return vehicle;
	}
	VehicleType* type = PizzaFrenzy::getTileManifest()->getVehicleType(m_vehicleType);
	Vehicle* vehicle;
	if (type->m_class == "copter")
		vehicle = new Copter(m_vehicleType);
	else if (type->m_class == "canoe")
		vehicle = new Canoe(m_vehicleType);
	else
		vehicle = new Vehicle(m_vehicleType);
	vehicle->init();
	return addVehicle(getParkingCell(), vehicle);
}

// 0x41CB80
void KitchenTile::showCombo(int combo, int unused)
{
	m_marqueeLights->removeAllAnimators();
	m_marqueeSign->removeAllAnimators();
	int level = (combo - 1) / 5;
	int hues[7] = { 0, 62, 45, 120, -65, -180, 0 };
	// the original's optimiser turned this loop into level -= 6 * ((unsigned)(level - 1) / 6)
	while (level >= 7)
		level -= 6;
	if (level > 0)
	{
		engine::Bitmap* image = engine::getApplication()->getImage("res\\fx\\kitchenMarquis4.jpg");
		image->setPivotType(1);
		m_signBitmap = image->copy();
		m_marqueeSign->setImage(m_signBitmap);
		engine::HsvFilter filter;
		filter.set((float)hues[level - 1], 0.0f, 0.0f);
		m_signBitmap->applyFilter(&filter);
	}
	else
		m_marqueeSign->setImage(NULL);

	engine::HsvFilter lightFilter;
	lightFilter.set((float)hues[level], 0.0f, 0.0f);
	std::string path;
	for (int i = 0; i < g_frenzyComboSize; i++)
	{
		engine::format(path, "res\\fx\\kitchenMarquis%d.jpg", i);
		engine::Bitmap* image = engine::getApplication()->getImage(path.c_str());
		image->setPivotType(1);
		m_lightBitmaps[i] = image->copy();
		m_lightBitmaps[i]->applyFilter(&lightFilter);
		m_lightImages[i]->setImage(m_lightBitmaps[i]);
	}

	if (combo == 0)
	{
		m_marqueeLights->select(0, 0.0f, true);
		m_flashTime = 0.0f;
	}
	else
	{
		int lights = combo % g_frenzyComboSize;
		m_flashTime = g_comboShowTime;
		engine::Blink* blink = new engine::Blink(g_comboFadeTime, g_comboDelay);
		if (lights == 0)
			m_marqueeLights->select(5, 0.0f, true);
		else
			m_marqueeLights->select(lights, 0.0f, true);
		m_marqueeLights->addAnimator(blink);
	}
}

// 0x41CF50
void KitchenTile::dispatchVehicle(CustomerTile* customer)
{
	Vehicle* vehicle = getIdleVehicle();
	if (vehicle)
	{
		vehicle->dispatchTo(customer);
		customer->onVehicleDispatched();
	}
	if (m_popup)
		m_popup->onDispatched();
}

// 0x41CF90
KitchenTile::KitchenTile(const std::string& typeName)
	: BuildingTile(typeName)
{
	setClassName("kitchen");
	m_vehicleType = "PizzaVan";
}

// 0x41D0C0
void KitchenTile::save(engine::XmlWriter& writer)
{
	writer.startElement("kitchen");
	Tile::save(writer);
	writer.endElement();
}

// 0x41D170
void KitchenTile::setVehicleType(const std::string& vehicleType)
{
	m_vehicleType = vehicleType;
}

// 0x41D190
bool KitchenTile::init(CityMap* map, const std::string& id)
{
	// unlike CustomerTile::init, a missing tile type still gets its overlay and returns true
	KitchenTileType* type = static_cast<KitchenTileType*>(PizzaFrenzy::getTileManifest()->getTile(getName(), m_className));
	if (type)
	{
		addChild(new engine::Image(type->m_image));
		m_parkingSpot = type->m_parkingSpot;
		Tile::setType(type, id);

		m_signBitmap = NULL;
		m_marqueeSign = new engine::Image(m_signBitmap);
		m_marqueeSign->setPosition(4.0f, -38.0f);
		addChild(m_marqueeSign);
		m_lastCombo = -1;

		m_marqueeLights = new engine::Selector;
		m_marqueeLights->addChild(new engine::Image(NULL));
		m_marqueeLights->setPosition(m_marqueeSign->getPosition());
		addChild(m_marqueeLights);
		for (int i = 0; i < g_frenzyComboSize; i++)
		{
			m_lightBitmaps[i] = NULL;
			m_lightImages[i] = new engine::Image(m_lightBitmaps[i]);
			m_marqueeLights->addChild(m_lightImages[i]);
		}

		engine::Bitmap* border = engine::getApplication()->getImage("res\\fx\\signBorder.jpg");
		border->setPivotType(1);
		engine::Image* borderImage = new engine::Image(border);
		borderImage->setPosition(m_marqueeSign->getPosition());
		addChild(borderImage);
	}
	m_overlay = new engine::Container;
	m_flashTime = 0.0f;
	return true;
}
