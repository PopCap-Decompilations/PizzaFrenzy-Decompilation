// CitizenAward: the tip left when a criminal's order is settled.
#include <string>

#include "engine/Application.h"
#include "engine/Image.h"
#include "engine/SoundMgr.h"
#include "engine/Splat.h"
#include "engine/SplatFactory.h"
#include "engine/StringUtil.h"
#include "engine/Surface.h"
#include "CitizenAward.h"
#include "GameProgress.h"
#include "HudScreen.h"
#include "PizzaFrenzy.h"

// 0x42D630
void CitizenAward::init(float size, Topping* topping)
{
	m_amount = 500;
	PizzaFrenzy::getGameStats()->m_bonus += m_amount;
	createGraphics();
	engine::Bitmap* bitmap = engine::getApplication()->getImage("res\\tips\\citizenAward.jpg");
	if (bitmap)
	{
		bitmap->setPivotType(1);
		engine::Image* image = new engine::Image(bitmap);
		image->setPosition(-1.0f, -6.0f);
		addChild(image);
	}
	setFlags(2);
	m_clickable = true;
	m_states.setState(new TipAppearState(this));
}

// 0x42D830
void CitizenAward::collect()
{
	PizzaFrenzy::getSounds()->playSound("order_click1", 1.0f, 1.0f);
	PizzaFrenzy::getSounds()->playSound("tip_citizenAward", 0.8f, 1.0f);
	m_clickable = false;
	engine::Splat* splat = PizzaFrenzy::getSplatFactory()->createSplat("cashTipSplat", getPosition().x, getPosition().y, 0, 0);
	std::string text;
	engine::format(text, "Citizen Award! $%d", m_amount);
	splat->setText(text);
	splat = PizzaFrenzy::getSplatFactory()->createSplat("tipSplat", getPosition().x, getPosition().y, 0, 0);
	splat->setContent(new engine::Image(engine::getApplication()->getImage("res\\tips\\citizenAward.jpg")));
	engine::Vector2 target(PizzaFrenzy::getHud()->m_tipJarCountPos);
	splat->moveTo(target.x, target.y);
	splat->m_onFinished.connect<Tip>(this, &Tip::onCashArrived);
	setVisible(false);
}

// 0x455CC0
CitizenAward::CitizenAward()
{
}
