#include "SpecialEventPopups.h"

#include <algorithm>
#include <string>
#include <vector>

#include "engine/Application.h"
#include "engine/FadeContainer.h"
#include "engine/Image.h"
#include "engine/Rect.h"
#include "engine/SoundHandle.h"
#include "engine/SoundMgr.h"
#include "engine/Splat.h"
#include "engine/SplatFactory.h"
#include "engine/StringUtil.h"
#include "engine/Surface.h"
#include "CityMap.h"
#include "Constants.h"
#include "CustomerTile.h"
#include "GameLogic.h"
#include "GameProgress.h"
#include "GameScreen.h"
#include "HudScreen.h"
#include "KitchenTile.h"
#include "Order.h"
#include "PizzaFrenzy.h"
#include "PizzaOrderPopup.h"
#include "TileManager.h"
#include "Tip.h"

// 0x448AD0
void CrankCallerEventPopup::onTimeout()
{
	m_actions.setState(new SpecialEventCloseAction(this));
}

// 0x448B40
void CrankCallerEventPopup::speak()
{
	say("\"Ha-hah! Fooled you!\"");
	getGame()->getGameLogic()->resetCombo();
	m_timeLeft = 2.0f;
}

// 0x448BF0
void ThiefGossipPopup::show()
{
	std::vector<engine::RefPtr<CustomerTile> >& customers = PizzaFrenzy::getCityMap()->m_customers;
	bool differs = false;
	m_pizza = m_customer->getOrder()->m_topping;
	for (std::vector<engine::RefPtr<CustomerTile> >::iterator it = customers.begin(); it != customers.end(); ++it)
	{
		CustomerTile* customer = *it;
		if (customer->isPopupActive())
		{
			PizzaPopup* popup = customer->getPopup();
			if (popup->getPopupType() == 0)
			{
				PizzaOrderPopup* orderPopup = static_cast<PizzaOrderPopup*>(popup);
				orderPopup->setScrambling(false);
				if (orderPopup->getOrder()->m_topping != m_pizza)
					differs = true;
			}
		}
	}
	if (differs)
	{
		SpecialEventPopup::show();
		return;
	}
	getGame()->getGameLogic()->deliverPizza(static_cast<KitchenTile*>(m_deliverer.get()), m_customer, true);
	PizzaFrenzy::getGameLogic()->removeEventPopup(this);
}

// 0x448CD0
void ThiefGossipPopup::onTimeout()
{
	if (m_pizza)
	{
		std::vector<engine::RefPtr<CustomerTile> >& customers = PizzaFrenzy::getCityMap()->m_customers;
		bool first = true;
		for (std::vector<engine::RefPtr<CustomerTile> >::iterator it = customers.begin(); it != customers.end(); ++it)
		{
			CustomerTile* customer = *it;
			if (customer->isPopupActive())
			{
				if (customer->getPopup()->getPopupType() == 0)
				{
					PizzaOrderPopup* popup = static_cast<PizzaOrderPopup*>(customer->getPopup());
					Order* order = popup->getOrder();
					order->m_topping = m_pizza;
					popup->shuffleTo(order, !first);
					first = false;
				}
			}
		}
		PizzaFrenzy::getGameLogic()->onOrdersChanged();
		m_pizza = 0;
		m_timeLeft = 2.0f;
	}
	else
	{
		m_actions.setState(new SpecialEventCloseAction(this));
	}
}

// 0x448E00
void ThiefGossipPopup::speak()
{
	std::string text;
	engine::format(text, "Sweetie, you must try the %s pizza!!", m_pizza->m_display.c_str());
	say(text);
	m_timeLeft = 0;
}

// 0x448EB0
void GraffitiArtistEventPopup::onRemovedFrom(engine::Container* parent)
{
	if (m_sound)
	{
		m_sound->stop();
		m_sound = 0;
	}
	SpecialEventPopup::onRemovedFrom(parent);
}

// 0x448EE0
void GraffitiArtistEventPopup::speak()
{
	m_timeLeft = 0;
	getGame()->getGameLogic()->resetCombo();
	m_state = 0;
	engine::Bitmap* image = engine::getApplication()->getImage("res\\specialEvents\\graffitiArt.jpg");
	m_graffitiImage = new engine::Image(image);
	m_graffitiImage->updateBounds();
	m_graffitiImage->setUseSourceRect(true);
	m_panel->addChild(m_graffitiImage);
	m_graffitiImage->setPosition(110.0f, 500.0f);
	m_graffitiImage->setSourceRect(engine::IntRect(0, 0, 0, image->getHeight()));
	m_sound = PizzaFrenzy::getSounds()->getSound("special_graffiti");
	if (m_sound)
		m_sound->setLooping(true);
}

// 0x4490A0
void GraffitiArtistEventPopup::onTimeout()
{
	if (m_state == 0)
	{
		if (m_sound)
			m_sound->play();
		m_timeLeft = 0;
		m_state = 1;
	}
	else if (m_state == 1)
	{
		// m_timeLeft keeps counting down below 0: the picture is revealed over 2 s
		float width = m_timeLeft * -0.5f * m_graffitiImage->getImageWidth();
		m_graffitiImage->setSourceRect(engine::IntRect(0, 0, (int)width, m_graffitiImage->getImageHeight()));
		if (m_graffitiImage->getImageWidth() < width)
		{
			if (m_sound)
				m_sound->stop();
			m_timeLeft = 0;
			m_state = 2;
		}
	}
	else if (m_state == 2)
	{
		engine::Vector2 position = PizzaFrenzy::getHud()->getStarLevelPosition();
		engine::Splat* splat = PizzaFrenzy::getSplatFactory()->createSplat("StealSatisfaction", position.x, position.y,
			0, 0);
		splat->moveTo(m_portrait->getPosition().x, m_portrait->getPosition().y);
		PizzaFrenzy::getGameLogic()->adjustSatisfaction(g_satisfactionPenalty);
		m_state = 3;
		m_timeLeft = 1.0f;
	}
	else if (m_state == 3)
	{
		m_panel->removeChild(m_graffitiImage);
		m_actions.setState(new SpecialEventCloseAction(this));
	}
}

// 0x449310
void MonkEventPopup::onTimeout()
{
	m_actions.setState(new SpecialEventCloseAction(this));
}

// 0x449380
void MonkEventPopup::speak()
{
	say("\"Do not rush! Be at peace!\"");
	PizzaFrenzy::getGameLogic()->setTimeEffect(8.0f, 0.33f);
	m_timeLeft = 2.0f;
}

// 0x449440
void MovieStarEventPopup::init(engine::Component* deliverer, CustomerTile* customer)
{
	SpecialEventPopup::init(deliverer, customer);
	m_billsLeft = g_thiefBillCount / 2;
}

// 0x449470
void MovieStarEventPopup::onBillArrived(int eventArg)
{
	PizzaFrenzy::getGameStats()->addCash(-g_stolenBillCash);
}

// 0x449530
void MovieStarEventPopup::speak()
{
	say("\"This pizza looks as fabulous as me! Here's a huge tip.\"");
	m_timeLeft = 0;
}

// 0x449650
void MovieStarEventPopup::onTimeout()
{
	if (m_billsLeft > 0)
	{
		m_billsLeft--;
		if (m_billsLeft > 0)
			m_timeLeft = g_billInterval;
		else
			m_timeLeft = 1.0f;
		engine::Splat* splat = PizzaFrenzy::getSplatFactory()->createSplat("StealBills",
			m_portrait->getPosition().x, m_portrait->getPosition().y, 0, 0);
		engine::Vector2 target = PizzaFrenzy::getHud()->m_tipJarCountPos;
		splat->moveTo(target.x, target.y);
		splat->m_onFinished.connect(this, &MovieStarEventPopup::onBillArrived);
	}
	else
	{
		m_actions.setState(new SpecialEventCloseAction(this));
	}
}

// 0x4497D0
void ScramblerPopup::show()
{
	std::vector<engine::RefPtr<CustomerTile> >& customers = PizzaFrenzy::getCityMap()->m_customers;
	for (std::vector<engine::RefPtr<CustomerTile> >::iterator it = customers.begin(); it != customers.end(); ++it)
	{
		CustomerTile* customer = *it;
		if (customer->isPopupActive() && customer != m_customer)
		{
			SpecialEventPopup::show();
			return;
		}
	}
	getGame()->getGameLogic()->deliverPizza(static_cast<KitchenTile*>(m_deliverer.get()), m_customer, true);
	PizzaFrenzy::getGameLogic()->removeEventPopup(this);
}

// 0x449850
void ScramblerPopup::onTimeout()
{
	if (!m_scrambled)
	{
		CityMap* map = PizzaFrenzy::getCityMap();
		std::vector<engine::RefPtr<Topping> >& pizzas = map->getKitchenPizzas();
		bool first = true;
		for (std::vector<engine::RefPtr<CustomerTile> >::iterator it = map->m_customers.begin();
			it != map->m_customers.end(); ++it)
		{
			CustomerTile* customer = *it;
			if (customer->isPopupActive())
			{
				if (customer->getPopup()->getPopupType() == 0)
				{
					PizzaOrderPopup* popup = static_cast<PizzaOrderPopup*>(customer->getPopup());
					Order* order = popup->getOrder();
					std::vector<engine::RefPtr<Topping> >::iterator next =
						std::find(pizzas.begin(), pizzas.end(), order->m_topping);
					++next;
					if (next == pizzas.end())
						next = pizzas.begin();
					order->m_topping = *next;
					popup->shuffleTo(order, !first);
					first = false;
				}
			}
		}
		PizzaFrenzy::getGameLogic()->onOrdersChanged();
		m_scrambled = true;
		m_timeLeft = 2.0f;
	}
	else
	{
		m_actions.setState(new SpecialEventCloseAction(this));
	}
}

// 0x4499E0
void ScramblerPopup::speak()
{
	say("\"Everybody switch!\"");
	m_timeLeft = 0;
	m_scrambled = false;
}

// 0x449A70
void ThiefEventPopup::init(engine::Component* deliverer, CustomerTile* customer)
{
	SpecialEventPopup::init(deliverer, customer);
	m_billsLeft = g_thiefBillCount;
}

// 0x449AA0
void ThiefEventPopup::speak()
{
	say("\"I'll take that money, thanks!\"");
	m_timeLeft = 0;
	getGame()->getGameLogic()->resetCombo();
}

// 0x449B50
void ThiefEventPopup::onTimeout()
{
	if (m_billsLeft > 0 && PizzaFrenzy::getGameStats()->m_cash > 0)
	{
		m_billsLeft--;
		if (m_billsLeft > 0)
			m_timeLeft = g_billInterval;
		else
			m_timeLeft = 1.0f;
		engine::Vector2 position = PizzaFrenzy::getHud()->m_tipJarCountPos;
		engine::Splat* splat = PizzaFrenzy::getSplatFactory()->createSplat("StealBills", position.x, position.y, 0, 0);
		splat->moveTo(m_portrait->getPosition().x, m_portrait->getPosition().y);
		PizzaFrenzy::getGameStats()->addCash(g_stolenBillCash);
	}
	else
	{
		m_actions.setState(new SpecialEventCloseAction(this));
	}
}

// 0x449CD0
TreasureHunterEventPopup::~TreasureHunterEventPopup()
{
	if (m_hunterImage)
		m_hunterImage->setFlags(0x10);
}

// 0x449DB0
void TreasureHunterEventPopup::onTimeout()
{
	m_stepsLeft--;
	std::vector<engine::RefPtr<Tip> >::iterator it;
	for (it = m_tips->begin(); it != m_tips->end(); ++it)
	{
		if ((*it)->m_clickable)
			break;
	}
	if (it != m_tips->end())
	{
		m_hunterImage->setPosition((*it)->getPosition());
		m_hunterImage->setVisible(true);
		(*it)->collect();
	}
	m_timeLeft = 0.5f;
	if (m_stepsLeft == 0)
	{
		m_hunterImage->setFlags(0x10);
		m_hunterImage = 0;
		m_actions.setState(new SpecialEventCloseAction(this));
	}
}

// 0x449ED0
void TreasureHunterEventPopup::speak()
{
	say("\"I can pick up those tips for you.\"");
	m_timeLeft = 0.5f;
	m_tips = &PizzaFrenzy::getCityMap()->m_tips;
	m_stepsLeft = 10;
	m_hunterImage = new engine::Image(m_customer->getOrder()->m_character->m_image);
	PizzaFrenzy::getGameScreen()->addChild(m_hunterImage);
	m_hunterImage->setVisible(false);
}

// 0x44A020
std::string TreasureHunterEventPopup::getTypeName() const
{
	return "TreasureHunterEventPopup";
}

// 0x44A050
SpecialEventPopupFactory::SpecialEventPopupFactory()
{
}

// 0x44A0C0
ThiefGossipPopup::ThiefGossipPopup()
{
}

// 0x44A120
MovieStarEventPopup::MovieStarEventPopup()
{
}

// 0x44A180
CrankCallerEventPopup::CrankCallerEventPopup()
{
}

// 0x44A1E0
MonkEventPopup::MonkEventPopup()
{
}

// 0x44A240
ScramblerPopup::ScramblerPopup()
{
}

// 0x44A2E0
TreasureHunterEventPopup::TreasureHunterEventPopup()
{
}

// 0x44A370
GraffitiArtistEventPopup::GraffitiArtistEventPopup()
{
}

// 0x44A490
SpecialEventPopup* SpecialEventPopupFactory::create(const std::string& type)
{
	SpecialEventPopup* popup = 0;
	if (type == "Thief")
		popup = new ThiefEventPopup();
	else if (type == "Gossip")
		popup = new ThiefGossipPopup();
	else if (type == "MovieStar")
		popup = new MovieStarEventPopup();
	else if (type == "CrankCaller")
		popup = new CrankCallerEventPopup();
	else if (type == "Monk")
		popup = new MonkEventPopup();
	else if (type == "Clown")
		popup = new ScramblerPopup();
	else if (type == "TreasureHunter")
		popup = new TreasureHunterEventPopup();
	else if (type == "GraffitiArtist")
		popup = new GraffitiArtistEventPopup();
	return popup;
}

// 0x44A730
std::string ThiefEventPopup::getTypeName() const
{
	return "ThiefEventPopup";
}

// 0x44A760
std::string ThiefGossipPopup::getTypeName() const
{
	return "ThiefGossipPopup";
}

// 0x44A790
std::string MovieStarEventPopup::getTypeName() const
{
	return "MovieStarEventPopup";
}

// 0x44A7C0
std::string CrankCallerEventPopup::getTypeName() const
{
	return "CrankCallerEventPopup";
}

// 0x44A7F0
std::string MonkEventPopup::getTypeName() const
{
	return "MonkEventPopup";
}

// 0x44A820
std::string ScramblerPopup::getTypeName() const
{
	return "ScramblerPopup";
}

// 0x44A850
std::string GraffitiArtistEventPopup::getTypeName() const
{
	return "GraffitiArtistEventPopup";
}
