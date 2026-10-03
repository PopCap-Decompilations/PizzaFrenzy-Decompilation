// Tip: the cash tip left on the city map after a delivery, and its states (appear, wait, blink, vanish).
#pragma once

#include "engine/Container.h"
#include "engine/RefPtr.h"
#include "engine/sigslot.h"
#include "engine/State.h"
#include "engine/StateMachine.h"

namespace engine
{
	class Selector;
}

class CityMap;
class Topping;

// Made by GameLogic::placeTip for cash tips (res\tips\cash-%d.jpg on the tip background); base of BalloonTip and
// CitizenAward. Clicked, it flies to the HUD cash counter and pays out when that splat finishes (onCashArrived).
// Bases: engine::Container +0x00, sigslot::has_slots<> +0x128 (receives the splat's signal). The members end at
// +0x164; the vtordisp (+0x164) and the Interface subobject (+0x168) follow them (0x16C bytes).
class Tip : public engine::Container, public sigslot::has_slots<>
{
public:
	Tip();
	virtual ~Tip();

	virtual void init(float size, Topping* topping);			// slot 75: size = stars 1-3, topping = the order's
	virtual void createGraphics();								// slot 76
	virtual void onAppeared();									// slot 77: called by TipAppearState
	virtual void collect();										// slot 78
	virtual void onCashArrived(int eventArg);					// slot 79: the splat's finished signal (argument unused)

	virtual void onMouseDown();									// slot 42 (engine::Component)
	virtual void onMouseEnter();								// slot 56 (engine::Component)
	virtual void onMouseLeave();								// slot 59 (engine::Component)
	virtual void onRemovedFrom(engine::Container* parent);		// slot 61 (engine::Component)

	void placeOnMap(CityMap* map, const engine::Vector2& pos);
	void updateOnMap(engine::UpdateContext& ctx);

	float m_size;												// +0x138 tip level 1-3; picks res\tips\cash-%d.jpg
	int m_amount;												// +0x13C cash paid when collected
	engine::RefPtr<engine::Selector> m_background;				// +0x140 0 normal, 1 mouse over (BalloonTip: 2 closed)
	engine::StateMachine m_states;								// +0x144 appear, wait, blink, vanish (0x18 bytes)
	CityMap* m_map;												// +0x15C map the tip is registered with (placeOnMap)
	bool m_clickable;											// +0x160 mouse down and enter react only when set
};

// The tip states: engine::State<Tip> (owner at +0x0C), own members from +0x10, then the vtordisp and the Interface
// subobject. No user-declared destructors.

// Pop-in: fades in and grows from 0.5 to 1 over 0.2 s, then Tip::onAppeared (0x1C bytes).
class TipAppearState : public engine::State<Tip>
{
public:
	TipAppearState(Tip* tip);

	virtual void enter();										// slot 1 (engine::StateBase)
	virtual void update(engine::UpdateContext& context);		// slot 3 (engine::StateBase)

	float m_time;												// +0x10 remaining pop-in time (0.2 s)
};

// Waits 5 s, then blinks if the tip is still clickable (0x1C bytes).
class TipWaitState : public engine::State<Tip>
{
public:
	TipWaitState(Tip* tip);

	virtual void enter();										// slot 1 (engine::StateBase)
	virtual void update(engine::UpdateContext& context);		// slot 3 (engine::StateBase)

	float m_time;												// +0x10 remaining wait (5 s)
};

// Blinks (engine::AlphaPulse animator) for 5 s, then vanishes (0x1C bytes).
class TipBlinkState : public engine::State<Tip>
{
public:
	TipBlinkState(Tip* tip);

	virtual void enter();										// slot 1 (engine::StateBase)
	virtual void exit();										// slot 2 (engine::StateBase)
	virtual void update(engine::UpdateContext& context);		// slot 3 (engine::StateBase)

	float m_time;												// +0x10 remaining blink time (5 s)
};

// Tip lost: "tip_lost", 0.2 s shrink and fade, then GameLogic::removeTip (0x1C bytes).
class TipVanishState : public engine::State<Tip>
{
public:
	TipVanishState(Tip* tip);

	virtual void enter();										// slot 1 (engine::StateBase)
	virtual void update(engine::UpdateContext& context);		// slot 3 (engine::StateBase)

	float m_time;												// +0x10 remaining vanish time (0.2 s)
};
