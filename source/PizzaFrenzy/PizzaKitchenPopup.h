// PizzaPopupState (the base of the popup states) and PizzaKitchenPopup (a kitchen's topping button).
#pragma once

#include <string>

#include "engine/RefPtr.h"
#include "engine/State.h"

class PizzaPopup;

// Base of the states a PizzaPopup's GameStateMachine runs (show, close, the order states...): empty
// enter/exit/update, accepts input and active by default. Used directly as the highlighted and selected state of
// the kitchen and police popups. The owner popup (raw pointer) is State<PizzaPopup>::m_owner at +0xC; 0x18 bytes
// (vtordisp +0x10, Interface +0x14). Declared before PizzaPopup.h is included: PizzaPopup.h needs it for its own
// states and includes this file after declaring PizzaPopup.
class PizzaPopupState : public engine::State<PizzaPopup>
{
public:
	PizzaPopupState(PizzaPopup* popup);

	virtual bool acceptsInput() const;										// slot 5 (0x451610, folded: return true)
	virtual bool isActive() const;											// slot 6 (0x451610, folded: return true)
};

// The resting state of a kitchen or police popup that is highlighted (while orders are selected) or selected:
// accepts input and is active, no animation. Not in the tables: it overrides nothing, so its vtable was identical
// to PizzaPopupState's and /OPT:ICF merged the two (0x500218). PizzaKitchenPopup::highlight/select and
// PolicePopup::highlight/select construct it: PizzaPopupState's constructor as a base (most-derived flag 0), then
// vtable 0x500218 and the vtordisp stored again. 0x18 bytes; the constructor is always inlined. (Name guessed.)
class PopupActiveState : public PizzaPopupState
{
public:
	PopupActiveState(PizzaPopup* popup)
		: PizzaPopupState(popup)
	{
	}
};

#include "PizzaPopup.h"

namespace engine
{
	class Container;
	class Image;
	class Object;
	class TextItem;
}

class BuildingTile;
class Topping;

// A kitchen's topping button (res/pizza/toppingButton.jpg): the topping image and a price tag
// (pizzaMultiplierBkg.jpg with hudFont "$N"). Popup type 1; created by KitchenTile::setTopping. 0x16C bytes:
// PizzaPopup up to +0x14C, the members below, vtordisp +0x164, Interface +0x168.
class PizzaKitchenPopup : public PizzaPopup
{
public:
	PizzaKitchenPopup();
	virtual ~PizzaKitchenPopup();

	// engine::Component
	virtual std::string getTypeName() const;								// slot 24: "PizzaKitchenPopup"

	// PizzaPopup
	virtual void init(BuildingTile* owner);									// slot 75
	virtual int getPopupType() const;										// slot 78: 1
	virtual void onShowFinished();											// slot 79
	virtual void onCloseFinished();											// slot 80 (0x452050, folded: remove())
	virtual void highlight();												// slot 81
	virtual void select();													// slot 82
	virtual void onDispatched();											// slot 83 (0x4517B0, folded with deselect)
	virtual void deselect();												// slot 84 (0x4517B0)

	virtual Topping* getTopping() const;									// slot 90 (0x452890, folded: return m_topping)

	void setPrice(int price);
	void setTopping(Topping* topping);

	engine::RefPtr<Topping> m_topping;										// +0x14C topping sold by the kitchen (setTopping)
	engine::RefPtr<engine::Container> m_toppingIcon;						// +0x150 holds the topping image, at (-10,-12)
	engine::RefPtr<engine::Image> m_button;									// +0x154 res/pizza/toppingButton.jpg at (-2,2)
	engine::RefPtr<engine::Container> m_priceTag;							// +0x158 pizzaMultiplierBkg.jpg and m_priceText, at (24,-26)
	engine::RefPtr<engine::TextItem> m_priceText;							// +0x15C hudFont text "$N"
	engine::RefPtr<engine::Object> m_unused160;								// +0x160 released by the destructor, never set
};
