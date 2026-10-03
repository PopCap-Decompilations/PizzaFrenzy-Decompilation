#include "CouponFrame.h"

#include <algorithm>
#include <string>

#include "engine/Application.h"
#include "engine/Component.h"
#include "engine/Image.h"
#include "engine/KeyframeCurve.h"
#include "engine/Rect.h"
#include "engine/Selector.h"
#include "engine/SoundMgr.h"
#include "engine/Surface.h"
#include "engine/TextItem.h"

#include "GameLogic.h"
#include "GameProgress.h"
#include "HudScreen.h"
#include "PizzaFrenzy.h"
#include "TileManager.h"

// 0x44EAA0
void CouponFrame::updateActions(engine::UpdateContext& context)
{
	m_actions.update(context);
}

// 0x44EAB0
Topping* CouponFrame::getTopping() const
{
	return m_topping;
}

// 0x44EAC0
void CouponFrame::updateAvailability(int cash)
{
	int price = m_price;
	m_affordable = cash >= (std::max)(price, price * PizzaFrenzy::getGameLogic()->getSelectedCount());
	if (!m_affordable)
	{
		setColorMode(1);
		setColor(-0.3f, -0.3f, -0.3f);
	}
	else
		setColorMode(0);
}

// 0x44EB20
void CouponMoveAction::enter()
{
	engine::Vector2 start = m_owner->getPosition();
	engine::Keyframe<engine::Vector2> first;
	first.time = 0.0f;
	first.value = start;
	first.inTime = 0.0f;
	first.inWeight = 1.0f;
	first.outTime = 1.0f;
	first.outWeight = 1.0f;
	m_path.addKey(first);

	engine::Vector2 end = m_dest;
	engine::Keyframe<engine::Vector2> last;
	last.time = 1.0f;
	last.value = end;
	last.inTime = 0.2f;
	last.inWeight = 1.0f;
	last.outTime = 0.0f;
	last.outWeight = 1.0f;
	m_path.addKey(last);

	m_timer.start(m_duration, 1.0f);
}

// 0x44EC10
void CouponMoveAction::update(engine::UpdateContext& context)
{
	m_timer.update(context.elapsed);
	m_path.animate(m_timer.getValue(), m_owner);
	if (m_timer.isFinished())
		m_owner->updateAvailability(PizzaFrenzy::getGameStats()->m_cash);
}

// 0x44EC70
void CouponFrame::onMouseLeave()
{
	m_captionBox->setVisible(false);
	m_frame->select(0, 0.0f, true);
}

// 0x44ECA0
void CouponFrame::updateBounds()
{
	m_bounds = m_rect;
	m_frame->updateBounds();
	m_bounds.unite(m_frame->getBounds());
	m_bounds.scale(m_scale.x, m_scale.y);
	m_bounds.normalize();
	m_bounds.offset(m_position.x, m_position.y);
	removeTreeFlags(8);
}

// 0x44ED20
void CouponFrame::addToHud()
{
	PizzaFrenzy::getHud()->addCoupon(this);
	m_frame->select(0, 0.0f, true);
}

// 0x44ED50
void CouponFrame::removeFromHud()
{
	PizzaFrenzy::getHud()->removeCoupon(this);
	m_captionBox->setVisible(false);
	m_frame->select(0, 0.0f, true);
}

// 0x44ED90
CouponMoveAction::CouponMoveAction(CouponFrame* coupon, const engine::Vector2& dest, float duration)
	: engine::State<CouponFrame>(coupon)
{
	m_dest = dest;
	m_duration = duration;
}

// 0x44EF50
CouponFrame::~CouponFrame()
{
	removeAllChildren();
}

// 0x44F080
int CouponFrame::getPrice() const
{
	return m_price;
}

// 0x44F090
bool CouponFrame::isLeaving() const
{
	return m_leaving;
}

// 0x44F0D0
void CouponFrame::moveTo(const engine::Vector2& dest, float time)
{
	if (dest.x < m_position.x)
		m_leaving = true;
	m_actions.setState(new CouponMoveAction(this, dest, time));
}

// 0x44F180
void CouponFrame::setPrice(int price)
{
	m_price = price;
	m_priceText->setNumber(m_price, "$");
}

// 0x44F210
void CouponFrame::onMouseDown()
{
	if (m_affordable)
	{
		redeem();
		PizzaFrenzy::getSounds()->playSound("order_click1", 1.0f, 1.0f);
	}
	else
	{
		PizzaFrenzy::getSounds()->playSound("order_invalidClick", 1.0f, 1.0f);
		PizzaFrenzy::getHud()->showScoreBackground();
	}
}

// 0x44F330
void CouponFrame::onMouseEnter()
{
	if (m_affordable)
	{
		m_captionBox->setVisible(true);
		m_frame->select(1, 0.0f, true);
		PizzaFrenzy::getSounds()->playSound("order_mouseover1", 1.0f, 1.0f);
	}
}

// 0x44F3F0
void CouponFrame::redeem()
{
	GameProgress* stats = PizzaFrenzy::getGameStats();
	int count = PizzaFrenzy::getGameLogic()->getSelectedCount();
	int price = m_price;
	if (count > 0)
		price *= count;
	if (stats->m_cash >= price)
	{
		addRef();
		removeFromHud();
		PizzaFrenzy::getGameLogic()->useCoupon(this);
		release();
	}
	else
	{
		PizzaFrenzy::getHud()->showScoreBackground();
		PizzaFrenzy::getSounds()->playSound("order_invalidClick", 1.0f, 1.0f);
	}
}

// 0x44F500
void CouponFrame::init(Topping* topping, int price)
{
	engine::Image* coupon = new engine::Image(topping->m_couponImage);
	coupon->setPosition(0.0f, -2.0f);
	addChild(coupon);

	engine::Image* icon = new engine::Image(topping->m_smallImage);
	icon->setPosition(-3.0f, -10.0f);
	addChild(icon);

	m_frame = new engine::Selector();
	addChild(m_frame);
	engine::Bitmap* frame = engine::getApplication()->getImage("res\\hud\\couponFrame.jpg");
	frame->setPivotType(1);
	engine::Bitmap* frameOn = engine::getApplication()->getImage("res\\hud\\couponFrame_on.jpg");
	frameOn->setPivotType(1);
	m_frame->addChild(new engine::Image(frame));
	m_frame->addChild(new engine::Image(frameOn));
	m_frame->select(0, 0.0f, true);

	PizzaFrenzy::getGameStats()->getToppingUpgrade(topping);
	m_caption = engine::getApplication()->formatString(230, topping->m_display.c_str());

	m_price = price;
	if (price > 0)
	{
		m_priceText = new engine::TextItem();
		m_priceText->setFont(engine::getApplication()->getFont("res\\fonts\\hudFont.xml"));
		m_priceText->setXAlign(2);
		m_priceText->setYAlign(1);
		m_priceText->setColorMode(2);
		m_priceText->setColor(0.0f, 0.0f, 0.0f);
		m_priceText->setPosition(23.0f, 17.0f);
		addChild(m_priceText);
	}
	setPrice(price);

	m_captionBox = new engine::Container();
	m_captionBox->setPosition(0.0f, 48.0f);
	addChild(m_captionBox);

	m_captionText = new engine::TextItem();
	m_captionText->setFont(engine::getApplication()->getFont("res\\fonts\\bookFont.xml"));
	m_captionText->setColorMode(2);
	m_captionText->setColor(0.0f, 0.0f, 0.0f);
	m_captionText->setXAlign(1);
	m_captionText->setYAlign(1);
	m_captionText->setText(m_caption);

	engine::Bitmap* caption = engine::getApplication()->getImage("res\\menuAssets\\toppingUpgradeCaption.jpg");
	caption->setPivotType(1);
	m_captionBox->addChild(new engine::Image(caption));
	m_captionBox->addChild(m_captionText);
	m_captionBox->setVisible(false);

	m_topping = topping;
	m_leaving = false;
}

// 0x44FA80
CouponFrame::CouponFrame()
{
}
