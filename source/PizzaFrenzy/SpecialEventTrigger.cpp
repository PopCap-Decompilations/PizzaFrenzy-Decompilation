#include "SpecialEventTrigger.h"

#include "Level.h"
#include "Order.h"
#include "PizzaFrenzy.h"
#include "TileManager.h"

// 0x44B3C0
void SpecialEventTrigger::reset()
{
	m_wavesLeft = m_def->delay.random();
}

// 0x44B3E0
Order* SpecialEventTrigger::createOrder()
{
	SpecialEvent* event = PizzaFrenzy::getTileManifest()->getEvent(m_def->name);
	if (event)
	{
		Order* order = new Order();
		order->reset();
		order->m_event = event;
		Character* character = PizzaFrenzy::getTileManifest()->getThemedCharacter(event->m_character);
		if (character)
		{
			order->m_character = character;
			order->m_numPizzas = character->m_numPizzas;
		}
		return order;
	}
	return 0;
}

// 0x44B490
SpecialEventTrigger::SpecialEventTrigger(const EventInfo* def)
{
	m_def = def;
	m_wavesLeft = def->delay.random();
	if (def->firstWave)
		m_wavesLeft = 0;
}

// 0x44B530
Order* SpecialEventTrigger::onWave()
{
	if (--m_wavesLeft <= 0)
	{
		Order* order = createOrder();
		m_wavesLeft = m_def->delay.random();
		return order;
	}
	return 0;
}
