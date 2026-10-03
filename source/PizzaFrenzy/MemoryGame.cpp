// MemoryGame: the GameLogic of game mode 1 ("newMemoryGame", shown as "Simon Says Mode").
#include "MemoryGame.h"

#include <algorithm>

#include "engine/Component.h"
#include "engine/SoundMgr.h"
#include "engine/Splat.h"
#include "engine/SplatFactory.h"
#include "engine/StringUtil.h"
#include "BuildingTile.h"
#include "CityMap.h"
#include "Constants.h"
#include "CustomerTile.h"
#include "GameProgress.h"
#include "KitchenTile.h"
#include "Level.h"
#include "Order.h"
#include "PizzaFrenzy.h"
#include "PizzaPopup.h"
#include "SpecialEventTrigger.h"
#include "TileManager.h"

// 0x4605D0
MemoryGame::~MemoryGame()
{
}

// 0x460610
MemoryGame::MemoryGame()
{
}

// 0x460690
void MemoryGame::onAllOrdersDone()
{
	m_player->m_toppingCombo = 0;
	GameLogic::onAllOrdersDone();
}

// 0x4606A0
void MemoryGame::startPlaying()
{
	m_action.setState(new GamePlayAction(this));
}

// 0x460710
void MemoryGame::updateOrders(engine::UpdateContext& ctx)
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
			spawnWave();
		m_orderTimer -= ctx.elapsed;
		while (!m_pendingOrders.empty() && m_orderTimer <= 0.0f)
		{
			Order* order = m_pendingOrders.back();
			if (!placeOrder(order))
				break;
			m_pendingOrders.pop_back();
			m_orderTimer = (std::max)(m_orderGap.random(), g_memoryFlipTime);
			m_waveTimer = m_level->m_waveGap + order->m_patience;
		}
		if (m_ordersSpawned >= m_level->m_numOrders && m_pendingOrders.empty())
			m_allOrdersSpawned = true;
	}
}

// 0x4609F0
void MemoryGame::dispatchPolice(BuildingTile* policeStation, CustomerTile* criminal)
{
	Order* order = criminal->getOrder();
	if (!m_activeOrders.empty() && order == m_activeOrders.front())
	{
		m_activeOrders.pop_front();
		m_player->m_toppingCombo++;
		order->m_deliveryTime = m_deliveryTimer;
		m_deliveryTimer = 0.0f;
		PizzaFrenzy::getSounds()->playSound("bonus_toppingCombo", 1.0f, (m_player->m_toppingCombo - 1) * 0.05f + 1.0f);
		GameLogic::dispatchPolice(policeStation, criminal);
	}
	else
	{
		PizzaFrenzy::getSplatFactory()->createSplat("wrongCustomer", policeStation->getPosition().x,
			policeStation->getPosition().y, NULL, NULL);
		PizzaFrenzy::getGameLogic()->handleEvent("customerHangup");
		criminal->setOrderWaiting(false);
		hangUpAll(true);
	}
}

// 0x460C40
void MemoryGame::deliverPizza(KitchenTile* kitchen, CustomerTile* customer, bool sendVehicle)
{
	Order* order = customer->getOrder();
	if (order->m_character->m_special == 2)
	{
		m_player->m_toppingCombo++;
		order->m_deliveryTime = m_deliveryTimer;
		m_deliveryTimer = 0.0f;
		PizzaFrenzy::getSounds()->playSound("bonus_toppingCombo", 1.0f, (m_player->m_toppingCombo - 1) * 0.05f + 1.0f);
	}
	else if (!m_activeOrders.empty() && order == m_activeOrders.front())
	{
		m_activeOrders.pop_front();
		m_player->m_toppingCombo++;
		order->m_deliveryTime = m_deliveryTimer;
		m_deliveryTimer = 0.0f;
		PizzaFrenzy::getSounds()->playSound("bonus_toppingCombo", 1.0f, (m_player->m_toppingCombo - 1) * 0.05f + 1.0f);
	}
	else
	{
		PizzaFrenzy::getSplatFactory()->createSplat("wrongCustomer", kitchen->getPosition().x,
			kitchen->getPosition().y, NULL, NULL);
		customer->setOrderWaiting(false);
		hangUpAll(true);
		return;
	}
	if (order->m_event && !order->m_event->m_deliver)
	{
		customer->removePopup();
		updatePoliceStation();
	}
	else
	{
		m_player->m_ordersCompleted++;
		std::string text;
		engine::Point pos = kitchen->getPosition();
		int price = order->m_topping->m_isPizza ? g_premiumToppingPrice : g_toppingPrice;
		engine::format(text, "$%d x%d", price, order->m_numPizzas);
		addMoney(price * order->m_numPizzas);
		if (++m_deliveryStreak == 10)
		{
			m_deliveryStreak = 0;
			adjustSatisfaction(-g_satisfactionPenalty);
		}
		addPizzasDelivered(order->m_numPizzas);
		if (sendVehicle)
		{
			engine::Splat* splat = PizzaFrenzy::getSplatFactory()->createSplat("toppingBonus", (float)pos.x,
				(float)pos.y, NULL, NULL);
			splat->setText(text);
			kitchen->dispatchVehicle(customer);
		}
		else
		{
			m_speedBonusTime = 0.0f;
		}
	}
	onOrderFinished();
}

// 0x461030
void MemoryGame::onTileClicked(BuildingTile* building)
{
	if (building->isPopupActive())
	{
		PizzaPopup* popup = building->getPopup();
		if (!m_heldCoupon && popup->getPopupType() != 0 && popup->getPopupType() != 2 && !m_selectedOrders.empty()
			&& m_selectedOrders.front()->getOrder()->m_topping != static_cast<KitchenTile*>(building)->getTopping())
		{
			PizzaFrenzy::getSplatFactory()->createSplat("wrongPizza", building->getPosition().x,
				building->getPosition().y, NULL, NULL);
			for (std::vector<engine::RefPtr<CustomerTile> >::iterator it = m_selectedOrders.begin();
				it != m_selectedOrders.end(); ++it)
			{
				(*it)->setOrderWaiting(false);
				(*it)->popupReleaseIfHovered();
			}
			clearSelection();
			hangUpAll(true);
		}
		else
		{
			GameLogic::onTileClicked(building);
		}
	}
}

// 0x4611A0
void MemoryGame::startSpecialEvent(KitchenTile* kitchen, CustomerTile* customer)
{
	Order* order = customer->getOrder();
	if (!m_activeOrders.empty() && order != m_activeOrders.front())
	{
		PizzaFrenzy::getSplatFactory()->createSplat("wrongCustomer", kitchen->getPosition().x,
			kitchen->getPosition().y, NULL, NULL);
		customer->setOrderWaiting(false);
		hangUpAll(true);
	}
	else
	{
		GameLogic::startSpecialEvent(kitchen, customer);
	}
}

// 0x4612C0
void MemoryGame::spawnWave()
{
	if (m_ordersSpawned >= m_level->m_numOrders)
	{
		m_allOrdersSpawned = true;
		return;
	}
	m_waveTimer = m_level->m_waveGap;
	m_deliveryTimer = 0.0f;
	m_player->m_wavesSpawned++;
	m_patienceRange = (*m_wave)->waitTime;
	m_orderGap = (*m_wave)->spawnDelay;
	int count = (*m_wave)->waveSize.random();
	if (m_ordersSpawned + count >= m_level->m_numOrders)
		count = m_level->m_numOrders - m_ordersSpawned;
	m_ordersSpawned += count;
	if (m_ordersSpawned + 1 == m_level->m_numOrders)
	{
		count++;
		m_ordersSpawned++;
	}
	m_speedBonusTime = count * g_waveTimeScale;
	for (std::vector<engine::RefPtr<SpecialEventTrigger> >::iterator it = m_specialEvents.begin();
		it != m_specialEvents.end(); ++it)
	{
		Order* order = (*it)->onWave();
		if (order && count > 0)
		{
			m_pendingOrders.push_back(order);
			addOrderInPlay();
			count--;
		}
	}
	for (int i = 0; i < count; i++)
	{
		Order* order = new Order();
		order->reset();
		m_pendingOrders.push_back(order);
		addOrderInPlay();
	}
	shuffleOrders(m_pendingOrders);
}

// 0x461560
bool MemoryGame::placeOrder(Order* order)
{
	if (!prepareOrder(order))
		return false;
	m_activeOrders.push_back(order);
	if (order->m_event && !order->m_event->m_deliver)
	{
		CustomerTile* station = m_map->getRandomCustomerOfType(2);
		if (station)
			station->showSpecialPopup(2);
	}
	else
	{
		m_player->m_pizzasOrdered += order->m_numPizzas;
	}
	order->getCustomer()->placeOrder(order, false);
	return true;
}
