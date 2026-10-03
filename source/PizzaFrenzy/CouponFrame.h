// CouponFrame, the HUD coupon for a topping upgrade, and CouponMoveAction, which slides it.
#pragma once

#include <string>

#include "engine/Container.h"
#include "engine/FadeTimer.h"
#include "engine/Point.h"
#include "engine/PropertyTrack.h"
#include "engine/RefPtr.h"
#include "engine/State.h"
#include "engine/StateMachine.h"

namespace engine
{
	class Selector;
	class TextItem;
}

class Topping;

// Coupon for a topping upgrade on the HUD (res\hud\couponFrame.jpg, couponFrame_on.jpg while hovered): the topping's
// images, the "$" price, and a caption shown on hover (toppingUpgradeCaption.jpg with message 230). A click redeems it
// if the player's cash covers the price times the level multiplier. Created by HudScreen::createCoupon; keeps
// engine::Container's type name. The members end at +0x178; the vtordisp (+0x178) and the Interface subobject
// (+0x17C) follow them (0x180 bytes).
class CouponFrame : public engine::Container
{
public:
	CouponFrame();
	virtual ~CouponFrame();

	virtual void init(Topping* topping, int price);								// slot 75
	virtual void updateActions(engine::UpdateContext& context);					// slot 76: runs m_actions
	virtual void redeem();														// slot 77
	virtual void addToHud();													// slot 78
	virtual void removeFromHud();												// slot 79
	virtual void moveTo(const engine::Vector2& dest, float time);				// slot 80
	virtual Topping* getTopping() const;										// slot 81
	virtual int getPrice() const;												// slot 82
	virtual void setPrice(int price);											// slot 83
	virtual bool isLeaving() const;												// slot 84

	virtual void updateBounds();												// slot 38 (engine::Component)
	virtual void onMouseDown();													// slot 42 (engine::Component)
	virtual void onMouseEnter();												// slot 56 (engine::Component)
	virtual void onMouseLeave();												// slot 59 (engine::Component)

	void updateAvailability(int cash);

	engine::RefPtr<engine::Selector> m_frame;									// +0x128 couponFrame.jpg / _on.jpg
	engine::RefPtr<engine::TextItem> m_priceText;								// +0x12C hudFont at (23,17) if price > 0
	engine::RefPtr<engine::TextItem> m_captionText;								// +0x130 bookFont, shows m_caption
	engine::RefPtr<engine::Container> m_captionBox;								// +0x134 at (0,48), visible on hover
	int m_price;																// +0x138 base price
	engine::StateMachine m_actions;												// +0x13C runs CouponMoveAction
	std::string m_caption;														// +0x154 message 230 with the topping name
	bool m_affordable;															// +0x170 set by updateAvailability
	bool m_leaving;																// +0x171 moving left, off the HUD
	Topping* m_topping;															// +0x174 raw
};

// Slides the coupon to m_dest over m_duration (a two-key position track driven by a timer), then updates its
// availability. engine::State<CouponFrame> (owner at +0x0C), members from +0x10, then the vtordisp (+0x90) and the
// Interface subobject (+0x94); 0x98 bytes, implicit destructor.
class CouponMoveAction : public engine::State<CouponFrame>
{
public:
	CouponMoveAction(CouponFrame* coupon, const engine::Vector2& dest, float duration);

	virtual void enter();														// slot 1 (engine::StateBase)
	virtual void update(engine::UpdateContext& context);						// slot 3 (engine::StateBase)

	engine::PositionTrack m_path;												// +0x10 key 0 at the coupon, key 1 at m_dest
	engine::FadeTimer m_timer;													// +0x60
	engine::Vector2 m_dest;														// +0x84
	float m_duration;															// +0x8C
};
