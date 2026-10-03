// BalloonTip: the tip the prize blimp drops, and its states (drop, float).
#include <string>

#include "engine/Application.h"
#include "engine/Image.h"
#include "engine/Oscillator.h"
#include "engine/Range.h"
#include "engine/Rect.h"
#include "engine/Selector.h"
#include "engine/SoundMgr.h"
#include "engine/Surface.h"
#include "BalloonTip.h"
#include "Constants.h"
#include "GameLogic.h"
#include "GameProgress.h"
#include "PizzaFrenzy.h"

// 0x42DAF0
BalloonTip::~BalloonTip()
{
}

// 0x42DB30
void BalloonTipFloatState::update(engine::UpdateContext& context)
{
	engine::Vector2 pos(m_owner->getPosition());
	pos += m_velocity * context.elapsed;
	m_owner->setPosition(pos);
	if (pos.y > 700.0f || pos.x < -50.0f || pos.x > 850.0f)
		PizzaFrenzy::getGameLogic()->removeTip(m_owner);
}

// 0x42DBC0
void BalloonTipDropState::enter()
{
	m_velocity.x = engine::randomFloat(-100.0f, 100.0f);
	m_velocity.y = 50.0f;
	m_time = engine::randomFloat(0.3f, 0.6f);
}

// 0x42DC40
BalloonTip::BalloonTip()
{
}

// 0x42DCC0
BalloonTipFloatState::BalloonTipFloatState(BalloonTip* tip)
	: engine::State<BalloonTip>(tip)
{
}

// 0x42DD90
BalloonTipDropState::BalloonTipDropState(BalloonTip* tip)
	: engine::State<BalloonTip>(tip)
{
}

// 0x42DE60
void BalloonTip::createGraphics()
{
	engine::Bitmap* bitmap = engine::getApplication()->getImage("res\\tips\\balloonTip_shadow.jpg");
	bitmap->setPivotType(1);
	engine::Image* shadow = new engine::Image(bitmap);
	shadow->setPosition(-67.0f, 113.0f);
	addChild(shadow);
	m_background = new engine::Selector();
	bitmap = engine::getApplication()->getImage("res\\tips\\balloonTip.jpg");
	bitmap->setPivotType(1);
	m_background->addChild(new engine::Image(bitmap));
	bitmap = engine::getApplication()->getImage("res\\tips\\balloonTip_on.jpg");
	bitmap->setPivotType(1);
	m_background->addChild(new engine::Image(bitmap));
	bitmap = engine::getApplication()->getImage("res\\tips\\balloonTip_closed.jpg");
	bitmap->setPivotType(1);
	m_background->addChild(new engine::Image(bitmap));
	m_background->select(2, 0.0f, true);
	addChild(m_background);
}

// 0x42E090
void BalloonTip::updateBounds()
{
	m_bounds = m_rect;
	m_background->updateBounds();
	m_bounds.unite(m_background->getBounds());
	m_bounds.scale(m_scale.x, m_scale.y);
	m_bounds.normalize();
	m_bounds.offset(m_position.x, m_position.y);
	removeTreeFlags(8);
}

// 0x42E110
void BalloonTip::init(float size, Topping* topping)
{
	m_size = size;
	m_amount = (int)(g_defaultTipValue * size);
	PizzaFrenzy::getGameStats()->m_bonus += m_amount;
	createGraphics();
	setFlags(2);
	m_clickable = false;
	m_states.setState(new TipAppearState(this));
}

// 0x42E1D0
void BalloonTip::onAppeared()
{
	m_states.setState(new BalloonTipDropState(this));
}

// 0x42E240
void BalloonTipDropState::update(engine::UpdateContext& context)
{
	engine::Vector2 pos(m_owner->getPosition());
	m_velocity.y += context.elapsed * 700.0f;
	pos += m_velocity * context.elapsed;
	m_owner->setPosition(pos);
	if (pos.y > 700.0f || pos.x < -50.0f || pos.x > 850.0f)
		PizzaFrenzy::getGameLogic()->removeTip(m_owner);
	m_time -= context.elapsed;
	if (m_time <= 0.0f)
		m_owner->m_states.setState(new BalloonTipFloatState(m_owner));
}

// 0x42E360
void BalloonTipFloatState::enter()
{
	m_owner->m_background->select(0, 0.0f, true);
	m_velocity.x = engine::randomFloat(-75.0f, 75.0f);
	m_velocity.y = engine::randomFloat(50.0f, 75.0f);
	PizzaFrenzy::getSounds()->playSound("bonus_blimpDropTip", 1.0f, 1.0f);
	m_owner->m_clickable = true;
	float period = engine::randomFloat(1.5f, 1.5f);
	float amplitude = engine::randomFloat(20.0f, 20.0f);
	m_owner->addAnimator(new engine::Oscillator(period, 0.0f, amplitude, 0.0f, 0.0f));
}
