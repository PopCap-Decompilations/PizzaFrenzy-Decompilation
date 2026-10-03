// The special-customer popups (SpecialEventPopup subclasses) and SpecialEventPopupFactory, which creates them.
#pragma once

#include <string>
#include <vector>

#include "engine/Object.h"
#include "engine/RefPtr.h"
#include "SpecialEventPopup.h"

namespace engine
{
	class Component;
	class Container;
	class Image;
	class SoundHandle;
}

class CustomerTile;
class Tip;
class Topping;

// SpecialEventPopup's members end at +0x188. The popups that add one member have the vtordisp at +0x18C and the
// Interface subobject at +0x190 (0x194 bytes); those that add none are 0x190 bytes, the two that add three 0x19C.
// The six popups without a destructor of their own share one implicit destructor (0x44A2D0) and deleting
// destructor (0x44A2B0) after /OPT:ICF.

// factory type "Thief": "I'll take that money, thanks!", then steals $100 bills one by one from the HUD cash counter
class ThiefEventPopup : public SpecialEventPopup
{
public:
	// no out-of-line copy: inlined in SpecialEventPopupFactory::create (0x44A490)
	ThiefEventPopup()
	{
	}

	virtual std::string getTypeName() const;									// slot 24 (engine::Component)
	virtual void init(engine::Component* deliverer, CustomerTile* customer);	// slot 75 (SpecialEventPopup)
	virtual void onTimeout();													// slot 79 (SpecialEventPopup)
	virtual void speak();														// slot 80 (SpecialEventPopup)

	int m_billsLeft;															// +0x188 set to g_thiefBillCount by init
};

// factory type "Gossip": "Sweetie, you must try the %s pizza!!", then switches every waiting customer's open order
// to her own pizza
class ThiefGossipPopup : public SpecialEventPopup
{
public:
	ThiefGossipPopup();

	virtual std::string getTypeName() const;									// slot 24 (engine::Component)
	virtual void show();														// slot 76 (SpecialEventPopup)
	virtual void onTimeout();													// slot 79 (SpecialEventPopup)
	virtual void speak();														// slot 80 (SpecialEventPopup)

	Topping* m_pizza;															// +0x188 her order's topping (set by show); 0 once pushed onto the others
};

// factory type "MovieStar": "This pizza looks as fabulous as me! Here's a huge tip.", then flies 5 bills from his
// portrait to the HUD cash counter, paying $100 as each one lands
class MovieStarEventPopup : public SpecialEventPopup
{
public:
	MovieStarEventPopup();

	virtual void onBillArrived(int eventArg);									// slot 82: a bill splat's m_onFinished

	virtual std::string getTypeName() const;									// slot 24 (engine::Component)
	virtual void init(engine::Component* deliverer, CustomerTile* customer);	// slot 75 (SpecialEventPopup)
	virtual void onTimeout();													// slot 79 (SpecialEventPopup)
	virtual void speak();														// slot 80 (SpecialEventPopup)

	int m_billsLeft;															// +0x188 set to g_thiefBillCount / 2 by init
};

// factory type "CrankCaller": "Ha-hah! Fooled you!" and the delivery combo is reset; adds no members
class CrankCallerEventPopup : public SpecialEventPopup
{
public:
	CrankCallerEventPopup();

	virtual std::string getTypeName() const;									// slot 24 (engine::Component)
	virtual void onTimeout();													// slot 79 (SpecialEventPopup)
	virtual void speak();														// slot 80 (SpecialEventPopup)
};

// factory type "Monk": "Do not rush! Be at peace!" and the game slows down; adds no members
class MonkEventPopup : public SpecialEventPopup
{
public:
	MonkEventPopup();

	virtual std::string getTypeName() const;									// slot 24 (engine::Component)
	virtual void onTimeout();													// slot 79 (SpecialEventPopup)
	virtual void speak();														// slot 80 (SpecialEventPopup)
};

// factory type "Clown": "Everybody switch!", then moves every waiting customer's open order to the next pizza of the
// city's pizza list
class ScramblerPopup : public SpecialEventPopup
{
public:
	ScramblerPopup();

	virtual std::string getTypeName() const;									// slot 24 (engine::Component)
	virtual void show();														// slot 76 (SpecialEventPopup)
	virtual void onTimeout();													// slot 79 (SpecialEventPopup)
	virtual void speak();														// slot 80 (SpecialEventPopup)

	bool m_scrambled;															// +0x188 set by speak (not by the ctor); the next timeout closes
};

// factory type "TreasureHunter": "I can pick up those tips for you.", then his image jumps to a collectable tip on the
// map every 0.5 s and collects it (10 steps)
class TreasureHunterEventPopup : public SpecialEventPopup
{
public:
	TreasureHunterEventPopup();
	virtual ~TreasureHunterEventPopup();

	virtual std::string getTypeName() const;									// slot 24 (engine::Component)
	virtual void onTimeout();													// slot 79 (SpecialEventPopup)
	virtual void speak();														// slot 80 (SpecialEventPopup)

	std::vector<engine::RefPtr<Tip> >* m_tips;									// +0x188 the city map's m_tips (set by speak)
	int m_stepsLeft;															// +0x18C 10 at start (set by speak)
	engine::RefPtr<engine::Image> m_hunterImage;								// +0x190 on the map layer, moved to each tip
};

// factory type "GraffitiArtist": paints res\specialEvents\graffitiArt.jpg left to right over 2 s with the looping
// "special_graffiti" sound, then a "StealSatisfaction" splat lowers the satisfaction by 10. Its destructor is
// implicit (0x44A430 releases m_graffitiImage only, without the vtable stores of a user-declared one): it does not
// stop m_sound.
class GraffitiArtistEventPopup : public SpecialEventPopup
{
public:
	GraffitiArtistEventPopup();

	virtual std::string getTypeName() const;									// slot 24 (engine::Component)
	virtual void onRemovedFrom(engine::Container* parent);						// slot 61 (engine::Component)
	virtual void onTimeout();													// slot 79 (SpecialEventPopup)
	virtual void speak();														// slot 80 (SpecialEventPopup)

	int m_state;																// +0x188 0 sound, 1 painting, 2 splat, 3 close; not set by the ctor
	engine::RefPtr<engine::Image> m_graffitiImage;								// +0x18C revealed by its source rectangle
	engine::SoundHandle* m_sound;												// +0x190 raw, from SoundMgr::getSound; not set by the ctor
};

// data-less object held by the game logic (GameLogic +0x88): creates the popup of a special customer's event type.
// Its vtable is the /OPT:ICF-folded one of every Object subclass whose only virtual is an implicit destructor
// (0x4F9470 = {0x44A0A0}). The vtordisp (+0x0C) and the Interface subobject (+0x10) follow Object (0x14 bytes).
class SpecialEventPopupFactory : public engine::Object
{
public:
	SpecialEventPopupFactory();

	SpecialEventPopup* create(const std::string& type);
};
