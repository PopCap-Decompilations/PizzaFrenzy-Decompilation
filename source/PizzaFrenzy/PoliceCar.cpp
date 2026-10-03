// PoliceCar: the police car the PolicePopup sends to arrest a special customer.
#include <string>

#include "engine/AnimImage.h"
#include "engine/Application.h"
#include "engine/Image.h"
#include "engine/Selector.h"
#include "engine/SoundMgr.h"
#include "engine/SplatFactory.h"
#include "engine/Surface.h"
#include "BuildingTile.h"
#include "CustomerTile.h"
#include "GameLogic.h"
#include "Order.h"
#include "PathFinder.h"
#include "PizzaFrenzy.h"
#include "PoliceCar.h"
#include "TileManager.h"

// 0x432950
PoliceCar::~PoliceCar()
{
}

// 0x432A60
void PoliceCar::setDefinition(VehicleType* def)
{
	Vehicle::setDefinition(def);
	engine::Bitmap* bitmap = engine::getApplication()->getImage("res\\fx\\policeLightRed.jpg");
	bitmap->setPivotType(1);
	m_redLight = new engine::Image(bitmap);
	bitmap = engine::getApplication()->getImage("res\\fx\\policeLightBlue.jpg");
	bitmap->setPivotType(1);
	m_blueLight = new engine::Image(bitmap);
	m_lights = new engine::AnimImage();
	m_lights->addChild(m_redLight);
	m_lights->addChild(m_blueLight);
	m_lights->onFramesLoaded();
	m_lights->setFrameRate(4.0f);
	m_lights->play();
	addChild(m_lights);
}

// 0x432C70
void PoliceCar::faceTowards(const engine::Point& tile)
{
	Vehicle::faceTowards(tile);
	switch (m_selector->getSelection())
	{
	case 0:
		if (getScale().x < 0.0f)
		{
			m_redLight->setPosition(-1.0f, -4.0f);
			m_blueLight->setPosition(-1.0f, -8.0f);
		}
		else
		{
			m_redLight->setPosition(1.0f, -4.0f);
			m_blueLight->setPosition(1.0f, -8.0f);
		}
		break;
	case 1:
		m_redLight->setPosition(4.0f, -4.0f);
		m_blueLight->setPosition(-4.0f, -4.0f);
		break;
	case 2:
		m_redLight->setPosition(-4.0f, -5.0f);
		m_blueLight->setPosition(4.0f, -5.0f);
		break;
	}
}

// 0x432D60
PoliceCar::PoliceCar()
	: Vehicle("PoliceCar")
{
}

// 0x432E30
void PoliceCar::completeDelivery()
{
	SpecialEvent* event = m_target->getOrder()->m_event;
	if (event && !event->m_deliver)
	{
		m_target->deliverOrder();
		PizzaFrenzy::getSounds()->playSound("special_police_arrest", 1.0f, 1.0f);
	}
	else
	{
		m_target->dismissPopup();
		PizzaFrenzy::getGameLogic()->handleEvent("falseArrest");
		PizzaFrenzy::getSplatFactory()->createSplat("falseArrest", m_target->getPosition().x, m_target->getPosition().y, 0, 0);
	}
	m_path = 0;
	m_owner->removeVehicle(this);
}

// 0x433010
void PoliceCar::dispatchTo(CustomerTile* customer)
{
	Vehicle::dispatchTo(customer);
	PizzaFrenzy::getSounds()->playSound("special_police", 1.0f, 1.0f);
}
