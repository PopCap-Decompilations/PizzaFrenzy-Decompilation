#include "SpecialEventPopup.h"

#include <string>

#include "engine/Application.h"
#include "engine/Component.h"
#include "engine/FadeContainer.h"
#include "engine/Image.h"
#include "engine/Point.h"
#include "engine/Rect.h"
#include "engine/RectangleItem.h"
#include "engine/SoundMgr.h"
#include "engine/Splat.h"
#include "engine/SplatFactory.h"
#include "engine/TextTyper.h"

#include "Constants.h"
#include "CustomerTile.h"
#include "GameLogic.h"
#include "KitchenTile.h"
#include "Order.h"
#include "PizzaFrenzy.h"
#include "TileManager.h"

// 0x448A40
SpecialEventCloseAction::SpecialEventCloseAction(SpecialEventPopup* popup)
	: engine::State<SpecialEventPopup>(popup)
{
}

// 0x44A880
void SpecialEventPopup::updateActions(engine::UpdateContext& context)
{
	m_actions.update(context);
}

// 0x44A890
void SpecialEventWaitAction::update(engine::UpdateContext& context)
{
	m_owner->m_timeLeft -= context.elapsed;
	if (m_owner->m_timeLeft <= 0.0f)
		m_owner->onTimeout();
}

// 0x44A930
void SpecialEventPopup::say(const std::string& text)
{
	m_text->setText(text);
	m_panel->setVisible(true);
	m_text->start();
}

// 0x44A970
void SpecialEventCloseAction::enter()
{
	m_owner->m_panel->fade(false, 0.3f, 1.0f, false);
}

// 0x44A990
void SpecialEventCloseAction::update(engine::UpdateContext& context)
{
	if (m_owner->m_panel->isFadeFinished())
		PizzaFrenzy::getGameLogic()->removeEventPopup(m_owner);
}

// 0x44AA00
SpecialEventWaitAction::SpecialEventWaitAction(SpecialEventPopup* popup)
	: engine::State<SpecialEventPopup>(popup)
{
}

// 0x44AA90
void SpecialEventPopup::onPortraitArrived(int splatTag)
{
	m_portrait->setVisible(true);
	speak();
	m_actions.setState(new SpecialEventWaitAction(this));
}

// 0x44AB20
void SpecialEventPopup::init(engine::Component* deliverer, CustomerTile* customer)
{
	SpecialEvent* event = customer->getOrder()->m_event;
	m_sound = event->m_sound;
	m_portraitImage = event->m_portrait;
	m_deliverer = deliverer;
	m_customer = customer;

	m_panel = new engine::FadeContainer();
	addChild(m_panel);

	engine::RectangleItem* background = new engine::RectangleItem(engine::Rect(0.0f, 515.0f, 800.0f, 600.0f));
	background->setAlpha(0.25f);
	background->setColor(0.0f, 0.0f, 0.0f);
	m_panel->addChild(background);

	m_text = new engine::TextTyper();
	m_text->setFont(engine::getApplication()->getFont("res\\fonts\\dialogFontBig.xml"));
	m_text->setup(580.0f, g_typerSpeed, 0.0f);
	m_text->setPosition(110.0f, 525.0f);
	m_panel->addChild(m_text);

	m_portrait = new engine::Image(m_portraitImage);
	m_portrait->setPosition(45.0f, 530.0f);
	m_portrait->setVisible(false);
	m_panel->addChild(m_portrait);
}

// 0x44AEA0
SpecialEventPopup::~SpecialEventPopup()
{
}

// 0x44B030
std::string SpecialEventPopup::getTypeName() const
{
	return "SpecialEventPopup";
}

// 0x44B100
SpecialEventPopup::SpecialEventPopup()
{
}

// 0x44B230
void SpecialEventPopup::show()
{
	m_panel->fade(true, 0.5f, 1.0f, false);
	engine::Vector2 position = m_customer->getPosition();
	engine::Splat* splat = PizzaFrenzy::getSplatFactory()->createSplat("portraitSplat", position.x, position.y, 0, 0);
	splat->setContent(new engine::Image(m_portraitImage));
	splat->moveTo(45.0f, 530.0f);
	splat->m_onFinished.connect(this, &SpecialEventPopup::onPortraitArrived);
	PizzaFrenzy::getSounds()->playSound(m_sound, 1.0f, 1.0f);
	getGame()->getGameLogic()->deliverPizza(static_cast<KitchenTile*>(m_deliverer.get()), m_customer, true);
}
