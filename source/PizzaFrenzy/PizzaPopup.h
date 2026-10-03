// PizzaPopup (the popups over the map buildings), GameStateMachine, and the states PopupShowState and PopupCloseState.
#pragma once

#include <string>

#include "engine/Container.h"
#include "engine/RefPtr.h"
#include "engine/StateMachine.h"

namespace engine
{
	class Image;
	class StateBase;
}

class BuildingTile;

// The game-side state machine: setState (slot 1) is disabled and returns false, states change through
// switchState (slot 5), which exits the old state and enters the new one. Held by value in PizzaPopup (+0x134) and
// MusicPlayer (+0x1C). Adds no members (0x18 bytes like engine::StateMachine); its constructor and destructor are
// implicit (always inlined; the destructor 0x4505D0 is compiler-generated).
class GameStateMachine : public engine::StateMachine
{
public:
	// engine::StateMachine
	virtual bool setState(engine::StateBase* state);						// slot 1 (0x454E60, folded: return false)

	virtual bool switchState(engine::StateBase* state);						// slot 5
	virtual engine::StateBase* getState() const;							// slot 6 (0x411960, folded: return m_state)
};

// Base of the popups floating over the map buildings: the order bubble (PizzaOrderPopup), the kitchen's topping
// button (PizzaKitchenPopup) and the police button (PolicePopup). It runs a PizzaPopupState machine (m_state) for
// the show/close/patience animations and gates mouse input with it; a left click is forwarded to the owning
// building. 0x154 bytes: Container up to +0x128, the members below, then the vtordisp (+0x14C) and the Interface
// (+0x150).
class PizzaPopup : public engine::Container
{
public:
	PizzaPopup();
	virtual ~PizzaPopup();

	// engine::Component
	virtual std::string getTypeName() const;								// slot 24: "PizzaPopup"
	virtual void setEnabled(bool enabled);									// slot 32: m_enabled only (the Component flag is untouched)
	virtual void updateBounds();											// slot 38
	virtual void onMouseDown();												// slot 42
	virtual void onMouseEnter();											// slot 56
	virtual void onMouseLeave();											// slot 59
	virtual void onRemovedFrom(engine::Container* parent);					// slot 61

	virtual void init(BuildingTile* owner);									// slot 75
	virtual bool acceptsInput() const;										// slot 76
	virtual void showSelection(bool show);									// slot 77
	virtual int getPopupType() const = 0;									// slot 78: 0 order, 1 kitchen, 2 police
	virtual void onShowFinished() = 0;										// slot 79: PopupShowState is done
	virtual void onCloseFinished() = 0;										// slot 80: PopupCloseState is done
	virtual void highlight();												// slot 81
	virtual void select();													// slot 82
	virtual void onDispatched();											// slot 83 (0x454D50, folded with deselect)
	virtual void deselect();												// slot 84 (0x454D50)
	virtual bool isActive() const;											// slot 85
	virtual bool isClosing() const;											// slot 86
	virtual void remove();													// slot 87 (0x42F770, folded: setFlags(0x10))
	virtual void close();													// slot 88
	virtual void updateState(engine::UpdateContext& ctx);					// slot 89

	BuildingTile* m_owner;													// +0x128 owning map building (raw pointer): gets the left clicks and slot 100 on removal
	engine::RefPtr<engine::Image> m_selectionBox;							// +0x12C res\pizza\selection-box.jpg, hidden until highlighted
	bool m_closing;															// +0x130 set by PopupCloseState and OrderHangUpState
	bool m_enabled;															// +0x131 set by init and setEnabled (not initialised by the constructor); +0x132 padding
	GameStateMachine m_state;												// +0x134 the popup's current PizzaPopupState
};

// PizzaPopupState, the base of the states below, is declared in PizzaKitchenPopup.h, which needs PizzaPopup as the
// base of PizzaKitchenPopup: it is included here, after PizzaPopup (it declares PizzaPopupState before including
// this file, so either header can be included first).
#include "PizzaKitchenPopup.h"

// Grow-in of a popup: 0.2 s from alpha 0 and scale 0.5 to 1, then popup->onShowFinished(). Input is allowed per
// m_acceptsInput. 0x20 bytes: PizzaPopupState, the members below, vtordisp +0x18, Interface +0x1C. Its constructor
// is always inlined (no address).
class PopupShowState : public PizzaPopupState
{
public:
	PopupShowState(PizzaPopup* popup, bool acceptsInput)
		: PizzaPopupState(popup), m_acceptsInput(acceptsInput)
	{
	}

	// engine::StateBase
	virtual void enter();													// slot 1
	virtual void update(engine::UpdateContext& context);					// slot 3

	// PizzaPopupState
	virtual bool acceptsInput() const;										// slot 5

	float m_time;															// +0x10 0.2 s countdown (set by enter)
	bool m_acceptsInput;													// +0x14 returned by acceptsInput()
};

// Fade-out of a popup: 0.5 s shrink with m_closing set, then popup->onCloseFinished(). No input, not active.
// 0x1C bytes: PizzaPopupState, m_time, vtordisp +0x14, Interface +0x18. Its constructor is always inlined.
class PopupCloseState : public PizzaPopupState
{
public:
	PopupCloseState(PizzaPopup* popup)
		: PizzaPopupState(popup)
	{
	}

	// engine::StateBase
	virtual void enter();													// slot 1
	virtual void exit();													// slot 2
	virtual void update(engine::UpdateContext& context);					// slot 3

	// PizzaPopupState
	virtual bool acceptsInput() const;										// slot 5 (0x4529D0, folded: return false)
	virtual bool isActive() const;											// slot 6 (0x4529D0, folded: return false)

	float m_time;															// +0x10 0.5 s countdown (set by enter)
};
