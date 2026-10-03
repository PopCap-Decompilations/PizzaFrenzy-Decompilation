#include "ConcentrationGameLogic.h"

#include <string>
#include <vector>

#include "engine/Application.h"
#include "engine/Component.h"
#include "engine/SoundHandle.h"
#include "engine/SoundMgr.h"
#include "CityMap.h"
#include "Constants.h"
#include "CustomerTile.h"
#include "GameProgress.h"
#include "Level.h"
#include "MusicPlayer.h"
#include "Order.h"
#include "PizzaFrenzy.h"
#include "PizzaOrderPopup.h"
#include "PizzaPopup.h"
#include "TileManager.h"

// 0x4529D0 (folded)
bool ConcentrationGameLogic::allowsScrambling()
{
	return false;
}

// 0x45B330
ConcentrationGameLogic::~ConcentrationGameLogic()
{
}

// 0x45B360
void ConcentrationGameLogic::handleEvent(const std::string& name)
{
	GameLogic::handleEvent(name);
}

// 0x45B370
void ConcentrationGameLogic::onRightClick(const engine::Point& position)
{
	GameLogic::onRightClick(position);
	if (m_orderTimer > 0.0f)
		m_orderTimer = -1.0f;
}

// 0x45B3E0
ConcentrationGameLogic::ConcentrationGameLogic()
{
}

// 0x45B4F0
void ConcentrationGameLogic::onOrdersChanged()
{
	if (m_waitSound && !m_waitSound->isPlaying())
	{
		m_waitSound->play();
		PizzaFrenzy::getMusicPlayer()->setVolume(m_musicVolume * 0.5f);
	}
	std::vector<engine::RefPtr<CustomerTile> >& customers = m_map->m_customers;
	int active = 0;
	for (std::vector<engine::RefPtr<CustomerTile> >::iterator it = customers.begin(); it != customers.end(); ++it)
	{
		if ((*it)->isPopupActive())
			active++;
	}
	m_orderTimer = m_patienceRange.random() * 0.06666667f * active;	// 1/15
}

// 0x45B590
void ConcentrationGameLogic::flipOverAll()
{
	for (std::vector<engine::RefPtr<CustomerTile> >::iterator it = m_map->m_customers.begin(); it != m_map->m_customers.end(); ++it)
	{
		CustomerTile* customer = *it;
		if (customer->getPopup() && customer->getPopup()->getPopupType() == 0)
		{
			PizzaOrderPopup* popup = static_cast<PizzaOrderPopup*>(customer->getPopup());
			if (popup->isActive())
				popup->flipOver();
		}
	}
	PizzaFrenzy::getSounds()->playSound("popup_flipover", 1.0f, 1.0f);
	if (m_waitSound && m_waitSound->isPlaying())
		m_waitSound->stop();
	PizzaFrenzy::getMusicPlayer()->setVolume(m_musicVolume);
	m_deliveryTimer = 0.0f;
	m_speedBonusTime = m_ordersInPlay * g_waveTimeScale;
}

// 0x45B6E0
void ConcentrationGameLogic::updateOrders(engine::UpdateContext& ctx)
{
	if (!m_deliveries.empty())
	{
		m_deliveryDelay -= ctx.elapsed;
		if (m_deliveryDelay <= 0.0f)
		{
			if (m_deliveries.front().first->getOrder()->m_event)
				startSpecialEvent(m_deliveries.front().second, m_deliveries.front().first);
			else
				deliverPizza(m_deliveries.front().second, m_deliveries.front().first, true);
			m_deliveries.pop_front();
			if (!m_deliveries.empty())
				m_deliveryDelay = 0.4f;
		}
	}
	if (!m_allOrdersSpawned)
	{
		m_deliveryTimer += ctx.elapsed;
		m_waveTimer -= ctx.elapsed;
		m_speedBonusTime -= ctx.elapsed;
		if (m_waveTimer < 0.0f && m_pendingOrders.empty() && m_map->allOrdersDone())
		{
			spawnWave();
			m_orderTimer = m_patienceRange.random() * 0.06666667f * m_pendingOrders.size();	// 1/15
		}
		while (!m_pendingOrders.empty())
		{
			Order* order = m_pendingOrders.back();
			if (!placeOrder(order))
				break;
			m_pendingOrders.pop_back();
			m_waveTimer = m_level->m_waveGap + order->m_patience;
			if (m_waitSound && !m_waitSound->isPlaying())
			{
				m_waitSound->play();
				PizzaFrenzy::getMusicPlayer()->setVolume(m_musicVolume * 0.5f);
			}
		}
		if (m_orderTimer != 0.0f)
		{
			m_orderTimer -= ctx.elapsed;
			if (m_orderTimer <= 0.0f && m_pendingOrders.empty())
			{
				m_orderTimer = 0.0f;
				flipOverAll();
			}
		}
	}
}

// 0x45BA10
bool ConcentrationGameLogic::placeOrder(Order* order)
{
	if (!prepareOrder(order))
		return false;
	m_activeOrders.push_back(order);
	if (order->m_event && !order->m_event->m_deliver)
	{
		CustomerTile* police = m_map->getRandomCustomerOfType(2);
		if (police)
			police->showSpecialPopup(2);
	}
	else
	{
		m_player->m_pizzasOrdered += order->m_numPizzas;
	}
	order->getCustomer()->placeOrder(order, true);
	return true;
}

// 0x45BBD0
void ConcentrationGameLogic::init(GameScreen* screen, CityMap* map, HudScreen* hud, GameProgress* player)
{
	GameLogic::init(screen, map, hud, player);
	m_waitSound = PizzaFrenzy::getSounds()->getSound("popup_wait");
	if (m_waitSound)
		m_waitSound->setLooping(true);
	engine::getApplication()->m_rightMouseDownSignal.connect(this, &ConcentrationGameLogic::onRightClick);
}
