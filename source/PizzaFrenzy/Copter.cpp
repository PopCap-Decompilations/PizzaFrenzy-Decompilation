// Copter: the delivery helicopter of the VehicleType class "copter", and its states (lift, parked, flying, arrived).
#include <cmath>
#include <string>
#include <vector>

#include "engine/Image.h"
#include "engine/ParticleSystem.h"
#include "engine/Selector.h"
#include "engine/SoundMgr.h"
#include "engine/Surface.h"
#include "BuildingTile.h"
#include "CityMap.h"
#include "Constants.h"
#include "Copter.h"
#include "CustomerTile.h"
#include "GameScreen.h"
#include "PathFinder.h"
#include "PizzaFrenzy.h"
#include "TileManager.h"

// 0x431CD0
Copter::~Copter()
{
}

// 0x431D00
void CopterLiftState::update(engine::UpdateContext& context)
{
	m_time -= context.elapsed;
	if (m_time < 0.0f)
	{
		m_owner->m_states.setState(m_next);
	}
	else
	{
		float t = m_time / g_copterLiftTime;
		if (m_ascending)
			t = 1.0f - t;
		m_owner->m_body->setPosition(0.0f, -g_copterLiftHeight * t);
	}
}

// 0x431D80
void CopterFlyingState::enter()
{
	m_owner->setFxActive(true);
}

// 0x431DC0
Copter::Copter(const std::string& name)
	: Vehicle(name)
{
}

// 0x431E40 (folded)
void Copter::faceTowards(const engine::Point& tile)
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

// 0x431F20
void CopterParkedState::enter()
{
	m_owner->m_path = 0;
	m_owner->setFxActive(false);
	m_owner->setTile(m_owner->m_homeTile);
	if (m_owner->m_selector->getCount() > 4)
		m_owner->m_selector->select(4, 0.0f, true);
	m_owner->m_zOrderDirty = true;
}

// 0x431FB0
CopterLiftState::CopterLiftState(Copter* copter, bool ascending, engine::StateBase* next, bool returning)
	: engine::State<Copter>(copter), m_ascending(ascending), m_returning(returning), m_next(next)
{
}

// 0x432060
void CopterLiftState::enter()
{
	m_owner->setFxActive(true);
	m_time = g_copterLiftTime;
	m_owner->m_selector->select(3, 0.0f, true);
	if (!m_ascending)
	{
		PizzaFrenzy::getGameScreen()->changeLayer(m_owner, 4, 3);
		m_owner->m_layer = 3;
		m_owner->m_map->addVehicle(m_owner);
	}
}

// 0x4320D0
CopterParkedState::CopterParkedState(Copter* copter)
	: engine::State<Copter>(copter)
{
}

// 0x432160
CopterFlyingState::CopterFlyingState(Copter* copter)
	: engine::State<Copter>(copter)
{
}

// 0x4321F0
CopterArrivedState::CopterArrivedState(Copter* copter)
	: engine::State<Copter>(copter)
{
}

// 0x42FE60 (folded)
void CopterArrivedState::enter()
{
	m_time = 1.0f;
	if (m_owner->m_selector->getCount() > 3)
		m_owner->m_selector->select(3, 0.0f, true);
	m_owner->onArrived();
}

// 0x432280
void CopterArrivedState::update(engine::UpdateContext& context)
{
	m_time -= context.elapsed;
	if (m_time <= 0.0f)
	{
		m_owner->completeDelivery();
		m_owner->m_states.setState(new CopterParkedState(m_owner));
	}
}

// 0x430850 (folded)
void CopterFlyingState::update(engine::UpdateContext& context)
{
	m_owner->driveAlongPath(context);
}

// 0x432320
void CopterFlyingState::onAction(const std::string& action)
{
	if (action == "arrived")
		m_owner->m_states.setState(new CopterLiftState(m_owner, false, new CopterArrivedState(m_owner), false));
}

// 0x4323F0
void Copter::dispatchTo(CustomerTile* customer)
{
	m_target = customer;
	engine::Point dest = customer->getParkingCell();
	if (isIdle() && !(m_tile == dest))
	{
		PizzaFrenzy::getSounds()->playSound("vehicle_leave_copter", 1.0f, 1.0f);
		setRoute(dest);
		if (m_path)
			m_states.setState(new CopterLiftState(this, true, new CopterFlyingState(this), false));
	}
}

// 0x432550
void CopterLiftState::exit()
{
	if (m_ascending)
	{
		m_owner->faceTowards(*m_owner->m_nextWaypoint);
		PizzaFrenzy::getGameScreen()->changeLayer(m_owner, 3, 4);
		m_owner->m_layer = 4;
		m_owner->m_map->addVehicle(m_owner);
	}
	if (m_returning)
	{
		m_owner->m_path = 0;
		m_owner->m_owner->removeVehicle(m_owner);
		PizzaFrenzy::getSounds()->playSound("copterReturn", 1.0f, 1.0f);
	}
}

// 0x432670
bool Copter::init()
{
	VehicleType* def = PizzaFrenzy::getTileManifest()->getVehicleType(getName());
	if (def)
	{
		setDefinition(def);
		m_selector->addRef();
		removeChild(m_selector);
		m_body = new engine::Container();
		addChild(m_body);
		m_body->addChild(m_selector);
		m_selector->release();
		if (m_fx)
		{
			m_fx->addRef();
			removeChild(m_fx);
			m_body->addChild(m_fx);
			m_fx->release();
		}
		if (def->m_hoverImage)
		{
			def->m_hoverImage->setPivotType(1);
			m_selector->addChild(new engine::Image(def->m_hoverImage));
		}
		if (def->m_parkedImage)
		{
			def->m_parkedImage->setPivotType(1);
			m_selector->addChild(new engine::Image(def->m_parkedImage));
		}
		m_layer = 3;
		setBlendMode(1);
		setSpeed((float)def->m_speed);
		m_states.setState(new CopterParkedState(this));
		return true;
	}
	return false;
}

// 0x4328D0
Path* Copter::findPath(const engine::Point& destTile)
{
	Path* path = new Path();
	path->m_points.push_back(m_tile);
	path->m_points.push_back(destTile);
	return path;
}
