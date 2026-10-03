// SpecialEventPopup, the base of the special-customer popups, and its wait and close actions.
#pragma once

#include <string>

#include "engine/Container.h"
#include "engine/RefPtr.h"
#include "engine/State.h"
#include "engine/StateMachine.h"
#include "engine/sigslot.h"

namespace engine
{
	class Bitmap;
	class FadeContainer;
	class Image;
	class TextTyper;
}

class CustomerTile;

// Bottom dialog panel of a special customer (Thief, Gossip, MovieStar, CrankCaller, Monk, Clown, TreasureHunter,
// GraffitiArtist: G08's subclasses): show() flies the portrait from the customer as a "portraitSplat"; when it lands
// (onPortraitArrived, connected to the splat's finished signal) speak() says the character's line and a
// SpecialEventWaitAction counts m_timeLeft down to onTimeout(). Bases: engine::Container +0x00, sigslot::has_slots<>
// +0x128 (its std::set at +0x12C). The members end at +0x188; the vtordisp (+0x188) and the Interface subobject
// (+0x18C) follow them (0x190 bytes).
class SpecialEventPopup : public engine::Container, public sigslot::has_slots<>
{
public:
	SpecialEventPopup();
	virtual ~SpecialEventPopup();

	virtual void init(engine::Component* deliverer, CustomerTile* customer);	// slot 75
	virtual void show();														// slot 76
	virtual void onPortraitArrived(int splatTag);								// slot 77: splat signal (argument unused)
	virtual void updateActions(engine::UpdateContext& context);					// slot 78: runs m_actions
	virtual void onTimeout() = 0;												// slot 79: m_timeLeft ran out
	virtual void speak() = 0;													// slot 80: says the line, sets m_timeLeft
	virtual void say(const std::string& text);									// slot 81

	virtual std::string getTypeName() const;									// slot 24 (engine::Component)

	engine::StateMachine m_actions;												// +0x138 current action (wait, close)
	std::string m_sound;														// +0x150 event type's sound cue (show)
	float m_timeLeft;															// +0x16C set by speak(), not by the ctor
	engine::RefPtr<engine::FadeContainer> m_panel;								// +0x170 the dialog panel
	engine::RefPtr<engine::TextTyper> m_text;									// +0x174 dialogFontBig at (110,525)
	engine::RefPtr<engine::Image> m_portrait;									// +0x178 at (45,530), hidden at first
	engine::RefPtr<engine::Bitmap> m_portraitImage;								// +0x17C portrait of the event type
	engine::RefPtr<engine::Component> m_deliverer;								// +0x180 the kitchen (init argument)
	engine::RefPtr<CustomerTile> m_customer;									// +0x184 the special customer
};

// The popup actions: engine::State<SpecialEventPopup> (owner at +0x0C), then the vtordisp (+0x10) and the Interface
// subobject (+0x14); 0x18 bytes, no members of their own, no user-declared destructors.

// Set on m_actions by onPortraitArrived: m_timeLeft -= elapsed, onTimeout() when it reaches 0.
class SpecialEventWaitAction : public engine::State<SpecialEventPopup>
{
public:
	SpecialEventWaitAction(SpecialEventPopup* popup);

	virtual void update(engine::UpdateContext& context);						// slot 3 (engine::StateBase)
};

// Set on m_actions by the subclasses' onTimeout when the event is over: fades the panel out over 0.3 s, then
// GameLogic::removeEventPopup.
class SpecialEventCloseAction : public engine::State<SpecialEventPopup>
{
public:
	SpecialEventCloseAction(SpecialEventPopup* popup);

	virtual void enter();														// slot 1 (engine::StateBase)
	virtual void update(engine::UpdateContext& context);						// slot 3 (engine::StateBase)
};
