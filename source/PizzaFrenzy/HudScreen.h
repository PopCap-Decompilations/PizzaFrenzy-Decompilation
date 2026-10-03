// HudScreen: the in-game HUD (order count and slips, tip jar, satisfaction stars, coupons).
#pragma once

#include <list>
#include <string>

#include "engine/Point.h"
#include "engine/Rect.h"
#include "engine/RefPtr.h"
#include "engine/Screen.h"

namespace engine
{
	class Component;
	class Container;
	class Graphics;
	class Image;
	class ParticleSystem;
	class SoundHandle;
	class TextItem;
}

class CouponFrame;
class GameProgress;
class Topping;

// res\screenLayouts\hudScreen.xml (game+0x118), a child of the GameScreen: order count and stacked order slips, tip
// jar with animated fill/roll-up and coin particles, five satisfaction stars, coupons along the top. The members end
// at +0x2AC; the vtordisp (+0x2AC) and the Interface subobject (+0x2B0) follow them (size 0x2B4).
class HudScreen : public engine::Screen
{
public:
	HudScreen();
	virtual ~HudScreen();

	virtual void init(GameProgress* stats);							// slot 95
	virtual void updateStars();										// slot 96
	virtual void updateTipJar();									// slot 97
	virtual void updateCoupons();									// slot 98
	virtual void updateOrderCount();								// slot 99
	virtual void tick(engine::UpdateContext& context);				// slot 100

	virtual void draw(engine::Graphics& g);							// slot 36 (engine::Component), folded body 0x471110

	void reset();
	void setPaused(bool paused);
	void checkHappinessBonus();
	engine::Vector2 getStarLevelPosition() const;
	void showScoreBackground();
	void showStarWarning(bool show);
	void startTipJarRollup(int fromValue);
	void updateTipJarRollup(engine::UpdateContext& context);
	void stopTipJarRollup();
	std::list<engine::RefPtr<CouponFrame> >& getCoupons();
	void createCoupon(Topping* topping, int price);
	void addCityCoupons();
	void addCoupon(CouponFrame* coupon);
	void removeCoupon(CouponFrame* coupon);
	void slideOutCoupons();
	void clearCoupons();

	engine::RefPtr<engine::TextItem> m_orderCount;						// +0x1D4 "orderCount"
	engine::RefPtr<engine::Component> m_progressText;					// +0x1D8 "progressText": visible while orders remain
	engine::RefPtr<GameProgress> m_stats;								// +0x1DC the game's stats (game+0x120): score, satisfaction,
																		//        orders left, tips, tip jar size
	engine::RefPtr<engine::Image> m_tipJarContents;						// +0x1E0 "tipJarContents": clipped to the fill level
	engine::RefPtr<engine::TextItem> m_tipJarCount;						// +0x1E4 "tipJarCount": "$<tips>"
	engine::RefPtr<engine::TextItem> m_tipJarMax;						// +0x1E8 "tipJarMax": "$<jar size>"
	engine::RefPtr<engine::Component> m_unused1EC;						// +0x1EC never set (released by the dtor)
	engine::RefPtr<engine::Container> m_progressStack;					// +0x1F0 "progressStack": one res\hud\orderSlip.jpg per 15 orders
	engine::RefPtr<engine::Component> m_scoreBkg;						// +0x1F4 "scoreBkg": faded in for 2 s by showScoreBackground
	engine::RefPtr<engine::Component> m_unused1F8;						// +0x1F8 never set
	engine::RefPtr<engine::Container> m_starWarnings;					// +0x1FC star-warning.jpg copies over the stars
	engine::RefPtr<engine::Image> m_stars[5];							// +0x200 res\hud\star-full.jpg images stacked in "stars"
	engine::RefPtr<engine::ParticleSystem> m_burstFx;					// +0x214 "BurstFx" emitter at the top centre of "stars"
	engine::RefPtr<engine::Component> m_unused218;						// +0x218 never set
	engine::RefPtr<engine::Component> m_unused21C;						// +0x21C never set
	std::string m_unused220;											// +0x220 only constructed and destroyed
	engine::RefPtr<engine::Component> m_unused23C;						// +0x23C never set
	float m_scoreBkgTimer;												// +0x240 seconds until scoreBkg is hidden again
	std::list<engine::RefPtr<CouponFrame> > m_coupons;					// +0x244 coupons at x = 650 - 72*i, y = 47
	engine::Rect m_tipJarBounds;										// +0x250 bounds of layout item "tipJar"
	engine::Rect m_starsBounds;											// +0x260 bounds of layout item "stars"
	int m_lastOrderCount;												// +0x270 orders left at the last updateOrderCount
	int m_lastTips;														// +0x274 tips last seen; a change starts the roll-up
	int m_lastSatisfaction;												// +0x278 satisfaction last seen; a change animates the stars
	int m_unknown27C;													// +0x27C never referenced
	int m_lastPriceMultiplier;											// +0x280 last value of GameLogic::getSelectedCount; a change
																		//        calls updateCoupons
	engine::RefPtr<engine::ParticleSystem> m_tipJarCoins;				// +0x284 "TipJarCoins" emitter added to "tipJar"
	bool m_tipJarRolling;												// +0x288 roll-up animation running
	int m_tipJarValue;													// +0x28C value shown while rolling
	int m_unknown290;													// +0x290 never referenced
	engine::SoundHandle* m_rollupSound;									// +0x294 "tip_jarRollup"; raw, never released
	int m_unknown298;													// +0x298 only zeroed by reset
	engine::Vector2 m_tipJarCountPos;									// +0x29C position of "tipJarCount"; read directly by the
																		//        tips (Tip::collect) as the fly-to point
	float m_starBlend;													// +0x2A4 1 -> 0 over a second after a satisfaction change
	int m_starsFrom;													// +0x2A8 satisfaction before the change
};
