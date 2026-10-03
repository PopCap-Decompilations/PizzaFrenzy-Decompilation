// BalloonTip: the tip the prize blimp drops, and its states (drop, float).
#pragma once

#include "engine/Point.h"
#include "engine/State.h"
#include "Tip.h"

class Topping;

// Dropped closed by Blimp::dropTips, opens into a balloon that drifts down (res\tips\balloonTip*.jpg); worth
// (int)(g_defaultTipValue * size). Adds no members (0x16C bytes, Tip's layout).
class BalloonTip : public Tip
{
public:
	BalloonTip();
	virtual ~BalloonTip();

	virtual void updateBounds();								// slot 38 (engine::Component): the shadow is not clickable
	virtual void init(float size, Topping* topping);			// slot 75 (Tip)
	virtual void createGraphics();								// slot 76 (Tip)
	virtual void onAppeared();									// slot 77 (Tip)
};

// The balloon states: engine::State<BalloonTip> (owner at +0x0C), own members from +0x10, then the vtordisp and the
// Interface subobject. No user-declared destructors.

// Closed balloon tossed from the blimp: random sideways speed, gravity 700 px/s^2, opens after 0.3-0.6 s (0x24 bytes).
class BalloonTipDropState : public engine::State<BalloonTip>
{
public:
	BalloonTipDropState(BalloonTip* tip);

	virtual void enter();										// slot 1 (engine::StateBase)
	virtual void update(engine::UpdateContext& context);		// slot 3 (engine::StateBase)

	engine::Vector2 m_velocity;									// +0x10 (random -100..100, 50), gravity added
	float m_time;												// +0x18 time until the balloon opens
};

// Open balloon drifting down at a constant speed (clickable); removed when it leaves the screen (0x20 bytes).
class BalloonTipFloatState : public engine::State<BalloonTip>
{
public:
	BalloonTipFloatState(BalloonTip* tip);

	virtual void enter();										// slot 1 (engine::StateBase)
	virtual void update(engine::UpdateContext& context);		// slot 3 (engine::StateBase)

	engine::Vector2 m_velocity;									// +0x10 (random -75..75, random 50..75)
};
