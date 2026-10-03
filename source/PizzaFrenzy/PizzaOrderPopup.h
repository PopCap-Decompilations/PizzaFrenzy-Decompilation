// PizzaOrderPopup (a customer's order bubble) and the states of an order (OrderWaitState ... OrderInactiveState).
#pragma once

#include <string>
#include <vector>

#include "engine/RefPtr.h"
#include "PizzaKitchenPopup.h"
#include "PizzaPopup.h"

namespace engine
{
	class AlphaPulse;
	class ColorPulse;
	class Container;
	class Image;
	class ScalePulse;
	class Selector;
}

class BuildingTile;
class Order;
class Topping;

// A customer's order bubble: stacked pizza icons, the customer's portrait, a front/back flip card, the patience
// countdown, the Scrambler's topping shuffles and Concentration's face-down mode; builds the order cursor. Popup type
// 0; shown by the customer building (its slot 107). 0x1A0 bytes: PizzaPopup up to +0x14C, the members below,
// vtordisp +0x198, Interface +0x19C.
class PizzaOrderPopup : public PizzaPopup
{
public:
	PizzaOrderPopup();
	virtual ~PizzaOrderPopup();

	// engine::Component
	virtual std::string getTypeName() const;								// slot 24: "PizzaOrderPopup"
	virtual void onRightMouseDown();										// slot 48

	// PizzaPopup
	virtual void init(BuildingTile* owner);									// slot 75
	virtual void showSelection(bool show);									// slot 77
	virtual int getPopupType() const;										// slot 78 (0x4D14F0, folded: return 0)
	virtual void onShowFinished();											// slot 79
	virtual void onCloseFinished();											// slot 80 (0x452050, folded: remove())
	virtual void highlight();												// slot 81
	virtual void select();													// slot 82
	virtual void onDispatched();											// slot 83
	virtual void deselect();												// slot 84
	virtual void close();													// slot 88

	virtual void setOrder(Order* order, bool concealed);					// slot 90
	virtual void replaceOrder(Order* order, bool animate);					// slot 91
	virtual void shuffleTo(Order* order, bool silent);						// slot 92
	virtual void buildOrderCursor(engine::Container* cursor);				// slot 93
	virtual void addCapacitySegment(engine::Container* onLayer, engine::Container* flashLayer, float x, float y, int kind);	// slot 94: does not use this
	virtual void setScrambling(bool scrambling);							// slot 95
	virtual void onDelivered();												// slot 96
	virtual void losePatience(bool warnOnly);								// slot 97
	virtual void flashOrder();												// slot 98
	virtual Order* getOrder() const;										// slot 99 (0x452890, folded: return m_order)
	virtual void flipOver();												// slot 100
	virtual void reveal();													// slot 101

	void buildPizzaIcons(engine::Container* container, Order* order);
	void updateScramble(engine::UpdateContext& ctx);
	void playOrderSound();

	engine::RefPtr<Order> m_order;											// +0x14C the order shown
	engine::RefPtr<engine::Container> m_pizzaIcons;							// +0x150 stacked pizza icons (buildPizzaIcons)
	engine::RefPtr<engine::Selector> m_flipSelector;						// +0x154 card side: 0 front, 1 back
	engine::RefPtr<engine::Selector> m_faceSelector;						// +0x158 0 pizzas, 1 the special customer's portrait (on hover)
	engine::RefPtr<engine::Image> m_button;									// +0x15C res/pizza/orderButton.jpg at (-4,2)
	engine::RefPtr<engine::Image> m_portrait;								// +0x160 customer portrait at (-33,-33)
	engine::RefPtr<engine::Image> m_backPortrait;							// +0x164 portrait on the back side
	engine::RefPtr<engine::Image> m_unknownImage;							// +0x168 res\pizza\unknown.jpg, shown when flipped over
	engine::RefPtr<engine::Image> m_specialPortrait;						// +0x16C 0.8-scale portrait inside m_faceSelector
	engine::RefPtr<engine::Container> m_content;							// +0x170 front side, then the icon container inside m_faceSelector
	engine::RefPtr<engine::Container> m_back;								// +0x174 back side
	engine::RefPtr<engine::ScalePulse> m_pulseEffect;						// +0x178 (0.5, 0.7, 1.1, 0.5, 0.7, 1.1); created, never added
	engine::RefPtr<engine::AlphaPulse> m_flashEffect;						// +0x17C (0.5, 0.6, 1.0); added by OrderImpatientState
	engine::RefPtr<engine::ColorPulse> m_highlightEffect;					// +0x180 (0.5 s, grey 0.5); added by flashOrder
	float m_waitTime;														// +0x184 seconds the order has waited (passed to spawnTip)
	float m_patience;														// +0x188 seconds left before hanging up (from the order)
	float m_scrambleTimer;													// +0x18C 3 s countdown to the next Scrambler shuffle
	int m_unused190;														// +0x190 never touched
	bool m_scrambling;														// +0x194 Scrambler active on this order
	bool m_concealed;														// +0x195 Concentration: OrderInactiveState after showing, until flipped; +0x196 padding
};

// The normal waiting order: shows the front card; the patience ticks down, at 2 s or less OrderImpatientState.
// 0x1C bytes (vtordisp +0x14, Interface +0x18).
class OrderWaitState : public PizzaPopupState
{
public:
	OrderWaitState(PizzaOrderPopup* popup);

	// engine::StateBase
	virtual void enter();													// slot 1
	virtual void update(engine::UpdateContext& context);					// slot 3

	PizzaOrderPopup* m_orderPopup;											// +0x10 same object as m_owner, typed
};

// Patience almost gone: adds the popup's flash effect (not in the tutorial or for special customers); at 0
// OrderHangUpState. 0x1C bytes (vtordisp +0x14, Interface +0x18). Its constructor is always inlined.
class OrderImpatientState : public PizzaPopupState
{
public:
	OrderImpatientState(PizzaOrderPopup* popup)
		: PizzaPopupState(popup), m_orderPopup(popup)
	{
	}

	// engine::StateBase
	virtual void enter();													// slot 1
	virtual void exit();													// slot 2
	virtual void update(engine::UpdateContext& context);					// slot 3

	PizzaOrderPopup* m_orderPopup;											// +0x10 same object as m_owner, typed
};

// The customer hangs up: tells GameLogic ("customerHangup" or "specialHangup"), plays popup_hangup or
// special_hangup, shrinks for 0.5 s, then popup->remove(). No input, not active. 0x20 bytes (vtordisp +0x18,
// Interface +0x1C). Its constructor is always inlined.
class OrderHangUpState : public PizzaPopupState
{
public:
	OrderHangUpState(PizzaOrderPopup* popup)
		: PizzaPopupState(popup), m_orderPopup(popup)
	{
	}

	// engine::StateBase
	virtual void enter();													// slot 1
	virtual void update(engine::UpdateContext& context);					// slot 3

	// PizzaPopupState
	virtual bool acceptsInput() const;										// slot 5 (0x4529D0, folded: return false)
	virtual bool isActive() const;											// slot 6 (0x4529D0, folded: return false)

	PizzaOrderPopup* m_orderPopup;											// +0x10 same object as m_owner, typed
	float m_time;															// +0x14 0.5 s shrink (set by enter)
};

// The order was clicked into the selection: the card flips to the back (portrait), the selection box is hidden, the
// patience keeps ticking, no input. 0x1C bytes (vtordisp +0x14, Interface +0x18). Its constructor is always
// inlined.
class OrderSelectedState : public PizzaPopupState
{
public:
	OrderSelectedState(PizzaOrderPopup* popup)
		: PizzaPopupState(popup), m_orderPopup(popup)
	{
	}

	// engine::StateBase
	virtual void enter();													// slot 1
	virtual void update(engine::UpdateContext& context);					// slot 3 (0x452060, folded with OrderHighlightState's)

	// PizzaPopupState
	virtual bool acceptsInput() const;										// slot 5 (0x4529D0, folded: return false)

	PizzaOrderPopup* m_orderPopup;											// +0x10 same object as m_owner, typed
};

// A pizza is on its way: the patience is frozen (only the wait time counts), no input, not active. 0x1C bytes
// (vtordisp +0x14, Interface +0x18). Its constructor is always inlined.
class OrderDeliveringState : public PizzaPopupState
{
public:
	OrderDeliveringState(PizzaOrderPopup* popup)
		: PizzaPopupState(popup), m_orderPopup(popup)
	{
	}

	// engine::StateBase
	virtual void update(engine::UpdateContext& context);					// slot 3

	// PizzaPopupState
	virtual bool acceptsInput() const;										// slot 5 (0x4529D0, folded: return false)
	virtual bool isActive() const;											// slot 6 (0x4529D0, folded: return false)

	PizzaOrderPopup* m_orderPopup;											// +0x10 same object as m_owner, typed
};

// Slot-machine change of the order's topping (Scrambler, coupons, special events): 7 steps of 0.05 s cycling
// through the kitchens' toppings (CityMap::getKitchenPizzas), a "popup" click per step unless silent. 0x34 bytes
// (vtordisp +0x2C, Interface +0x30).
class OrderShuffleState : public PizzaPopupState
{
public:
	OrderShuffleState(PizzaOrderPopup* popup, Topping* target, bool silent);

	// engine::StateBase
	virtual void enter();													// slot 1
	virtual void exit();													// slot 2
	virtual void update(engine::UpdateContext& context);					// slot 3

	float m_time;															// +0x10 0.05 s per step
	int m_stepsLeft;														// +0x14 7
	std::vector<engine::RefPtr<Topping> >::iterator m_current;				// +0x18 topping shown, in *m_toppings
	std::vector<engine::RefPtr<Topping> >* m_toppings;						// +0x1C the map's kitchen toppings
	Topping* m_target;														// +0x20 final topping (NULL: the one after the order's topping)
	PizzaOrderPopup* m_orderPopup;											// +0x24 same object as m_owner, typed
	bool m_silent;															// +0x28 no "popup" click per step
};

// Concentration mode: the order is darkened (colour offset -0.1) and not clickable until the orders flip over.
// 0x1C bytes (vtordisp +0x14, Interface +0x18).
class OrderInactiveState : public PizzaPopupState
{
public:
	OrderInactiveState(PizzaOrderPopup* popup);

	// engine::StateBase
	virtual void enter();													// slot 1
	virtual void exit();													// slot 2

	// PizzaPopupState
	virtual bool acceptsInput() const;										// slot 5 (0x4529D0, folded: return false)

	PizzaOrderPopup* m_orderPopup;											// +0x10 same object as m_owner, typed
};
