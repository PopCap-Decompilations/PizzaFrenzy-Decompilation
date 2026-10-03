// HudScreen: the in-game HUD (order count and slips, tip jar, satisfaction stars, coupons).
#include <algorithm>
#include <list>
#include <string>
#include <vector>

#include "engine/AlphaPulse.h"
#include "engine/Application.h"
#include "engine/Container.h"
#include "engine/Image.h"
#include "engine/ParticleSystem.h"
#include "engine/Rect.h"
#include "engine/SoundHandle.h"
#include "engine/SoundMgr.h"
#include "engine/Splat.h"
#include "engine/SplatFactory.h"
#include "engine/Surface.h"
#include "engine/TextItem.h"

#include "CityMap.h"
#include "Constants.h"
#include "CouponFrame.h"
#include "GameLogic.h"
#include "GameProgress.h"
#include "HudScreen.h"
#include "PizzaFrenzy.h"
#include "TileManager.h"

// 0x438270
void HudScreen::setPaused(bool paused)
{
	if (paused)
	{
		if (m_rollupSound && m_rollupSound->isPlaying())
			m_rollupSound->stop();
	}
	else
	{
		if (m_rollupSound && m_tipJarRolling)
			m_rollupSound->play();
	}
}

// 0x4382C0
std::list<engine::RefPtr<CouponFrame> >& HudScreen::getCoupons()
{
	return m_coupons;
}

// 0x4382D0: the five stars bottom to top, each 20 points of the satisfaction shown (blended from m_starsFrom);
// the partly filled star is clipped from the top.
void HudScreen::updateStars()
{
	int satisfaction = m_stats->m_satisfaction - (int)((m_stats->m_satisfaction - m_starsFrom) * m_starBlend);
	for (int i = 0; i < 5; ++i)
	{
		float x = m_starsBounds.left;
		float y = m_starsBounds.bottom - m_starsBounds.getHeight() * 0.1f * (2 * i + 1);
		if (satisfaction >= i * 20 + 20)
		{
			m_stars[i]->setSourceRect(engine::IntRect(0, 0, m_stars[i]->getImageWidth(), m_stars[i]->getImageHeight()));
			m_stars[i]->setPosition(x, y);
		}
		else if (satisfaction > i * 20)
		{
			float fill = 1.0f - (satisfaction - i * 20) * 0.05f;
			int top = (int)(m_stars[i]->getImageHeight() * fill);
			m_stars[i]->setSourceRect(engine::IntRect(0, top, m_stars[i]->getImageWidth(), m_stars[i]->getImageHeight()));
			m_stars[i]->setPosition(x, top + y);
		}
		else
			m_stars[i]->setSourceRect(engine::IntRect(0, 0, 0, 0));
	}
}

// 0x4384C0
engine::Vector2 HudScreen::getStarLevelPosition() const
{
	int satisfaction = m_stats->m_satisfaction - (int)((m_stats->m_satisfaction - m_starsFrom) * m_starBlend);
	float y = m_starsBounds.bottom - m_starsBounds.getHeight() * 0.1f * (satisfaction * 0.1f + 1.0f);
	return engine::Vector2(m_starsBounds.left, y + m_stars[0]->getImageHeight() * 0.5f);
}

// 0x438570
void HudScreen::showScoreBackground()
{
	engine::AlphaPulse* pulse = new engine::AlphaPulse(0.5f, 0.0f, 1.0f, 0.0f);
	m_scoreBkg->setVisible(true);
	m_scoreBkg->addAnimator(pulse);
	m_scoreBkgTimer = 2.0f;
}

// 0x438610
void HudScreen::showStarWarning(bool show)
{
	m_starWarnings->removeAllAnimators();
	if (show)
	{
		m_starWarnings->addAnimator(new engine::AlphaPulse(0.5f, 0.0f, 1.0f, 0.0f));
		m_starWarnings->setVisible(true);
	}
	else
		m_starWarnings->setVisible(false);
}

// 0x4386D0
void HudScreen::startTipJarRollup(int fromValue)
{
	m_tipJarRolling = true;
	m_tipJarValue = fromValue;
	if (m_tipJarValue > m_stats->m_cash)
		m_tipJarCoins->start();
	if (m_rollupSound)
	{
		m_rollupSound->setLooping(true);
		m_rollupSound->play();
	}
}

// 0x438720: rolls the shown value toward the cash at the level's cash target per g_tipJarRollupTime seconds
void HudScreen::updateTipJarRollup(engine::UpdateContext& context)
{
	int step = (int)(m_stats->m_cashGoal / g_tipJarRollupTime * context.elapsed);
	if (m_tipJarRolling && m_tipJarValue > m_stats->m_cash)
	{
		m_tipJarValue -= step;
		if (m_tipJarValue <= m_stats->m_cash)
		{
			m_tipJarValue = m_stats->m_cash;
			m_lastTips = m_stats->m_cash;
			m_tipJarRolling = false;
			if (m_rollupSound)
				m_rollupSound->stop();
			m_tipJarCoins->stop();
		}
	}
	else if (m_tipJarRolling)
	{
		m_tipJarValue += step;
		if (m_tipJarValue >= m_stats->m_cash)
		{
			m_tipJarValue = m_stats->m_cash;
			m_lastTips = m_stats->m_cash;
			m_tipJarRolling = false;
			if (m_rollupSound)
				m_rollupSound->stop();
			m_tipJarCoins->stop();
		}
	}
}

// 0x4387F0
void HudScreen::stopTipJarRollup()
{
	if (m_rollupSound && m_rollupSound->isPlaying())
		m_rollupSound->stop();
	m_tipJarCoins->stop();
}

// 0x4388A0
void HudScreen::updateCoupons()
{
	for (std::list<engine::RefPtr<CouponFrame> >::iterator it = m_coupons.begin(); it != m_coupons.end(); ++it)
		(*it)->updateAvailability(m_stats->m_cash);
}

// 0x4388D0
void HudScreen::slideOutCoupons()
{
	for (std::list<engine::RefPtr<CouponFrame> >::iterator it = m_coupons.begin(); it != m_coupons.end(); ++it)
	{
		CouponFrame* coupon = *it;
		coupon->moveTo(engine::Vector2(coupon->getPosition().x - 800.0f, 47.0f), 0.4f);
	}
}

// 0x438970
void HudScreen::removeCoupon(CouponFrame* coupon)
{
	std::list<engine::RefPtr<CouponFrame> >::iterator it = std::find(m_coupons.begin(), m_coupons.end(), coupon);
	if (it != m_coupons.end())
		m_coupons.erase(it);
	removeChild(coupon);

	int i = 0;
	for (it = m_coupons.begin(); it != m_coupons.end(); ++it)
	{
		(*it)->moveTo(engine::Vector2(650.0f - i * 72, 47.0f), 0.3f);
		++i;
	}
}

// 0x438A40
void HudScreen::clearCoupons()
{
	for (std::list<engine::RefPtr<CouponFrame> >::iterator it = m_coupons.begin(); it != m_coupons.end(); ++it)
		removeChild(*it);
	m_coupons.clear();
}

// 0x438AC0
HudScreen::HudScreen()
{
}

// 0x438C80
HudScreen::~HudScreen()
{
	clearCoupons();
	m_progressStack = NULL;
}

// 0x471110 (folded)
void HudScreen::draw(engine::Graphics& g)
{
	engine::Container::draw(g);
}

// 0x438FD0
void HudScreen::tick(engine::UpdateContext& context)
{
	if (m_tipJarRolling)
	{
		updateTipJarRollup(context);
		updateTipJar();
	}
	else if (m_stats->m_cash != m_lastTips)
	{
		startTipJarRollup(m_lastTips);
		m_lastTips = m_stats->m_cash;
	}

	if (m_stats->m_ordersLeft != m_lastOrderCount)
	{
		updateOrderCount();
		m_lastOrderCount = m_stats->m_ordersLeft;
	}

	if (m_stats->m_satisfaction != m_lastSatisfaction)
	{
		m_starsFrom = m_lastSatisfaction;
		m_starBlend = 1.0f;
		m_lastSatisfaction = m_stats->m_satisfaction;
		if (m_lastSatisfaction > m_starsFrom)
			PizzaFrenzy::getSounds()->playSound("satisfaction_up", 1.0f, 1.0f);
		else
			PizzaFrenzy::getSounds()->playSound("satisfaction_down", 1.0f, 1.0f);
	}

	if (PizzaFrenzy::getGameLogic()->getSelectedCount() != m_lastPriceMultiplier)
	{
		updateCoupons();
		m_lastPriceMultiplier = PizzaFrenzy::getGameLogic()->getSelectedCount();
	}

	if (m_starBlend > 0.0f)
	{
		m_starBlend -= context.elapsed;
		if (m_starBlend < 0.0f)
			m_starBlend = 0.0f;
		updateStars();
	}

	if (m_scoreBkgTimer > 0.0f)
	{
		m_scoreBkgTimer -= context.elapsed;
		if (m_scoreBkgTimer < 0.0f)
		{
			m_scoreBkgTimer = 0.0f;
			m_scoreBkg->removeAllAnimators();
			m_scoreBkg->setVisible(false);
		}
	}

	// the original advances the iterator again after erasing, skipping the coupon after a removed one
	for (std::list<engine::RefPtr<CouponFrame> >::iterator it = m_coupons.begin(); it != m_coupons.end(); ++it)
	{
		CouponFrame* coupon = *it;
		coupon->updateActions(context);
		if (coupon->isLeaving() && coupon->getPosition().x < -100.0f)
		{
			removeChild(coupon);
			it = m_coupons.erase(it);
		}
	}

	engine::Screen::update(context);
}

// 0x4392E0
void HudScreen::reset()
{
	m_starWarnings->removeAllAnimators();
	m_starWarnings->setVisible(false);
	m_unknown298 = 0;
	m_tipJarRolling = false;
	m_lastTips = 0;
	m_starBlend = 0.0f;
	m_scoreBkgTimer = 0.0f;
	m_lastPriceMultiplier = 0;
	m_lastSatisfaction = m_stats->m_satisfaction;
	updateTipJar();
	updateOrderCount();
	updateStars();
	m_tipJarMax->setNumber(m_stats->m_cashGoal, "$");
	clearCoupons();
}

// 0x4393F0: fills the tip jar to the shown cash's share of the level's cash target
void HudScreen::updateTipJar()
{
	int tips;
	if (m_tipJarRolling)
		tips = m_tipJarValue;
	else
		tips = m_stats->m_cash;
	float fill = (std::max)(0.0f, (std::min)((float)tips / m_stats->m_cashGoal, 1.0f));

	float y = m_tipJarBounds.getHeight() - m_tipJarContents->getImageHeight() * fill;
	float srcTop = (std::max)(-y, 0.0f);
	float srcBottom = (std::min)(m_tipJarBounds.getHeight(), m_tipJarContents->getImageHeight() * fill) + srcTop;
	y = y > 0.0f ? y : 0.0f;
	engine::IntRect source(0, (int)srcTop, m_tipJarContents->getImageWidth(), (int)srcBottom);
	m_tipJarContents->setSourceRect(source);
	m_tipJarContents->setPosition(3.0f, y);
	if (m_tipJarRolling)
		m_tipJarCoins->setPosition(m_tipJarBounds.getWidth() * 0.5f, y);

	m_tipJarCount->setNumber(tips, "$");
	updateCoupons();
}

// 0x439650: one order slip per 15 orders left (rounded), stacked 2 pixels apart
void HudScreen::updateOrderCount()
{
	int ordersLeft = m_stats->m_ordersLeft;
	m_progressStack->removeAllChildren();
	int slips = (ordersLeft + 18) / 15;
	if (ordersLeft <= 0)
	{
		m_progressText->setVisible(false);
		slips = 0;
	}
	else
	{
		m_orderCount->setNumber(ordersLeft, "");
		m_progressText->setVisible(true);
	}

	for (int i = slips - 1; i >= 0; --i)
	{
		engine::Image* slip = new engine::Image(engine::getApplication()->getImage("res\\hud\\orderSlip.jpg"));
		slip->setPosition((float)(i * 2), (float)(i * 2));
		m_progressStack->addChild(slip);
	}
}

// 0x4397D0
void HudScreen::checkHappinessBonus()
{
	if (m_stats->m_satisfaction >= 100)
	{
		float x = m_starsBounds.left;
		float y = m_starsBounds.bottom - m_starsBounds.getHeight() * 0.1f * 9.0f;
		engine::Splat* splat = PizzaFrenzy::getSplatFactory()->createSplat("happinessBonus", x, y, NULL, NULL);
		PizzaFrenzy::getSounds()->playSound("satisfaction_full", 1.0f, 1.0f);
		splat->setBlendMode(1);
		getGame()->getGameLogic()->addMoney(g_happinessBonus);
	}
}

// 0x4399D0: slides the coupon in from the left to the first free place after the coupons not leaving
void HudScreen::addCoupon(CouponFrame* coupon)
{
	int count = 0;
	for (std::list<engine::RefPtr<CouponFrame> >::iterator it = m_coupons.begin(); it != m_coupons.end(); ++it)
	{
		if (!(*it)->isLeaving())
			++count;
	}
	float x = 650.0f - count * 72;
	coupon->setPosition(x - 700.0f, 47.0f);
	coupon->moveTo(engine::Vector2(x, 47.0f), 0.5f);
	addChild(coupon);
	m_coupons.push_back(coupon);
}

// 0x439B00
void HudScreen::createCoupon(Topping* topping, int price)
{
	CouponFrame* coupon = new CouponFrame();
	if (!topping)
		topping = PizzaFrenzy::getCityMap()->getRandomKitchenPizza();
	coupon->init(topping, price);
	coupon->updateAvailability(m_stats->m_cash);
	addCoupon(coupon);
}

// 0x439BA0
void HudScreen::init(GameProgress* stats)
{
	load("res\\screenLayouts\\hudScreen.xml");
	m_stats = stats;
	m_orderCount = (engine::TextItem*)getComponent("orderCount");
	m_progressText = getComponent("progressText");
	m_tipJarCount = (engine::TextItem*)getComponent("tipJarCount");
	m_tipJarMax = (engine::TextItem*)getComponent("tipJarMax");
	m_scoreBkg = getComponent("scoreBkg");
	m_scoreBkg->setVisible(false);
	m_tipJarCountPos = m_tipJarCount->getPosition();
	m_tipJarContents = (engine::Image*)getComponent("tipJarContents");
	m_tipJarContents->setUseSourceRect(true);

	engine::Container* tipJar = (engine::Container*)getComponent("tipJar");
	m_tipJarBounds = tipJar->getBounds();
	m_tipJarCoins = new engine::ParticleSystem();
	m_tipJarCoins->load(PizzaFrenzy::getTileManifest()->getFx("TipJarCoins")->m_effect);
	tipJar->addChild(m_tipJarCoins);
	m_rollupSound = PizzaFrenzy::getSounds()->getSound("tip_jarRollup");

	m_progressStack = (engine::Container*)getComponent("progressStack");

	engine::Component* stars = getComponent("stars");
	m_starsBounds = stars->getBounds();
	m_starWarnings = new engine::Container();
	m_starWarnings->setVisible(false);
	addChild(m_starWarnings);
	engine::Bitmap* starImage = engine::getApplication()->getImage("res\\hud\\star-full.jpg");
	starImage->setPivotType(1);
	engine::Bitmap* warningImage = engine::getApplication()->getImage("res\\hud\\star-warning.jpg");
	warningImage->setPivotType(1);
	for (int i = 0; i < 5; ++i)
	{
		float x = m_starsBounds.left;
		float y = m_starsBounds.bottom - m_starsBounds.getHeight() * 0.1f * (2 * i + 1);
		m_stars[i] = new engine::Image(starImage);
		m_stars[i]->setPosition(x, y);
		m_stars[i]->setUseSourceRect(true);
		addChild(m_stars[i]);
		engine::Image* warning = new engine::Image(warningImage);
		warning->setPosition(x, y);
		m_starWarnings->addChild(warning);
	}
	addChild(m_starWarnings);

	engine::ParticleSystemDef* burst = PizzaFrenzy::getTileManifest()->getFx("BurstFx")->m_effect;
	m_burstFx = new engine::ParticleSystem();
	m_burstFx->load(burst);
	m_burstFx->setPosition((m_starsBounds.right + m_starsBounds.left) * 0.5f, m_starsBounds.top);
	addChild(m_burstFx);
}

// 0x43A5B0
void HudScreen::addCityCoupons()
{
	std::vector<engine::RefPtr<Topping> >& pizzas = PizzaFrenzy::getCityMap()->getKitchenPizzas();
	for (std::vector<engine::RefPtr<Topping> >::iterator it = pizzas.begin(); it != pizzas.end(); ++it)
		createCoupon(*it, 200);
}
