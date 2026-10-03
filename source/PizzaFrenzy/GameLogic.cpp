// GameLogic (a day's gameplay controller) and the phase actions it runs: intro, play, prize blimp, end of the day.
#include "GameLogic.h"

#include <algorithm>

#include "engine/Application.h"
#include "engine/FadeContainer.h"
#include "engine/ScreenLayout.h"
#include "engine/SoundHandle.h"
#include "engine/SoundMgr.h"
#include "engine/Splat.h"
#include "engine/SplatFactory.h"
#include "engine/StringUtil.h"
#include "engine/TextItem.h"
#include "AnimatedText.h"
#include "Blimp.h"
#include "CitizenAward.h"
#include "CityMap.h"
#include "Constants.h"
#include "CouponFrame.h"
#include "CustomerTile.h"
#include "GameProgress.h"
#include "GameScreen.h"
#include "HudScreen.h"
#include "KitchenTile.h"
#include "Level.h"
#include "MusicPlayer.h"
#include "MusicTrack.h"
#include "Order.h"
#include "PizzaFrenzy.h"
#include "PizzaOrderPopup.h"
#include "PizzaPopup.h"
#include "PolicePopup.h"
#include "SpecialEventPopup.h"
#include "SpecialEventTrigger.h"
#include "SpeedBonusBanner.h"
#include "TileManager.h"
#include "Tip.h"
#include "Vehicle.h"

// 0x455B10
void GameLogic::onUnknown22(void* a, void* b, void* c)
{
}

// 0x455B20
void GameLogic::startSpecialEvent(KitchenTile* kitchen, CustomerTile* customer)
{
	SpecialEventPopup* popup = m_eventFactory.create(customer->getOrder()->m_event->m_type);
	popup->init(kitchen, customer);
	showEventPopup(popup);
	customer->onVehicleDispatched();
}

// 0x455B70
void GameLogic::notifyPopupRightClicked()
{
	m_popupRightClicked = true;
}

// 0x455B80
void GameLogic::setTimeEffect(float duration, float timeScale)
{
	m_timeScale = timeScale;
	m_timeEffectLeft = duration;
}

// 0x455BA0
void GameLogic::addOrderInPlay()
{
	m_ordersInPlay++;
}

// 0x455BB0
void LevelIntroAction::onSkipClick(const engine::Point& position)
{
	if (m_time > 0.0f && m_phase <= 1)
	{
		m_time = 0.0f;
		m_phase = 1;
	}
}

// 0x455BE0
void GamePlayAction::update(engine::UpdateContext& context)
{
	m_owner->updateOrders(context);
	m_owner->updateMap(context);
	m_owner->checkLevelEnd();
}

// 0x455D60
void GameLogic::placeTip(Order* order, const engine::Vector2& position, Tip* tip)
{
	if (!tip)
		tip = new Tip();
	float deliveryTime = order ? order->m_deliveryTime : 0.0f;
	int stars = 1;
	if (deliveryTime < g_tipThreeStarTime)
		stars = 3;
	else if (deliveryTime < g_tipTwoStarTime)
		stars = 2;
	if (order)
		tip->init((float)stars, order->m_topping);
	else
		tip->init((float)stars, NULL);
	addTip(tip, position);
}

// 0x455E50
int GameLogic::getSelectedCount() const
{
	return (int)m_selectedOrders.size();
}

// 0x455FE0
void GameLogic::updateMap(engine::UpdateContext& ctx)
{
	m_map->update(ctx);
}

// 0x455FF0
void GameLogic::spawnTip(BuildingTile* building, Order* order, float waitTime)
{
	engine::Point position = m_map->cellToScreen(building->getParkingCell());
	position.x += (int)(m_map->getCellSize() * -0.5f);
	position.y += (int)(m_map->getCellSize() * -0.5f);
	if (order->m_event && !order->m_event->m_deliver)
		placeTip(order, engine::Vector2(position), new CitizenAward());
	else
		placeTip(order, engine::Vector2(position), new Tip());
}

// 0x456130
void GameLogic::onRightClick(const engine::Point& position)
{
	if (!m_popupRightClicked)
	{
		if (m_heldCoupon)
		{
			m_heldCoupon->addToHud();
			m_heldCoupon = NULL;
			m_screen->setOrderCursorItem(NULL);
		}
		else
		{
			deselectAll();
		}
	}
	m_popupRightClicked = false;
}

// 0x4561A0
void GameLogic::addTip(Tip* tip, const engine::Vector2& position)
{
	tip->placeOnMap(m_map, position);
	m_screen->addTip(tip);
}

// 0x4561D0
void GameLogic::removeTip(Tip* tip)
{
	tip->setFlags(16);
	if (m_map->allOrdersDone())
		m_waveTimer = (std::min)(m_waveTimer, g_moneyCollectTime);
}

// 0x456220
void GameLogic::removeVehicle(Vehicle* vehicle)
{
	m_screen->removeVehicle(vehicle);
	m_map->removeVehicle(vehicle);
}

// 0x456240
void GameLogic::addVehicle(Vehicle* vehicle)
{
	m_screen->addVehicle(vehicle);
}

// 0x456250
void GameLogic::addMoney(int amount)
{
	m_player->addCash(amount);
}

// 0x456260
void GameLogic::adjustSatisfaction(int delta)
{
	m_player->m_satisfaction += delta;
	int satisfaction = m_player->m_satisfaction;
	if (satisfaction > 100)
		satisfaction = 100;
	else if (satisfaction < 0)
		satisfaction = 0;
	m_player->m_satisfaction = satisfaction;
	if (m_player->m_satisfaction <= g_lowSatisfaction)
		setLowSatisfaction(true);
	else if (m_player->m_satisfaction > g_lowSatisfaction)
		setLowSatisfaction(false);
	m_hud->checkHappinessBonus();
}

// 0x4562E0
void GameLogic::addPizzasDelivered(int count)
{
	m_player->m_pizzasDelivered += count;
}

// 0x456300
void GameLogic::setPaused(bool paused)
{
	deselectAll();
	m_hud->setPaused(paused);
}

// 0x456320
void GameLogic::endLevel()
{
	PizzaFrenzy::getMusicPlayer()->stop();
	m_music = NULL;
	if (m_timeEffectSound && m_timeEffectSound->isPlaying())
		m_timeEffectSound->stop();
	deselectAll();
	clearMap();
	clearEventPopups();
	m_player->m_score += m_player->m_cash;
}

// 0x456390
void GameLogic::resetCombo()
{
	m_comboTopping = NULL;
	m_player->m_toppingCombo = 0;
	m_map->setKitchenSigns(NULL, 0, 0);
}

// 0x4563B0
void GameLogic::onOrderFinished()
{
	m_ordersInPlay--;
	m_player->m_ordersLeft--;
	if (m_ordersInPlay == 0)
		onAllOrdersDone();
}

// 0x4563D0
bool GameLogic::isPoliceOrderPending()
{
	CustomerTile* station = m_map->getRandomCustomerOfType(2);
	if (station)
	{
		PizzaPopup* popup = station->getPopup();
		if (popup && popup->getPopupType() != 2)
			return true;
	}
	return false;
}

// 0x456410
void GameLogic::updatePoliceStation()
{
	CustomerTile* station = m_map->getRandomCustomerOfType(2);
	if (station)
	{
		PizzaPopup* popup = station->getPopup();
		if (popup && popup->getPopupType() == 2 && !hasCriminals())
			station->dismissPopup();
	}
}

// 0x456460
void GameLogic::cheatWinLevel()
{
	m_player->m_pizzasOrdered = 200;
	m_player->m_pizzasDelivered = 200;
	m_player->m_bonus = 1000;
	m_player->addCash(1000);
	deselectAll();
	m_allOrdersSpawned = true;
}

// 0x4564A0
void LevelIntroAction::update(engine::UpdateContext& context)
{
	m_time -= context.elapsed;
	if (m_time < 0.0f)
	{
		switch (m_phase)
		{
		case 0:
			m_phase = 1;
			m_time = 3.0f;
			if (m_owner->m_titles)
				static_cast<engine::FadeContainer*>(m_owner->m_titles.get())->fade(true, 0.5f, 1.0f, false);
			break;
		case 1:
			m_phase = 2;
			m_time = 1.0f;
			if (m_owner->m_titles)
				static_cast<engine::FadeContainer*>(m_owner->m_titles.get())->fade(false, 0.5f, 1.0f, false);
			break;
		case 2:
			m_owner->startPlaying();
			break;
		}
	}
}

// 0x456540
void LevelIntroAction::exit()
{
	m_owner->m_titles->setVisible(false);
}

// 0x4565D0
GamePlayAction::GamePlayAction(GameLogic* game)
	: engine::State<GameLogic>(game)
{
}

// 0x456660
LevelEndAction::LevelEndAction(GameLogic* game, bool gameOver)
	: engine::State<GameLogic>(game), m_gameOver(gameOver)
{
}

// 0x456730
PrizeBlimpAction::PrizeBlimpAction(GameLogic* game)
	: engine::State<GameLogic>(game)
{
}

// 0x4569C0
void GameLogic::update(engine::UpdateContext& ctx)
{
	if (m_eventPopups.size() != 0)
		m_eventPopups[0]->updateActions(ctx);
	m_hud->tick(ctx);
	m_player->m_playTime += ctx.elapsed;
	if (m_timeEffectLeft > 0.0f)
	{
		m_timeEffectLeft -= ctx.elapsed;
		if (m_timeEffectSound && !m_timeEffectSound->isPlaying())
		{
			m_timeEffectSound->play();
			PizzaFrenzy::getMusicPlayer()->setVolume(0.0f);
		}
		if (m_timeEffectLeft <= 0.0f)
		{
			m_timeEffectLeft = 0.0f;
			m_timeScale = 1.0f;
			if (m_timeEffectSound)
				m_timeEffectSound->stop();
			PizzaFrenzy::getMusicPlayer()->setVolume(m_musicVolume);
		}
		else
		{
			ctx.elapsed *= m_timeScale;
		}
	}
	m_screen->tick(ctx);
	m_action.update(ctx);
}

// 0x456AC0
void GameLogic::checkLevelEnd()
{
	if (m_player->m_satisfaction <= 0)
	{
		m_action.setState(new LevelEndAction(this, true));
	}
	else if (m_allOrdersSpawned && m_map->allOrdersDone() && m_map->m_tips.size() == 0)
	{
		if (m_player->m_pizzasDelivered >= m_player->m_pizzasOrdered)
			m_action.setState(new PrizeBlimpAction(this));
		else
			m_action.setState(new LevelEndAction(this, false));
	}
}

// 0x456BD0
void GameLogic::collectMoney(int amount)
{
	m_player->addCash(amount);
	if (m_map->allOrdersDone())
		m_waveTimer = (std::min)(m_waveTimer, g_moneyCollectTime);
}

// 0x456C20
void GameLogic::startPlaying()
{
	m_hud->addCityCoupons();
	m_action.setState(new GamePlayAction(this));
}

// 0x456C90
void GameLogic::clearMap()
{
	std::vector<engine::RefPtr<Tip> >& tips = m_map->m_tips;
	for (std::vector<engine::RefPtr<Tip> >::iterator it = tips.begin(); it != tips.end(); ++it)
		removeTip(*it);
	std::vector<engine::RefPtr<CustomerTile> >& customers = m_map->m_customers;
	for (std::vector<engine::RefPtr<CustomerTile> >::iterator it = customers.begin(); it != customers.end(); ++it)
		(*it)->removePopup();
	std::vector<engine::RefPtr<KitchenTile> >& kitchens = m_map->m_kitchens;
	for (std::vector<engine::RefPtr<KitchenTile> >::iterator it = kitchens.begin(); it != kitchens.end(); ++it)
		(*it)->removePopup();
}

// 0x456D10
void GameLogic::dispatchPolice(BuildingTile* policeStation, CustomerTile* criminal)
{
	// the police station is a customer tile (a customer of type 2)
	CustomerTile* station = static_cast<CustomerTile*>(policeStation);
	static_cast<PolicePopup*>(policeStation->getPopup())->sendPoliceCar(criminal);
	m_player->m_ordersCompleted++;
	onOrderFinished();
	if (!hasCriminals())
	{
		Order* order = new Order();
		order->m_customer = station;
		order->m_character = station->getCharacter();
		order->m_topping = criminal->getOrder()->m_topping;
		order->m_numPizzas = order->m_character->m_numPizzas;
		m_player->m_pizzasOrdered += order->m_numPizzas;
		m_player->m_ordersLeft++;
		m_player->m_ordersCompleted--;
		m_ordersInPlay++;
		order->m_patience = criminal->getOrder()->m_patience;
		station->placeOrder(order, false);
	}
	else
	{
		station->showSpecialPopup(2);
	}
}

// 0x456E50
bool GameLogic::hasCriminals()
{
	bool found = false;
	for (std::vector<engine::RefPtr<CustomerTile> >::iterator it = m_map->m_customers.begin();
		it != m_map->m_customers.end(); ++it)
	{
		CustomerTile* customer = *it;
		if (customer->isPopupActive())
		{
			PizzaPopup* popup = customer->getPopup();
			if (popup->getPopupType() == 0)
			{
				SpecialEvent* event = static_cast<PizzaOrderPopup*>(popup)->getOrder()->m_event;
				if (event && !event->m_deliver)
				{
					found = true;
					break;
				}
			}
		}
	}
	for (std::vector<engine::RefPtr<CustomerTile> >::iterator it = m_selectedOrders.begin();
		it != m_selectedOrders.end(); ++it)
	{
		SpecialEvent* event = (*it)->getOrder()->m_event;
		if (event && !event->m_deliver)
		{
			found = true;
			break;
		}
	}
	for (std::list<engine::RefPtr<Order> >::iterator it = m_pendingOrders.begin(); it != m_pendingOrders.end(); ++it)
	{
		SpecialEvent* event = (*it)->m_event;
		if (event && !event->m_deliver)
		{
			found = true;
			break;
		}
	}
	return found;
}

// 0x456F40
void PrizeBlimpAction::update(engine::UpdateContext& context)
{
	m_owner->updateMap(context);
	m_time -= context.elapsed;
	if (!m_blimp || (m_blimp->isFinished() && m_time <= 0.0f && m_owner->m_map->allOrdersDone()))
		m_owner->m_action.setState(new LevelEndAction(m_owner, false));
}

// 0x457100
bool GameLogic::prepareOrder(Order* order)
{
	CustomerTile* customer = order->getCustomer();
	if (!customer || !customer->isAvailable())
	{
		customer = m_map->getRandomCustomer();
		if (!customer)
			return false;
		order->m_customer = customer;
	}
	if (!order->m_character)
		order->m_character = customer->getCharacter();
	if (!order->m_numPizzas)
		order->m_numPizzas = order->m_character->m_numPizzas;
	if (order->m_patience == 0.0f)
	{
		order->m_patience = m_patienceRange.random();
		if (getGame()->m_mode)
			order->m_patience *= g_orderPrepareTime;
		if (order->m_event && order->m_event->m_type == "MovieStar")
			order->m_patience *= g_orderPrepareDelay;
	}
	if (!order->m_topping)
		order->m_topping = m_map->getRandomKitchenPizza();
	return true;
}

// 0x4571E0
void GameLogic::updateOrderCursor()
{
	m_screen->clearOrderCursor();
	if (m_selectedOrders.size() != 0)
		static_cast<PizzaOrderPopup*>(m_selectedOrders.back()->getPopup())->buildOrderCursor(m_screen->m_orderCursor);
}

// 0x457290
void GameLogic::resetLevel()
{
	m_player->startLevel(m_level);
	m_orderTimer = 0.0f;
	m_waveTimer = 1.0f;
	m_deliveryDelay = 0.0f;
	while (!m_deliveries.empty())
		m_deliveries.pop_front();
	m_pendingOrders.clear();
	m_allOrdersSpawned = false;
	m_heldCoupon = NULL;
	m_comboTopping = NULL;
	for (std::vector<engine::RefPtr<SpecialEventTrigger> >::iterator it = m_specialEvents.begin();
		it != m_specialEvents.end(); ++it)
		(*it)->reset();
	m_wave = m_level->getWaves().begin();
	m_activeOrders.clear();
	m_ordersInPlay = 0;
	m_deliveryStreak = 0;
	m_ordersSpawned = 0;
	m_deliveryTimer = 0.0f;
	m_hud->reset();
	resetCombo();
	m_popupRightClicked = false;
	m_timeEffectLeft = 0.0f;
	m_timeScale = 1.0f;
}

// 0x457490
void GameLogic::updateOrders(engine::UpdateContext& ctx)
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
		if (m_waveTimer < 0.0f && m_pendingOrders.size() == 0 && !isPoliceOrderPending())
			spawnWave();
		m_orderTimer -= ctx.elapsed;
		while (m_pendingOrders.size() != 0 && m_orderTimer <= 0.0f)
		{
			Order* order = m_pendingOrders.back();
			if (!placeOrder(order))
				break;
			m_pendingOrders.pop_back();
			m_orderTimer = m_orderGap.random();
			m_waveTimer = m_level->m_waveGap + order->m_patience;
		}
		if (m_ordersSpawned >= m_level->m_numOrders && m_pendingOrders.size() == 0)
			m_allOrdersSpawned = true;
	}
}

// 0x457760
void GameLogic::handleEvent(const std::string& name)
{
	if (name == "customerHangup")
	{
		m_activeOrders.pop_front();
		adjustSatisfaction(g_satisfactionPenalty);
		m_deliveryStreak = 0;
		m_speedBonusTime = 0.0f;
		onOrderFinished();
	}
	else if (name == "falseArrest")
	{
		adjustSatisfaction(g_satisfactionPenalty);
		m_deliveryStreak = 0;
	}
	else if (name == "specialHangup")
	{
		m_activeOrders.pop_front();
		onOrderFinished();
		updatePoliceStation();
	}
}

// 0x457900
void GameLogic::hangUpAll(bool silent)
{
	m_player->m_toppingCombo = 0;
	m_activeOrders.clear();
	m_player->m_ordersLeft -= m_pendingOrders.size();
	m_pendingOrders.clear();
	m_speedBonusTime = 0.0f;
	std::vector<engine::RefPtr<CustomerTile> >& customers = m_map->m_customers;
	for (std::vector<engine::RefPtr<CustomerTile> >::iterator it = customers.begin(); it != customers.end(); ++it)
	{
		if ((*it)->getActiveOrder())
		{
			(*it)->dismissPopup();
			m_player->m_ordersLeft--;
		}
	}
	if (!silent)
	{
		PizzaFrenzy::getSounds()->playSound("popup_hangup", 1.0f, 1.0f);
		m_player->m_ordersLeft++;
		m_ordersInPlay++;
		handleEvent("customerHangup");
	}
	m_ordersInPlay = 0;
	updatePoliceStation();
	onAllOrdersDone();
}

// 0x457A90
void GameLogic::removeEventPopup(SpecialEventPopup* popup)
{
	m_eventPopups.erase(std::remove(m_eventPopups.begin(), m_eventPopups.end(), popup));
	popup->setFlags(16);
}

// 0x457B00
void GameLogic::clearEventPopups()
{
	for (std::vector<engine::RefPtr<SpecialEventPopup> >::iterator it = m_eventPopups.begin();
		it != m_eventPopups.end(); ++it)
		(*it)->setFlags(16);
	m_eventPopups.clear();
}

// 0x457B80
void GameLogic::setLowSatisfaction(bool low)
{
	if (low)
	{
		m_hud->showStarWarning(true);
		PizzaFrenzy::getMusicPlayer()->play(PizzaFrenzy::getTileManifest()->getMusicClip("music_lowSatWarning"));
	}
	else
	{
		m_hud->showStarWarning(false);
		PizzaFrenzy::getMusicPlayer()->play(m_music);
	}
}

// 0x457C50
void GameLogic::useCoupon(CouponFrame* coupon)
{
	int cost = getSelectedCount() * coupon->getPrice();
	if (!m_selectedOrders.empty())
	{
		if (cost <= m_player->m_cash)
		{
			PizzaFrenzy::getSounds()->playSound("tip_couponConvert", 1.0f, 1.0f);
			m_player->addCash(-cost);
			PizzaOrderPopup* popup;
			for (std::vector<engine::RefPtr<CustomerTile> >::iterator it = m_selectedOrders.begin();
				it != m_selectedOrders.end(); ++it)
			{
				popup = static_cast<PizzaOrderPopup*>((*it)->getPopup());
				Order* order = popup->getOrder();
				order->m_topping = coupon->getTopping();
				popup->replaceOrder(order, false);
			}
			m_screen->clearOrderCursor();
			coupon->setPrice(coupon->getPrice() + 100);
			coupon->addToHud();
			popup->buildOrderCursor(m_screen->m_orderCursor);
		}
	}
	else
	{
		if (m_heldCoupon)
		{
			m_heldCoupon->addToHud();
			m_heldCoupon = NULL;
			m_screen->setOrderCursorItem(NULL);
		}
		m_heldCoupon = coupon;
		m_screen->setOrderCursorItem(coupon);
	}
}

// 0x457E80
void GameLogic::applyHeldCoupon(PizzaOrderPopup* popup)
{
	Order* order = popup->getOrder();
	order->m_topping = m_heldCoupon->getTopping();
	popup->replaceOrder(order, true);
	PizzaFrenzy::getSounds()->playSound("tip_couponConvert", 1.0f, 1.0f);
	m_player->addCash(-m_heldCoupon->getPrice());
	m_heldCoupon->setPrice(m_heldCoupon->getPrice() + 100);
	m_heldCoupon->addToHud();
	m_heldCoupon = NULL;
	m_screen->setOrderCursorItem(NULL);
}

// 0x457FC0
std::string GameLogic::getFrenzyText(int combo)
{
	int level = combo / g_frenzyComboSize;
	if (level < 1)
		return "";
	unsigned int id;
	switch (level)
	{
	case 1:
		id = 211;
		break;
	case 2:
		id = 212;
		break;
	case 3:
		id = 213;
		break;
	case 4:
		id = 214;
		break;
	case 5:
		id = 215;
		break;
	default:
		id = 216;
		break;
	}
	return engine::getApplication()->loadString(id);
}

// 0x458060
void GameLogic::clearSelection()
{
	m_selectedOrders.clear();
	if (m_screen)
	{
		if (m_heldCoupon)
		{
			m_heldCoupon->addToHud();
			m_heldCoupon = NULL;
			m_screen->setOrderCursorItem(NULL);
		}
		m_screen->clearOrderCursor();
	}
}

// 0x4580F0
void GameLogic::setupTitles()
{
	City* city = getGame()->getCurrentCity();
	engine::ScreenLayout* layout = m_screen->m_layout;
	engine::Container* extra = static_cast<engine::Container*>(layout->getComponent("extra"));
	extra->removeAllChildren();
	engine::TextItem* cityText = static_cast<engine::TextItem*>(layout->getComponent("text1"));
	std::string text;
	if (cityText)
	{
		text = engine::getApplication()->formatString(225, city->getName().c_str());
		cityText->setText(text);
	}
	engine::TextItem* levelText = static_cast<engine::TextItem*>(layout->getComponent("text2"));
	if (levelText)
	{
		text = engine::getApplication()->formatString(226, m_level->getName().c_str());
		levelText->setText(text);
	}
	engine::TextItem* dayText = static_cast<engine::TextItem*>(layout->getComponent("text3"));
	if (dayText)
	{
		text = engine::getApplication()->formatString(224, getGame()->m_levelNumber);
		dayText->setText(text);
	}
}

// 0x458490
void LevelEndAction::enter()
{
	if (m_owner->m_hud)
		m_owner->m_hud->stopTipJarRollup();
	engine::ParticleSystemDef* effect = PizzaFrenzy::getTileManifest()->getFx("BurstFx")->m_effect;
	m_title = new AnimatedText();
	std::string text;
	if (m_gameOver)
		text = engine::getApplication()->loadString(210);
	else if (m_owner->m_player->m_pizzasDelivered == m_owner->m_player->m_pizzasOrdered)
		text = engine::getApplication()->loadString(209);
	else
		text = engine::getApplication()->loadString(208);
	m_title->setText(text, 0.1f, 4.0f, engine::getApplication()->getFont("res\\fonts\\titleFont.xml"), effect, 0.0f,
		0.05f);
	m_owner->m_screen->addChild(m_title);
	m_title->show(engine::Vector2(400.0f, 200.0f));
	m_leaving = false;
	m_time = 2.5f;
	m_owner->endLevel();
	m_owner->m_screen->deactivate();
	if (m_gameOver)
		PizzaFrenzy::getMusicPlayer()->play(PizzaFrenzy::getTileManifest()->getMusicClip("music_gameOver"));
	else
		PizzaFrenzy::getMusicPlayer()->play(PizzaFrenzy::getTileManifest()->getMusicClip("music_levelComplete"));
}

// 0x4587A0
void LevelEndAction::update(engine::UpdateContext& context)
{
	if (!m_leaving)
	{
		m_time -= context.elapsed;
		if (m_time < 0.0f)
		{
			m_title->hide();
			m_leaving = true;
			m_time = 2.0f;
		}
	}
	else
	{
		m_time -= context.elapsed;
		if (m_time < 0.0f)
		{
			if (m_gameOver)
				getGame()->onScreenEvent("gameOver");
			else
				getGame()->onScreenEvent("levelEnd");
		}
	}
}

// 0x4588B0
void PrizeBlimpAction::enter()
{
	m_blimp = new Blimp("PrizeBlimp");
	if (m_blimp)
	{
		m_blimp->init();
		m_blimp->setDirection(engine::Vector2(-1.0f, 0.0f));
		m_blimp->start(m_owner->m_map, engine::Point(830, 200), NULL);
		m_owner->addVehicle(m_blimp);
		PizzaFrenzy::getMusicPlayer()->play(PizzaFrenzy::getTileManifest()->getMusicClip("music_blimpFlyby"));
		m_time = 7.5f;
	}
	std::vector<engine::RefPtr<KitchenTile> >& kitchens = m_owner->m_map->m_kitchens;
	for (std::vector<engine::RefPtr<KitchenTile> >::iterator it = kitchens.begin(); it != kitchens.end(); ++it)
		(*it)->dismissPopup();
	m_owner->m_hud->slideOutCoupons();
}

// 0x458C40
void GameLogic::deselectAll()
{
	for (std::vector<engine::RefPtr<CustomerTile> >::iterator it = m_selectedOrders.begin();
		it != m_selectedOrders.end(); ++it)
		(*it)->popupReleaseIfHovered();
	clearSelection();
}

// 0x459150
void GameLogic::unloadLevel()
{
	m_specialEvents.clear();
	m_map->clearBoatSpawners();
	m_level = NULL;
}

// 0x459220
LevelIntroAction::LevelIntroAction(GameLogic* game)
	: engine::State<GameLogic>(game)
{
}

// 0x459510
GameLogic::GameLogic()
{
}

// 0x4596D0
GameLogic::~GameLogic()
{
	m_action.setState(NULL);
}

// 0x459950
void GameLogic::init(GameScreen* screen, CityMap* map, HudScreen* hud, GameProgress* player)
{
	m_screen = screen;
	m_map = map;
	m_hud = hud;
	m_player = player;
	m_titles = m_screen->m_layout->getComponent("titles");
	m_timeEffectSound = PizzaFrenzy::getSounds()->getSound("special_timeEffect");
	if (m_timeEffectSound)
		m_timeEffectSound->setLooping(true);
	m_music = NULL;
	engine::getApplication()->m_rightMouseDownSignal.connect(this, &GameLogic::onRightClick);
}

// 0x459B90
bool GameLogic::placeOrder(Order* order)
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

// 0x459CB0
void GameLogic::startLevel(Level* level)
{
	m_level = level;
	resetLevel();
	level->setupKitchens(m_map);
	m_map->updateKitchenPizzas();
	level->createBoatSpawners(m_map);
	m_screen->buildMap();
	m_music = PizzaFrenzy::getTileManifest()->getMusicClip(level->getMusic());
	if (m_music)
	{
		m_musicVolume = m_music->getVolume();
		PizzaFrenzy::getMusicPlayer()->play(m_music);
	}
	level->createEvents(m_specialEvents);
	resetCombo();
	m_screen->m_speedBonus->hide();
	setupTitles();
	m_action.setState(new LevelIntroAction(this));
}

// 0x459E20
void GameLogic::shuffleOrders(std::list<engine::RefPtr<Order> >& orders)
{
	std::list<engine::RefPtr<Order> > shuffled;
	int serial = orders.size() - 1;
	while (orders.size() != 0)
	{
		int index = engine::randomInt(0, orders.size() - 1);
		std::list<engine::RefPtr<Order> >::iterator it = orders.begin();
		for (int i = 0; i < index; i++)
			++it;
		Order* order = *it;
		order->m_serial = serial--;
		shuffled.push_back(order);
		orders.erase(it);
	}
	orders.insert(orders.begin(), shuffled.begin(), shuffled.end());
}

// 0x459FC0
void GameLogic::onAllOrdersDone()
{
	m_deliveryTimer = 0.0f;
	if (m_map->allOrdersDone())
		m_waveTimer = (std::min)(m_waveTimer, g_moneyCollectTime);
	else if (m_level->m_waveGap < m_waveTimer)
		m_waveTimer = (std::max)(m_level->m_waveGap, g_moneyCollectTime);
	if (m_speedBonusTime > 0.0f)
	{
		engine::Point mouse = engine::getApplication()->getMousePosition();
		PizzaFrenzy::getSounds()->playSound("bonus_speedyBlast", 1.0f, 1.0f);
		engine::Splat* splat = PizzaFrenzy::getSplatFactory()->createSplat("speedyDelivery", (float)mouse.x,
			(float)mouse.y, NULL, NULL);
		engine::Vector2 target = m_hud->getStarLevelPosition();
		splat->moveTo(target.x, target.y);
		splat->m_onFinished.connect(this, &GameLogic::adjustSatisfaction);
		splat->m_eventArg = -g_satisfactionPenalty;
	}
}

// 0x45A1A0
void LevelIntroAction::enter()
{
	m_owner->m_titles->setVisible(false);
	m_owner->m_titles->setAlpha(0.0f);
	m_time = g_levelIntroFadeTime;
	m_phase = 0;
	engine::getApplication()->m_mouseDownSignal.connect(this, &LevelIntroAction::onSkipClick);
}

// 0x45A300
void GameLogic::deliverPizza(KitchenTile* kitchen, CustomerTile* customer, bool sendVehicle)
{
	Order* order = customer->getOrder();
	if (order->m_topping == m_comboTopping && (!order->m_event || order->m_event->m_deliver))
	{
		m_player->m_toppingCombo++;
	}
	else
	{
		m_comboTopping = order->m_topping;
		m_player->m_toppingCombo = 1;
	}
	if (m_player->m_toppingCombo > m_player->m_bestToppingCombos[m_comboTopping])
	{
		m_player->m_bestToppingCombos[m_comboTopping] = m_player->m_toppingCombo;
		int level = -1;
		for (unsigned int i = 0; i < m_comboTopping->m_upgrades.size(); i++)
		{
			if (m_player->m_toppingCombo == m_comboTopping->m_upgrades[i]->m_combo)
				level = i;
		}
		if (level > m_player->m_toppingLevels[m_comboTopping])
			PizzaFrenzy::getSounds()->playSound("bonus_upgrade", 1.0f, 1.0f);
	}
	m_activeOrders.pop_front();
	order->m_deliveryTime = m_deliveryTimer;
	m_deliveryTimer = 0.0f;
	if (order->m_event && !order->m_event->m_deliver)
	{
		customer->removePopup();
		updatePoliceStation();
	}
	else
	{
		m_player->m_ordersCompleted++;
		if (m_player->m_toppingCombo > 0)
		{
			float pitch = (m_player->m_toppingCombo - 1) * 0.05f + 1.0f;
			int frenzy = m_player->m_toppingCombo / g_frenzyComboSize;
			int step = m_player->m_toppingCombo % g_frenzyComboSize;
			m_map->setKitchenSigns(m_comboTopping, m_player->m_toppingCombo, frenzy);
			std::string frenzyText = getFrenzyText(m_player->m_toppingCombo);
			if (step == 0 && frenzy > 0)
			{
				std::string message;
				engine::format(message, "frenzy%d", (std::min)(frenzy - 1, 5));
				PizzaFrenzy::getSounds()->playSound(message, 1.0f, 1.0f);
				int bonus = g_frenzyComboBonus * frenzy;
				engine::TextItem number;
				number.setNumber(bonus, "");
				engine::format(message, "%s +$%s", frenzyText.c_str(), number.getText().c_str());
				m_screen->showMessage(message, kitchen->getPosition());
				m_player->addCash(bonus);
			}
			PizzaFrenzy::getSounds()->playSound("bonus_toppingCombo", 1.0f, pitch);
		}
		if (++m_deliveryStreak == 10)
		{
			m_deliveryStreak = 0;
			adjustSatisfaction(-g_satisfactionPenalty);
		}
		int numPizzas = order->m_numPizzas;
		int price = g_toppingPrice;
		if (order->m_topping->m_isPizza)
			price = g_premiumToppingPrice;
		int value = PizzaFrenzy::getGameStats()->getToppingUpgrade(order->m_topping)->m_bonusMultiplier * price;
		if (numPizzas > 0)
		{
			std::string text;
			engine::format(text, "$%d x%d", value, numPizzas);
			m_player->addCash(value * numPizzas);
			engine::Splat* splat = PizzaFrenzy::getSplatFactory()->createSplat("toppingBonus",
				kitchen->getPosition().x, kitchen->getPosition().y, NULL, NULL);
			splat->setText(text);
		}
		m_player->m_pizzasDelivered += numPizzas;
		if (sendVehicle)
			kitchen->dispatchVehicle(customer);
		else
			m_speedBonusTime = 0.0f;
	}
	onOrderFinished();
}

// 0x45A9A0
void GameLogic::spawnWave()
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
	if (getGame()->m_mode == 2 && g_maxWaveOrders < count)
		count = g_maxWaveOrders;
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
			m_ordersInPlay++;
			count--;
		}
	}
	for (int i = 0; i < count; i++)
	{
		Order* order = new Order();
		order->reset();
		m_pendingOrders.push_back(order);
		m_ordersInPlay++;
	}
	shuffleOrders(m_pendingOrders);
}

// 0x45AC60
void GameLogic::showEventPopup(SpecialEventPopup* popup)
{
	if (m_eventPopups.size() != 0)
		m_eventPopups.back()->setVisible(false);
	m_eventPopups.push_back(popup);
	m_screen->addChild(popup);
	popup->show();
}

// 0x45AD20
void GameLogic::selectOrder(CustomerTile* customer)
{
	m_selectedOrders.push_back(customer);
	customer->popupMouseOut();
	updateOrderCursor();
}

// 0x45ADB0
void GameLogic::onTileClicked(BuildingTile* building)
{
	if (!building->isPopupActive())
		return;
	PizzaPopup* popup = building->getPopup();
	if (m_heldCoupon)
	{
		if (popup->getPopupType() == 0)
		{
			applyHeldCoupon(static_cast<PizzaOrderPopup*>(popup));
		}
		else
		{
			PizzaFrenzy::getSounds()->playSound("order_invalidClick", 1.0f, 1.0f);
			return;
		}
	}
	if (popup->getPopupType() == 0)
	{
		// an order: select it if it matches the selected ones
		CustomerTile* customer = static_cast<CustomerTile*>(building);
		if (getSelectedCount() == 0)
		{
			selectOrder(customer);
			PizzaFrenzy::getSounds()->playSound("order_click1", 1.0f, 1.0f);
		}
		else if (m_selectedOrders[0]->getOrder()->m_topping == customer->getOrder()->m_topping)
		{
			int level = m_player->getToppingLevel(customer->getOrder()->m_topping);
			if (getSelectedCount() < level)
			{
				selectOrder(customer);
				PizzaFrenzy::getSounds()->playSound("order_click1", 1.0f, 1.0f);
			}
			else
			{
				m_screen->flashInvalidOrderMarker();
				PizzaFrenzy::getSounds()->playSound("order_invalidClick", 1.0f, 1.0f);
			}
		}
		else
		{
			m_screen->flashInvalidOrderMarker();
			PizzaFrenzy::getSounds()->playSound("order_invalidClick", 1.0f, 1.0f);
		}
	}
	else if (popup->getPopupType() == 1)
	{
		// a kitchen: queue the deliveries of the selected orders if it makes their pizza
		KitchenTile* kitchen = static_cast<KitchenTile*>(building);
		if (m_selectedOrders.empty())
		{
			PizzaFrenzy::getSounds()->playSound("order_invalidClick", 1.0f, 1.0f);
		}
		else if (m_selectedOrders[0]->getOrder()->m_topping == kitchen->getTopping())
		{
			PizzaFrenzy::getSounds()->playSound("order_click2", 1.0f, 1.0f);
			std::pair<CustomerTile*, KitchenTile*> delivery;
			delivery.second = kitchen;
			for (std::vector<engine::RefPtr<CustomerTile> >::iterator it = m_selectedOrders.begin();
				it != m_selectedOrders.end(); ++it)
			{
				CustomerTile* selected = *it;
				selected->getOrder();
				delivery.first = selected;
				m_deliveries.push_back(delivery);
			}
			clearSelection();
		}
		else
		{
			PizzaFrenzy::getSplatFactory()->createSplat("wrongPizza", kitchen->getPosition().x,
				kitchen->getPosition().y, NULL, NULL);
			for (std::vector<engine::RefPtr<CustomerTile> >::iterator it = m_selectedOrders.begin();
				it != m_selectedOrders.end(); ++it)
			{
				CustomerTile* selected = *it;
				selected->setOrderWaiting(false);
				selected->popupReleaseIfHovered();
			}
			clearSelection();
		}
	}
	else if (popup->getPopupType() == 2)
	{
		// the police station: send a car after each selected criminal
		int criminals = 0;
		for (std::vector<engine::RefPtr<CustomerTile> >::iterator it = m_selectedOrders.begin();
			it != m_selectedOrders.end(); ++it)
		{
			SpecialEvent* event = (*it)->getOrder()->m_event;
			if (event && !event->m_deliver)
				criminals++;
		}
		if (getSelectedCount() == 0 || criminals == 0)
		{
			PizzaFrenzy::getSounds()->playSound("order_invalidClick", 1.0f, 1.0f);
		}
		else
		{
			PizzaFrenzy::getSounds()->playSound("order_click2", 1.0f, 1.0f);
			for (std::vector<engine::RefPtr<CustomerTile> >::iterator it = m_selectedOrders.begin();
				it != m_selectedOrders.end();)
			{
				CustomerTile* selected = *it;
				SpecialEvent* event = selected->getOrder()->m_event;
				if (event && !event->m_deliver)
				{
					it = m_selectedOrders.erase(it);
					dispatchPolice(building, selected);
					popup->onDispatched();
				}
				else
				{
					++it;
				}
			}
			updateOrderCursor();
		}
	}
}

// 0x4D0470 (folded)
void GameLogic::onOrdersChanged()
{
}

// 0x451610 (folded)
bool GameLogic::allowsScrambling()
{
	return true;
}

// 0x45C240 (folded)
void LevelEndAction::exit()
{
	m_title->setFlags(16);
}
