// Tip: the cash tip left on the city map after a delivery, and its states (appear, wait, blink, vanish).
#include <string>

#include "engine/AlphaPulse.h"
#include "engine/Application.h"
#include "engine/Image.h"
#include "engine/Selector.h"
#include "engine/SoundMgr.h"
#include "engine/Splat.h"
#include "engine/SplatFactory.h"
#include "engine/StringUtil.h"
#include "engine/Surface.h"
#include "CityMap.h"
#include "Constants.h"
#include "GameLogic.h"
#include "GameProgress.h"
#include "HudScreen.h"
#include "PizzaFrenzy.h"
#include "TileManager.h"
#include "Tip.h"

// 0x42D5A0
TipAppearState::TipAppearState(Tip* tip)
	: engine::State<Tip>(tip)
{
}

// 0x42E4C0
void Tip::onCashArrived(int eventArg)
{
	PizzaFrenzy::getGameLogic()->removeTip(this);
	PizzaFrenzy::getGameLogic()->collectMoney(m_amount);
}

// 0x42E4F0
void Tip::onMouseDown()
{
	if (m_clickable)
		collect();
}

// 0x42E510
void Tip::placeOnMap(CityMap* map, const engine::Vector2& pos)
{
	m_map = map;
	setPosition(pos);
	m_map->addTip(this);
}

// 0x42E540
void Tip::onRemovedFrom(engine::Container* parent)
{
	engine::Container::onRemovedFrom(parent);
	m_map->removeTip(this);
}

// 0x42E560
void Tip::updateOnMap(engine::UpdateContext& ctx)
{
	m_states.update(ctx);
	engine::Container::update(ctx);
}

// 0x42E590
void TipAppearState::enter()
{
	m_time = 0.2f;
	m_owner->setAlpha(0.0f);
	m_owner->setScale(0.5f);
}

// 0x42E5C0
void TipAppearState::update(engine::UpdateContext& context)
{
	m_time -= context.elapsed;
	if (m_time < 0.0f)
		m_time = 0.0f;
	float t = m_time * 5.0f;
	m_owner->setAlpha(1.0f - t);
	float scale = 1.0f - t * 0.5f;
	m_owner->setScale(scale, scale);
	if (m_time <= 0.0f)
		m_owner->onAppeared();
}

// 0x42E650
void TipWaitState::enter()
{
	m_time = 5.0f;
}

// 0x42E660
void TipBlinkState::enter()
{
	m_owner->addAnimator(new engine::AlphaPulse(0.5f, 0.6f, 1.0f, 0.0f));
	m_time = 5.0f;
}

// 0x42E6F0
void TipBlinkState::exit()
{
	m_owner->removeAllAnimators();
}

// 0x42E700
void TipVanishState::update(engine::UpdateContext& context)
{
	m_time -= context.elapsed;
	m_time = m_time < 0.0f ? 0.0f : m_time;
	float t = m_time * 5.0f;
	m_owner->setAlpha(t);
	float scale = t * 0.5f + 0.5f;
	m_owner->setScale(scale, scale);
	if (m_time <= 0.0f)
		PizzaFrenzy::getGameLogic()->removeTip(m_owner);
}

// 0x42E790
TipWaitState::TipWaitState(Tip* tip)
	: engine::State<Tip>(tip)
{
}

// 0x42E820
TipBlinkState::TipBlinkState(Tip* tip)
	: engine::State<Tip>(tip)
{
}

// 0x42E8B0
TipVanishState::TipVanishState(Tip* tip)
	: engine::State<Tip>(tip)
{
}

// 0x42E940
void Tip::createGraphics()
{
	m_background = new engine::Selector();
	engine::Bitmap* image = engine::getApplication()->getImage("res\\tips\\tipBackground.jpg");
	image->setPivotType(1);
	m_background->addChild(new engine::Image(image));
	image = engine::getApplication()->getImage("res\\tips\\tipBackground_on.jpg");
	image->setPivotType(1);
	m_background->addChild(new engine::Image(image));
	m_background->select(0, 0.0f, true);
	addChild(m_background);
}

// 0x42EAC0
void Tip::onAppeared()
{
	m_states.setState(new TipWaitState(this));
}

// 0x42EB30
void Tip::onMouseLeave()
{
	m_background->select(0, 0.0f, true);
}

// 0x42EB50
void TipWaitState::update(engine::UpdateContext& context)
{
	m_time -= context.elapsed;
	if (m_time <= 0.0f && m_owner->m_clickable)
		m_owner->m_states.setState(new TipBlinkState(m_owner));
}

// 0x42EBF0
void TipBlinkState::update(engine::UpdateContext& context)
{
	m_time -= context.elapsed;
	if (m_time <= 0.0f)
		m_owner->m_states.setState(new TipVanishState(m_owner));
}

// 0x42EC80
void Tip::init(float size, Topping* topping)
{
	m_size = size;
	int value;
	if (topping)
	{
		ToppingUpgrade* upgrade = PizzaFrenzy::getGameStats()->getToppingUpgrade(topping);
		if (upgrade == 0)
			value = 5 * g_tipValueScale;
		else
			value = upgrade->m_bonusMultiplier * g_tipValueScale;
	}
	else
	{
		value = g_defaultTipValue;
	}
	m_amount = (int)(value * size);
	PizzaFrenzy::getGameStats()->m_bonus += m_amount;
	createGraphics();
	std::string path;
	engine::format(path, "res\\tips\\cash-%d.jpg", (int)size);
	engine::Bitmap* bitmap = engine::getApplication()->getImage(path.c_str());
	if (bitmap)
	{
		bitmap->setPivotType(1);
		engine::Image* image = new engine::Image(bitmap);
		image->setPosition(-1.0f, 3.0f);
		addChild(image);
	}
	m_clickable = true;
	m_states.setState(new TipAppearState(this));
}

// 0x42EE40
void Tip::onMouseEnter()
{
	if (m_clickable)
	{
		m_background->select(1, 0.0f, true);
		PizzaFrenzy::getSounds()->playSound("order_mouseover1", 1.0f, 1.0f);
	}
}

// 0x42EEF0
void TipVanishState::enter()
{
	m_time = 0.2f;
	m_owner->m_clickable = false;
	PizzaFrenzy::getSounds()->playSound("tip_lost", 0.8f, 1.0f);
}

// 0x42EF80
Tip::~Tip()
{
}

// 0x42F050
Tip::Tip()
{
	maskFlags(4);
}

// 0x42F160
void Tip::collect()
{
	PizzaFrenzy::getSounds()->playSound("order_click1", 1.0f, 1.0f);
	PizzaFrenzy::getSounds()->playSound("tip_cash", 1.0f, 1.0f);
	m_clickable = false;
	engine::Splat* splat = PizzaFrenzy::getSplatFactory()->createSplat("cashTipSplat", getPosition().x, getPosition().y, 0, 0);
	std::string text;
	engine::format(text, "$%d", m_amount);
	splat->setText(text);
	splat = PizzaFrenzy::getSplatFactory()->createSplat("tipSplat", getPosition().x, getPosition().y, 0, 0);
	std::string path;
	engine::format(path, "res\\tips\\cash-%d.jpg", (int)m_size);
	splat->setContent(new engine::Image(engine::getApplication()->getImage(path.c_str())));
	engine::Vector2 target(PizzaFrenzy::getHud()->m_tipJarCountPos);
	splat->moveTo(target.x, target.y);
	splat->m_onFinished.connect(this, &Tip::onCashArrived);
	setVisible(false);
	setEnabled(false);
}
