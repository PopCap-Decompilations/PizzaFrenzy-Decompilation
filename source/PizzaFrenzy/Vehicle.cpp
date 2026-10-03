// Vehicle: a vehicle driving tile paths on the city map, and its states (parked, driving, arrived).
#include <string>
#include <vector>

#include "engine/Image.h"
#include "engine/ParticleSystem.h"
#include "engine/Selector.h"
#include "engine/SoundMgr.h"
#include "engine/Surface.h"
#include "BuildingTile.h"
#include "CityMap.h"
#include "CustomerTile.h"
#include "PathFinder.h"
#include "PizzaFrenzy.h"
#include "Tile.h"
#include "TileManager.h"
#include "Vehicle.h"

// 0x41AFA0 (folded)
bool Vehicle::canTravelOn(Tile* tile)
{
	return true;
}

// 0x492310 (folded)
void Vehicle::onEnterTile(Tile* tile)
{
}

// 0x471110 (folded)
void Vehicle::draw(engine::Graphics& g)
{
	engine::Container::draw(g);
}

// 0x42F650
void Vehicle::onRemovedFrom(engine::Container* parent)
{
	engine::Container::onRemovedFrom(parent);
	m_map->removeVehicle(this);
}

// 0x42F670
void Vehicle::clearZOrderDirty()
{
	m_zOrderDirty = false;
}

// 0x42F680
bool Vehicle::isZOrderDirty() const
{
	return m_zOrderDirty;
}

// 0x42F690
void Vehicle::placeOnMap(CityMap* map, const engine::Point& homeTile, BuildingTile* owner)
{
	m_map = map;
	m_owner = owner;
	m_homeTile = homeTile;
	setTile(homeTile);
	updateBounds();
	m_map->addVehicle(this);
	faceRoad();
	m_zOrderDirty = false;
}

// 0x42F700
void Vehicle::faceRoad()
{
	for (int i = 0; i < 4; i++)
	{
		engine::Point cell = m_tile + g_directions[i];
		if (m_map && m_map->isRoadAt(cell))
		{
			faceTowards(cell);
			return;
		}
	}
}

// 0x42F770 (folded)
void Vehicle::removeFromMap()
{
	setFlags(0x10);
}

// 0x42F780
void Vehicle::updateOnMap(engine::UpdateContext& ctx)
{
	m_states.update(ctx);
	engine::Container::update(ctx);
}

// 0x42F7B0
void Vehicle::setSpeed(float speed)
{
	m_speed = speed;
}

// 0x42F7C0
bool Vehicle::isDrivableTile(Tile* tile, const engine::Point& pos)
{
	return tile && tile->isDrivable(pos);
}

// 0x42F7F0
Vehicle::~Vehicle()
{
}

// 0x42F8F0
bool Vehicle::isIdle() const
{
	return m_path == 0;
}

// 0x42F900
Path* Vehicle::findPath(const engine::Point& destTile)
{
	return m_map->findPath(m_tile, destTile, &Vehicle::isDrivableTile);
}

// 0x42F940
void Vehicle::setDefinition(VehicleType* def)
{
	ParticleFx* fx = PizzaFrenzy::getTileManifest()->getFx(def->m_fx);
	if (fx)
	{
		engine::ParticleSystemDef* effect = fx->m_effect;
		m_fx = new engine::ParticleSystem();
		m_fx->load(effect);
		addChild(m_fx);
	}
	m_selector = new engine::Selector();
	addChild(m_selector);
	def->m_image->setPivotType(1);
	m_selector->addChild(new engine::Image(def->m_image));
	def->m_northImage->setPivotType(1);
	m_selector->addChild(new engine::Image(def->m_northImage));
	def->m_southImage->setPivotType(1);
	m_selector->addChild(new engine::Image(def->m_southImage));
	m_selector->select(0, 0.0f, true);
}

// 0x42FB80
void Vehicle::setTile(const engine::Point& tile)
{
	if (m_map)
	{
		engine::Point pos = m_map->cellToScreen(tile);
		setPosition((float)(pos.x - m_map->getCellSize() / 2), (float)(pos.y - m_map->getCellSize() / 2));
		m_tile = tile;
		m_selector->select(0, 0.0f, true);
		setScale(1.0f);
	}
}

// 0x42FC30
void Vehicle::faceTowards(const engine::Point& tile)
{
	engine::Point fxOffset;
	if (m_tile != tile)
	{
		engine::Point d = tile - m_tile;
		if (d.x != 0)
		{
			m_selector->select(0, 0.0f, true);
			fxOffset.x = -10;
			fxOffset.y = 0;
		}
		else
		{
			fxOffset.x = 0;
			if (d.y < 0)
			{
				fxOffset.y = 10;
				m_selector->select(1, 0.0f, true);
			}
			else
			{
				fxOffset.y = -10;
				m_selector->select(2, 0.0f, true);
			}
		}
		float scaleX = 1.0f;
		if (d.x < 0)
		{
			scaleX = -1.0f;
			fxOffset.x = 10;
		}
		setScale(scaleX, 1.0f);
		if (m_fx != 0)
			m_fx->setPosition(engine::Vector2(fxOffset));
	}
}

// 0x42FD20
void Vehicle::completeDelivery()
{
	m_target->deliverOrder();
	m_path = 0;
	m_owner->removeVehicle(this);
}

// 0x42FD70
void Vehicle::setFxActive(bool active)
{
	if (m_fx)
	{
		if (active)
			m_fx->start();
		else
			m_fx->stop();
	}
}

// 0x42FDA0
void VehicleParkedState::enter()
{
	m_owner->m_path = 0;
	m_owner->setFxActive(false);
	if (m_owner->m_selector->getCount() > 4)
		m_owner->m_selector->select(4, 0.0f, true);
	m_owner->setTile(m_owner->m_homeTile);
	m_owner->faceRoad();
	m_owner->m_zOrderDirty = true;
}

// 0x42FE40
void VehicleDrivingState::enter()
{
	m_owner->setFxActive(true);
}

// 0x42FE60 (folded)
void VehicleArrivedState::enter()
{
	m_time = 1.0f;
	if (m_owner->m_selector->getCount() > 3)
		m_owner->m_selector->select(3, 0.0f, true);
	m_owner->onArrived();
}

// 0x42FEB0
VehicleParkedState::VehicleParkedState(Vehicle* vehicle)
	: engine::State<Vehicle>(vehicle)
{
}

// 0x42FF40
VehicleDrivingState::VehicleDrivingState(Vehicle* vehicle)
	: engine::State<Vehicle>(vehicle)
{
}

// 0x42FFD0
VehicleArrivedState::VehicleArrivedState(Vehicle* vehicle)
	: engine::State<Vehicle>(vehicle)
{
}

// 0x430070
bool Vehicle::init()
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

// 0x430130
void Vehicle::setRoute(const engine::Point& destTile)
{
	m_path = findPath(destTile);
	if (m_path)
	{
		m_prevWaypoint = m_tile;
		m_nextWaypoint = m_path->m_points.begin() + 1;
		m_segmentProgress = 0.0f;
		faceTowards(*m_nextWaypoint);
	}
}

// 0x4301C0
void VehicleArrivedState::update(engine::UpdateContext& context)
{
	m_time -= context.elapsed;
	if (m_time <= 0.0f)
	{
		m_owner->completeDelivery();
		m_owner->m_states.setState(new VehicleParkedState(m_owner));
	}
}

// 0x430260
void VehicleDrivingState::onAction(const std::string& action)
{
	if (action == "arrived")
		m_owner->m_states.setState(new VehicleArrivedState(m_owner));
}

// 0x4302F0
void Vehicle::dispatchTo(CustomerTile* customer)
{
	m_target = customer;
	engine::Point dest = customer->getParkingCell();
	if (isIdle() && !(m_tile == dest))
	{
		if (m_speed <= 200.0f)
			PizzaFrenzy::getSounds()->playSound("vehicle_leave_van", 1.0f, 1.0f);
		else
			PizzaFrenzy::getSounds()->playSound("vehicle_leave_car", 1.0f, 1.0f);
		setRoute(dest);
		if (m_path)
			m_states.setState(new VehicleDrivingState(this));
	}
}

// 0x430480
void Vehicle::onArrived()
{
	PizzaFrenzy::getSounds()->playSound("vehicle_arrive", 1.0f, 1.0f);
}

// 0x430510
void Vehicle::driveAlongPath(engine::UpdateContext& ctx)
{
	int startY = (int)getY();
	float dist = m_speed * ctx.elapsed;
	while (dist > 0.0f)
	{
		engine::Point next = *m_nextWaypoint;
		engine::Vector2 dir(next - m_prevWaypoint);
		float segLen = (float)m_map->getCellSize() * dir.length();
		float remaining = (1.0f - m_segmentProgress) * segLen;
		if (remaining <= dist)
		{
			dist -= remaining;
			m_prevWaypoint = *m_nextWaypoint;
			m_tile = *m_nextWaypoint;
			engine::Point pos = m_map->cellToScreen(m_prevWaypoint);
			setPosition((float)(pos.x - m_map->getCellSize() / 2), (float)(pos.y - m_map->getCellSize() / 2));
			m_segmentProgress = 0.0f;
			++m_nextWaypoint;
			if (m_nextWaypoint == m_path->m_points.end())
			{
				m_states.handleCommand("arrived");
				break;
			}
			faceTowards(*m_nextWaypoint);
		}
		else
		{
			engine::Vector2 pos(getPosition());
			dir.normalize();
			pos += dir * dist;
			engine::Point cell = m_map->screenToCell(pos + engine::Vector2(m_map->getCellSize() * 0.5f, m_map->getCellSize() * 0.5f));
			if (!m_map->isBlockedAt(cell))
			{
				setPosition(pos);
				m_tile = cell;
				m_segmentProgress += dist / segLen;
			}
			break;
		}
	}
	if ((int)getY() != startY)
		m_zOrderDirty = true;
}

// 0x430850 (folded)
void VehicleDrivingState::update(engine::UpdateContext& context)
{
	m_owner->driveAlongPath(context);
}

// 0x430860
Vehicle::Vehicle(const std::string& name)
	: m_map(0), m_speed(200.0f), m_zOrderDirty(false), m_layer(3), m_target(0)
{
	setName(name);
	maskFlags(4);
}
