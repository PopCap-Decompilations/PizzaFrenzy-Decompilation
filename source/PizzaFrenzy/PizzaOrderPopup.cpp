#include "PizzaOrderPopup.h"

#include <algorithm>
#include <string>
#include <vector>

#include "engine/AlphaPulse.h"
#include "engine/Application.h"
#include "engine/Color.h"
#include "engine/ColorPulse.h"
#include "engine/Component.h"
#include "engine/Container.h"
#include "engine/Image.h"
#include "engine/ScalePulse.h"
#include "engine/Selector.h"
#include "engine/SoundMgr.h"
#include "engine/Surface.h"
#include "CityMap.h"
#include "Constants.h"
#include "GameLogic.h"
#include "GameProgress.h"
#include "GameScreen.h"
#include "Level.h"
#include "Order.h"
#include "OrderHighlightState.h"
#include "PizzaFrenzy.h"
#include "TileManager.h"

// 0x451E50
void PizzaOrderPopup::addCapacitySegment(engine::Container* onLayer, engine::Container* flashLayer, float x, float y, int kind)
{
	// The switch has no default, as in the original: another kind would leave both images uninitialised
	// (VS2003 then uses y's slot, 0x451F46/0x451F4A), but every caller passes 0-3.
	engine::Bitmap* onImage;
	engine::Bitmap* flashImage;
	switch (kind)
	{
	case 0:
		onImage = engine::getApplication()->getImage("res/pizza/orderIndicatorL_on.jpg");
		flashImage = engine::getApplication()->getImage("res/pizza/orderIndicatorL_flash.jpg");
		x += 5.0f;
		y += 4.0f;
		break;
	case 1:
		onImage = engine::getApplication()->getImage("res/pizza/orderIndicatorC_on.jpg");
		flashImage = engine::getApplication()->getImage("res/pizza/orderIndicatorC_flash.jpg");
		x += 1.0f;
		y += 4.0f;
		break;
	case 2:
		onImage = engine::getApplication()->getImage("res/pizza/orderIndicatorR_on.jpg");
		flashImage = engine::getApplication()->getImage("res/pizza/orderIndicatorR_flash.jpg");
		x += 1.0f;
		y += 4.0f;
		break;
	case 3:
		onImage = engine::getApplication()->getImage("res/pizza/orderIndicator_on.jpg");
		flashImage = engine::getApplication()->getImage("res/pizza/orderIndicator_flash.jpg");
		x += 5.0f;
		y += 4.0f;
		break;
	}
	engine::Image* on = new engine::Image(onImage);
	on->setPosition(x, y);
	onLayer->addChild(on);
	engine::Image* flash = new engine::Image(flashImage);
	flash->setPosition(x, y);
	flashLayer->addChild(flash);
}

// 0x452020
void PizzaOrderPopup::onRightMouseDown()
{
	if (acceptsInput())
	{
		losePatience(false);
		PizzaFrenzy::getGameLogic()->notifyPopupRightClicked();
	}
}

// 0x452050 (folded)
void PizzaOrderPopup::onCloseFinished()
{
	remove();
}

// 0x452060 (folded)
void OrderSelectedState::update(engine::UpdateContext& context)
{
	m_orderPopup->m_patience -= context.elapsed;
	m_orderPopup->m_waitTime += context.elapsed;
}

// 0x452090
void OrderDeliveringState::update(engine::UpdateContext& context)
{
	m_orderPopup->m_waitTime += context.elapsed;
}

// 0x4520B0
void OrderInactiveState::enter()
{
	m_owner->setColorMode(1);
	m_owner->setColor(-0.1f, -0.1f, -0.1f);
}

// 0x4520E0
void OrderInactiveState::exit()
{
	m_owner->setColorMode(0);
	m_owner->setColor(0.0f, 0.0f, 0.0f);
}

// 0x452100
void OrderHangUpState::update(engine::UpdateContext& context)
{
	m_time -= context.elapsed;
	m_time = m_time < 0.0f ? 0.0f : m_time;
	float t = m_time / g_popupCloseTime;
	m_orderPopup->setAlpha(t);
	m_orderPopup->setScale(t * 0.5f + 0.5f);
	if (m_time <= 0.0f)
		m_orderPopup->remove();
}

// 0x452220
PizzaOrderPopup::PizzaOrderPopup()
	: m_scrambling(false)
{
}

// 0x4522F0
PizzaOrderPopup::~PizzaOrderPopup()
{
	m_order = NULL;
	m_pizzaIcons = NULL;
	m_flipSelector = NULL;
	m_faceSelector = NULL;
	m_button = NULL;
	m_portrait = NULL;
	m_backPortrait = NULL;
	m_unknownImage = NULL;
	m_pulseEffect = NULL;
	m_flashEffect = NULL;
	m_highlightEffect = NULL;
}

// 0x452660
void PizzaOrderPopup::buildPizzaIcons(engine::Container* container, Order* order)
{
	container->removeAllChildren();
	if (m_unknownImage->isVisible())
	{
		engine::Bitmap* image = engine::getApplication()->getImage("res\\pizza\\unknown.jpg");
		image->setPivotType(1);
		container->addChild(new engine::Image(image));
		return;
	}
	Topping* topping = order->m_topping;
	if (topping)
	{
		float y = (order->m_numPizzas - 1) * 1.5f;
		for (int i = 0; i < order->m_numPizzas; i++)
		{
			engine::Image* icon = new engine::Image(topping->m_pizzaImage);
			icon->setPosition(0.0f, y);
			container->addChild(icon);
			y -= 3.0f;
		}
	}
}

// 0x4527C0
void PizzaOrderPopup::showSelection(bool show)
{
	PizzaPopup::showSelection(show);
	if (m_faceSelector && m_faceSelector->getCount() > 1 && m_order && m_order->m_event
		&& !getGame()->getGameLogic()->m_level->getDeliverOnlyCharacter())
	{
		if (show)
			m_faceSelector->select(1, 0.0f, true);
		else
			m_faceSelector->select(0, 0.0f, true);
	}
}

// 0x452840
void PizzaOrderPopup::flashOrder()
{
	if (m_order && acceptsInput())
		addAnimator(m_highlightEffect);
}

// 0x452890 (folded)
Order* PizzaOrderPopup::getOrder() const
{
	return m_order;
}

// 0x4528A0
void PizzaOrderPopup::reveal()
{
	m_unknownImage->setVisible(false);
	m_pizzaIcons->setVisible(true);
}

// 0x4528C0
void OrderWaitState::enter()
{
	if (m_orderPopup->m_flipSelector)
	{
		m_orderPopup->m_flipSelector->select(0, 0.0f, true);
		m_orderPopup->m_flipSelector->setVisible(true);
	}
}

// 0x4528F0
void OrderImpatientState::enter()
{
	if (!getGame()->getGameLogic()->m_level->getDeliverOnlyCharacter()
		&& (!m_orderPopup->m_order->m_event || m_orderPopup->m_order->m_event->m_deliver))
	{
		m_orderPopup->addAnimator(m_orderPopup->m_flashEffect);
	}
}

// 0x452950
void OrderImpatientState::exit()
{
	m_orderPopup->removeAnimator(m_orderPopup->m_flashEffect);
	m_orderPopup->setAlpha(1.0f);
}

// 0x452980
void OrderSelectedState::enter()
{
	if (m_orderPopup->m_flipSelector)
	{
		m_orderPopup->m_flipSelector->select(1, 0.5f, false);
		m_orderPopup->m_flipSelector->setVisible(true);
	}
	m_orderPopup->m_selectionBox->setVisible(false);
}

// 0x4529D0 (folded)
bool OrderHangUpState::acceptsInput() const
{
	return false;
}

// 0x4529D0 (folded)
bool OrderHangUpState::isActive() const
{
	return false;
}

// 0x4529D0 (folded)
bool OrderSelectedState::acceptsInput() const
{
	return false;
}

// 0x4529D0 (folded)
bool OrderDeliveringState::acceptsInput() const
{
	return false;
}

// 0x4529D0 (folded)
bool OrderDeliveringState::isActive() const
{
	return false;
}

// 0x4529D0 (folded)
bool OrderInactiveState::acceptsInput() const
{
	return false;
}

// 0x4529E0
OrderWaitState::OrderWaitState(PizzaOrderPopup* popup)
	: PizzaPopupState(popup), m_orderPopup(popup)
{
}

// 0x452A40
OrderShuffleState::OrderShuffleState(PizzaOrderPopup* popup, Topping* target, bool silent)
	: PizzaPopupState(popup), m_target(target), m_orderPopup(popup), m_silent(silent)
{
}

// 0x452AB0
OrderInactiveState::OrderInactiveState(PizzaOrderPopup* popup)
	: PizzaPopupState(popup), m_orderPopup(popup)
{
}

// 0x452B30
void PizzaOrderPopup::setScrambling(bool scrambling)
{
	m_scrambling = scrambling;
	m_state.switchState(new OrderWaitState(this));
	if (m_scrambling)
		m_scrambleTimer = g_orderShuffleTime;
	else
		m_scrambleTimer = 0.0f;
}

// 0x452C10
void PizzaOrderPopup::replaceOrder(Order* order, bool animate)
{
	m_order = order;
	buildPizzaIcons(m_pizzaIcons, order);
	m_scrambling = false;
	m_scrambleTimer = 0.0f;
	reveal();
	if (animate)
		m_state.switchState(new OrderWaitState(this));
}

// 0x452D20
void PizzaOrderPopup::shuffleTo(Order* order, bool silent)
{
	m_order = order;
	m_scrambling = false;
	m_scrambleTimer = 0.0f;
	reveal();
	m_state.switchState(new OrderShuffleState(this, order->m_topping, silent));
}

// 0x452E20
void PizzaOrderPopup::highlight()
{
	PizzaPopup::highlight();
	m_state.switchState(new OrderHighlightState(this));
}

// 0x452ED0
void PizzaOrderPopup::select()
{
	m_state.switchState(new OrderSelectedState(this));
}

// 0x452F70
void PizzaOrderPopup::deselect()
{
	PizzaPopup::deselect();
	m_button->setVisible(true);
	m_state.switchState(new OrderWaitState(this));
}

// 0x453020
void PizzaOrderPopup::losePatience(bool warnOnly)
{
	if (m_order)
	{
		if (warnOnly)
			m_state.switchState(new OrderImpatientState(this));
		else
			m_state.switchState(new OrderHangUpState(this));
	}
}

// 0x453140
void PizzaOrderPopup::onDispatched()
{
	PizzaPopup::onDispatched();
	removeAllAnimators();
	setAlpha(1.0f);
	setColorMode(0);
	setScale(1.0f);
	m_state.switchState(new OrderDeliveringState(this));
}

// 0x453210
void PizzaOrderPopup::onDelivered()
{
	if (m_order)
		PizzaFrenzy::getGameLogic()->spawnTip(m_owner, m_order, m_waitTime);
	m_state.switchState(new PopupCloseState(this));
}

// 0x4532E0
void PizzaOrderPopup::close()
{
	if (m_order)
		m_state.switchState(new PopupCloseState(this));
}

// 0x453390
void PizzaOrderPopup::flipOver()
{
	m_unknownImage->setVisible(true);
	m_pizzaIcons->setVisible(false);
	setAlpha(1.0f);
	setScale(1.0f);
	m_state.switchState(new OrderWaitState(this));
}

// 0x453460
void PizzaOrderPopup::updateScramble(engine::UpdateContext& ctx)
{
	if (m_scrambleTimer > 0.0f)
	{
		m_scrambleTimer -= ctx.elapsed;
		if (m_scrambleTimer <= 0.0f)
			m_state.switchState(new OrderShuffleState(this, NULL, false));
	}
}

// 0x453510
void PizzaOrderPopup::onShowFinished()
{
	if (m_concealed)
		m_state.switchState(new OrderInactiveState(this));
	else
		m_state.switchState(new OrderWaitState(this));
}

// 0x453620
void OrderWaitState::update(engine::UpdateContext& context)
{
	m_orderPopup->m_patience -= context.elapsed;
	m_orderPopup->m_waitTime += context.elapsed;
	if (m_orderPopup->m_patience <= 2.0f)
		m_orderPopup->m_state.switchState(new OrderImpatientState(m_orderPopup));
	else
		m_orderPopup->updateScramble(context);
}

// 0x453730
void OrderImpatientState::update(engine::UpdateContext& context)
{
	m_orderPopup->m_patience -= context.elapsed;
	m_orderPopup->m_waitTime += context.elapsed;
	if (m_orderPopup->m_patience <= 0.0f)
		m_orderPopup->m_state.switchState(new OrderHangUpState(m_orderPopup));
	else
		m_orderPopup->updateScramble(context);
}

// 0x453840
void OrderShuffleState::enter()
{
	m_owner->removeAllAnimators();
	m_owner->setAlpha(1.0f);
	m_owner->setColorMode(0);
	m_owner->setScale(1.0f);
	m_time = 0.0f;
	m_toppings = &PizzaFrenzy::getCityMap()->getKitchenPizzas();
	if (m_target)
	{
		m_current = std::find(m_toppings->begin(), m_toppings->end(), m_target);
	}
	else
	{
		m_current = std::find(m_toppings->begin(), m_toppings->end(), m_orderPopup->m_order->m_topping);
		++m_current;
		if (m_current == m_toppings->end())
			m_current = m_toppings->begin();
		m_target = *m_current;
	}
	m_stepsLeft = 7;
}

// 0x453900
std::string PizzaOrderPopup::getTypeName() const
{
	return "PizzaOrderPopup";
}

// 0x453930
void PizzaOrderPopup::playOrderSound()
{
	if (!m_order->m_event && getGame()->m_mode != 1)
		PizzaFrenzy::getSounds()->playSound("popup", 1.0f, 1.0f);
	else
		PizzaFrenzy::getSounds()->playSound(m_order->m_character->m_greeting, 1.0f, 1.0f);
}

// 0x453A10
void OrderHangUpState::enter()
{
	m_orderPopup->m_closing = true;
	m_time = g_popupCloseTime;
	if (!m_orderPopup->m_order->m_event || m_orderPopup->m_order->m_event->m_deliver)
	{
		PizzaFrenzy::getGameLogic()->handleEvent("customerHangup");
		if (!getGame()->getGameLogic()->m_level->getDeliverOnlyCharacter()
			|| getGame()->getGameLogic()->m_level->getDeliverOnlyCharacter() == m_orderPopup->m_order->m_character)
		{
			PizzaFrenzy::getSounds()->playSound("popup_hangup", 1.0f, 1.0f);
		}
	}
	else
	{
		PizzaFrenzy::getSounds()->playSound("special_hangup", 1.0f, 1.0f);
		PizzaFrenzy::getGameLogic()->handleEvent("specialHangup");
	}
}

// 0x453C10
void OrderShuffleState::update(engine::UpdateContext& context)
{
	m_orderPopup->m_patience -= context.elapsed;
	m_time -= context.elapsed;
	if (m_time <= 0.0f)
	{
		m_time = 0.05f;
		Topping* topping = *m_current++;
		if (topping)
		{
			Order order(*m_orderPopup->m_order);
			order.m_topping = topping;
			m_orderPopup->buildPizzaIcons(m_orderPopup->m_pizzaIcons, &order);
			if (!m_silent)
				PizzaFrenzy::getSounds()->playSound("popup", 1.0f, 1.0f);
		}
		if (m_current == m_toppings->end())
			m_current = m_toppings->begin();
		if (--m_stepsLeft <= 0)
		{
			if (m_orderPopup->m_concealed)
				m_orderPopup->m_state.switchState(new OrderInactiveState(m_orderPopup));
			else
				m_orderPopup->m_state.switchState(new OrderWaitState(m_orderPopup));
		}
	}
}

// 0x453DD0
void OrderShuffleState::exit()
{
	if (m_stepsLeft > 0)
	{
		m_orderPopup->buildPizzaIcons(m_orderPopup->m_pizzaIcons, m_orderPopup->m_order);
		m_orderPopup->playOrderSound();
	}
	else
	{
		m_orderPopup->m_order->m_topping = m_target;
		m_orderPopup->buildPizzaIcons(m_orderPopup->m_pizzaIcons, m_orderPopup->m_order);
	}
	if (m_orderPopup->m_scrambling)
		m_orderPopup->m_scrambleTimer = g_orderShuffleTime;
	else
		m_orderPopup->m_scrambleTimer = 0.0f;
}

// 0x453E50
void PizzaOrderPopup::setOrder(Order* order, bool concealed)
{
	m_order = order;
	Character* character = order->m_character;
	m_portrait->setImage(character->m_image);
	m_backPortrait->setImage(character->m_image);
	buildPizzaIcons(m_pizzaIcons, order);
	m_flipSelector->select(0, 0.0f, true);
	if (m_order->m_event || getGame()->getGameLogic()->m_level->getDeliverOnlyCharacter())
		m_specialPortrait->setImage(m_order->m_character->m_image);
	if (getGame()->getGameLogic()->m_level->getDeliverOnlyCharacter())
		m_faceSelector->select(1, 0.0f, true);
	playOrderSound();
	m_scrambling = m_order->m_character->m_special == 1 && PizzaFrenzy::getGameLogic()->allowsScrambling();
	if (m_scrambling)
		m_scrambleTimer = g_orderShuffleTime;
	else
		m_scrambleTimer = 0.0f;
	m_concealed = concealed;
	m_patience = m_order->m_patience;
	m_state.switchState(new PopupShowState(this, !concealed));
}

// 0x454050
void PizzaOrderPopup::buildOrderCursor(engine::Container* cursor)
{
	int count = PizzaFrenzy::getGameLogic()->getSelectedCount();
	engine::Bitmap* buttonImage = engine::getApplication()->getImage("res/pizza/orderButton.jpg");
	buttonImage->setPivotType(1);
	float y = 0.0f;
	for (int i = 0; i < count; i++)
	{
		engine::Image* button = new engine::Image(buttonImage);
		button->setPosition(0.0f, y);
		y -= 5.0f;
		cursor->addChild(button);
	}
	engine::Container* icons = new engine::Container();
	cursor->addChild(icons);
	icons->setPosition(0.0f, y);
	buildPizzaIcons(icons, m_order);
	y -= 45.0f;
	engine::Container* marker = new engine::Container();
	int level = PizzaFrenzy::getGameStats()->getToppingLevel(m_order->m_topping);
	if (level == 1)
	{
		engine::Bitmap* image = engine::getApplication()->getImage("res/pizza/orderIndicator.jpg");
		engine::Image* indicator = new engine::Image(image);
		indicator->setPosition(-29.0f, y);
		cursor->addChild(indicator);
		if (count > 0)
			addCapacitySegment(cursor, marker, -29.0f, y, 3);
	}
	else
	{
		engine::Bitmap* image = engine::getApplication()->getImage("res/pizza/orderIndicatorL.jpg");
		engine::Image* left = new engine::Image(image);
		left->setPosition(-29.0f, y);
		cursor->addChild(left);
		if (count > 0)
			addCapacitySegment(cursor, marker, -29.0f, y, 0);
		float x = image->getWidth() - 29.0f;
		for (int i = 0; i < level - 2; i++)
		{
			image = engine::getApplication()->getImage("res/pizza/orderIndicatorC.jpg");
			engine::Image* center = new engine::Image(image);
			center->setPosition(x, y);
			cursor->addChild(center);
			if (count > i + 1)
				addCapacitySegment(cursor, marker, x, y, 1);
			x += image->getWidth();
		}
		image = engine::getApplication()->getImage("res/pizza/orderIndicatorR.jpg");
		engine::Image* right = new engine::Image(image);
		right->setPosition(x, y);
		cursor->addChild(right);
		if (count == level)
			addCapacitySegment(cursor, marker, x, y, 2);
	}
	cursor->addChild(marker);
	PizzaFrenzy::getGameScreen()->setInvalidOrderMarker(marker);
}

// 0x454480
void PizzaOrderPopup::init(BuildingTile* owner)
{
	PizzaPopup::init(owner);
	m_pulseEffect = new engine::ScalePulse(0.5f, 0.7f, 1.1f, 0.5f, 0.7f, 1.1f, 0.0f);
	m_flashEffect = new engine::AlphaPulse(0.5f, 0.6f, 1.0f, 0.0f);
	m_highlightEffect = new engine::ColorPulse(0.5f, engine::Color(0.5f, 0.5f, 0.5f), 0.0f, 1);
	m_flipSelector = new engine::Selector();
	addChild(m_flipSelector);
	m_content = new engine::Container();
	m_back = new engine::Container();
	m_flipSelector->addChild(m_content);
	m_flipSelector->addChild(m_back);
	engine::Bitmap* buttonImage = engine::getApplication()->getImage("res/pizza/orderButton.jpg");
	buttonImage->setPivotType(1);
	m_button = new engine::Image(buttonImage);
	m_button->setPosition(-4.0f, 2.0f);
	m_content->addChild(m_button);
	m_faceSelector = new engine::Selector();
	m_content->addChild(m_faceSelector);
	m_content = new engine::Container();
	m_faceSelector->addChild(m_content);
	m_faceSelector->select(0, 0.0f, true);
	m_portrait = new engine::Image(NULL);
	m_portrait->setPosition(-33.0f, -33.0f);
	m_content->addChild(m_portrait);
	m_backPortrait = new engine::Image(NULL);
	m_backPortrait->setPosition(0.0f, 0.0f);
	m_back->addChild(m_backPortrait);
	m_pizzaIcons = new engine::Container();
	m_content->addChild(m_pizzaIcons);
	m_pizzaIcons->setPosition(-4.0f, -2.0f);
	engine::Bitmap* unknownImage = engine::getApplication()->getImage("res\\pizza\\unknown.jpg");
	unknownImage->setPivotType(1);
	m_unknownImage = new engine::Image(unknownImage);
	m_content->addChild(m_unknownImage);
	m_unknownImage->setVisible(false);
	m_specialPortrait = new engine::Image(NULL);
	m_specialPortrait->setScale(0.8f, 0.8f);
	m_specialPortrait->setPosition(-2.0f, -3.0f);
	m_specialPortrait->setBlendMode(1);
	m_faceSelector->addChild(m_specialPortrait);
}

// 0x4D14F0 (folded)
int PizzaOrderPopup::getPopupType() const
{
	return 0;
}
