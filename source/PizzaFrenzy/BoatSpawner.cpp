// BoatSpawner: launches a Boat along one river route of the level at random intervals.
#include "engine/Component.h"
#include "engine/Range.h"
#include "Boat.h"
#include "BoatSpawner.h"
#include "GameLogic.h"
#include "Level.h"
#include "PizzaFrenzy.h"

// 0x42F4B0
void BoatSpawner::update(engine::UpdateContext& ctx)
{
	m_timer -= ctx.elapsed;
	if (m_timer <= 0.0f)
	{
		spawn();
		m_timer = m_route->delay.random();
	}
}

// 0x42F4F0
void BoatSpawner::spawn()
{
	Boat* boat = new Boat(m_route->name);
	if (boat)
	{
		boat->init();
		boat->setDirection(engine::Vector2(m_route->direction));
		boat->start(m_map, m_route->pos, 0);
		PizzaFrenzy::getGameLogic()->addVehicle(boat);
	}
}

// 0x42F5B0
BoatSpawner::BoatSpawner(CityMap* map, const BoatInfo* route)
	: m_route(route), m_map(map)
{
	m_timer = m_route->delay.random();
}
