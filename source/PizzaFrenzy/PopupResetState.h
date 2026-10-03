// PopupResetState: the pizza popups' resting state (alpha and scale back to 1).
#pragma once

#include "PizzaKitchenPopup.h"

class PizzaPopup;

// Set by the kitchen and police popups when their show animation ends and when they are deselected: restores the
// popup's alpha and scale on enter. PizzaPopupState +0x00 (owner popup at +0x0C), then the vtordisp (+0x10) and the
// Interface subobject (+0x14); 0x18 bytes, implicit destructor, constructor always inlined.
class PopupResetState : public PizzaPopupState
{
public:
	PopupResetState(PizzaPopup* popup)
		: PizzaPopupState(popup)
	{
	}

	virtual void enter();														// slot 1 (engine::StateBase)
};
