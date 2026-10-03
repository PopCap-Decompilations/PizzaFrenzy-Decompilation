// Canoe: the delivery vehicle of the VehicleType class "canoe".
#include <cmath>
#include <string>

#include "engine/Selector.h"
#include "Canoe.h"
#include "CustomerTile.h"
#include "PizzaFrenzy.h"
#include "TileManager.h"

// 0x431AA0
Canoe::~Canoe()
{
}

// 0x431AD0
Canoe::Canoe(const std::string& name)
	: Vehicle(name)
{
}

// 0x431B50
bool Canoe::init()
{
	VehicleType* def = PizzaFrenzy::getTileManifest()->getVehicleType(getName());
	if (def)
	{
		setDefinition(def);
		m_layer = 3;
		setSpeed((float)def->m_speed);
		m_states.setState(new VehicleParkedState(this));
		return true;
	}
	return false;
}

// 0x431C10
void Canoe::dispatchTo(CustomerTile* customer)
{
	m_target = customer;
	engine::Point dest = customer->getParkingCell();
	if (isIdle() && !(m_tile == dest))
	{
		setRoute(dest);
		if (m_path)
			m_states.setState(new VehicleDrivingState(this));
	}
}

// 0x431E40 (folded)
void Canoe::faceTowards(const engine::Point& tile)
{
	if (m_tile != tile)
	{
		engine::Vector2 d(tile - m_tile);
		d.normalize();
		if (fabs(d.y) <= fabs(d.x))
			m_selector->select(0, 0.0f, true);
		else if (d.y < 0.0f)
			m_selector->select(1, 0.0f, true);
		else
			m_selector->select(2, 0.0f, true);
		float scaleX = fabs(getScale().x);
		if (d.x < 0.0f)
			scaleX = -scaleX;
		setScale(scaleX, getScale().y);
	}
}
