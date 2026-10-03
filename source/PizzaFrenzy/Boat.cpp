// Boat: an ambient river boat launched by a BoatSpawner.
#include <string>

#include "engine/Application.h"
#include "engine/ParticleSystem.h"
#include "engine/Selector.h"
#include "Boat.h"
#include "BridgeTile.h"
#include "CityMap.h"
#include "PizzaFrenzy.h"
#include "Tile.h"
#include "TileManager.h"

// 0x431520
Boat::~Boat()
{
}

// 0x431550
void Boat::start(CityMap* map, const engine::Point& pos, BuildingTile* owner)
{
	m_owner = owner;
	m_map = map;
	updateBounds();
	setPosition((float)pos.x, (float)pos.y);
	m_map->addVehicle(this);
	m_zOrderDirty = false;
	m_boostFrames = 0;
}

// 0x4315B0
void Boat::sail(engine::UpdateContext& ctx)
{
	int startY = (int)getY();
	engine::Vector2 pos(getPosition());
	if (m_boostFrames > 0)
	{
		m_boostFrames--;
		pos += m_velocity * ctx.elapsed * 3.0f;
	}
	else
	{
		pos += m_velocity * ctx.elapsed;
	}
	if (canSailTo(m_lookAhead + pos) && canSailTo(pos))
	{
		setPosition(pos);
		if ((int)getY() != startY)
			m_zOrderDirty = true;
		int height = engine::getApplication()->getHeight();
		int width = engine::getApplication()->getWidth();
		if ((pos.x > width && m_velocity.x > 0.0f) || (pos.x < 0.0f && m_velocity.x < 0.0f)
			|| (pos.y > height && m_velocity.y > 0.0f) || (pos.y < 0.0f && m_velocity.y < 0.0f))
		{
			removeFromMap();
		}
	}
}

// 0x431750
void Boat::onMouseDown(const engine::Vector2& position)
{
	m_boostFrames += 10;
}

// 0x431760
void Boat::setDirection(const engine::Vector2& dir)
{
	m_velocity = dir * m_speed;
	faceTowards(engine::Point(0, 0));
	m_lookAhead = m_velocity;
	m_lookAhead.normalize();
	m_lookAhead *= 25.0f;
}

// 0x431810
Boat::Boat(const std::string& name)
	: Vehicle(name)
{
}

// 0x4318D0
void Boat::updateOnMap(engine::UpdateContext& ctx)
{
	sail(ctx);
	engine::Container::update(ctx);
}

// 0x4318F0
bool Boat::init()
{
	VehicleType* def = PizzaFrenzy::getTileManifest()->getVehicleType(getName());
	setFlags(2);
	if (def)
	{
		setDefinition(def);
		if (m_fx)
		{
			removeChild(m_fx);
			addChild(m_fx);
		}
		m_layer = 3;
		setSpeed((float)def->m_speed);
		setBlendMode(1);
		return true;
	}
	return false;
}

// 0x431980
void Boat::faceTowards(const engine::Point& tile)
{
	if (m_velocity.x != 0.0f)
		m_selector->select(0, 0.0f, true);
	else if (m_velocity.y < 0.0f)
		m_selector->select(1, 0.0f, true);
	else
		m_selector->select(2, 0.0f, true);
	float scaleX = 1.0f;
	if (m_velocity.x < 0.0f)
		scaleX = -1.0f;
	setScale(scaleX, 1.0f);
}

// 0x431A10
bool Boat::canSailTo(const engine::Vector2& pos)
{
	engine::Point cell = m_map->screenToCell(pos);
	if (cell.x > -1 && cell.y > -1)
	{
		Tile* tile = m_map->getTile(cell);
		if (tile && tile->getClassName() == "bridge" && !static_cast<BridgeTile*>(tile)->requestBlocked(true))
			return false;
	}
	return true;
}
