// OrderHighlightState: the state of an order bubble under the mouse (patience keeps ticking, input allowed).
#pragma once

#include "PizzaKitchenPopup.h"
#include "PizzaOrderPopup.h"

// Set by PizzaOrderPopup::highlight (0x452E20), which inlines the constructor. Every slot but update is
// PizzaPopupState's; update is one /OPT:ICF body with OrderSelectedState's (0x452060). The vtordisp (+0x14) and the
// Interface subobject (+0x18) follow m_orderPopup (0x1C bytes).
class OrderHighlightState : public PizzaPopupState
{
public:
	// no out-of-line copy: inlined in PizzaOrderPopup::highlight
	OrderHighlightState(PizzaOrderPopup* popup)
		: PizzaPopupState(popup), m_orderPopup(popup)
	{
	}

	virtual void update(engine::UpdateContext& context);	// slot 3 (engine::StateBase), folded body 0x452060

	PizzaOrderPopup* m_orderPopup;							// +0x10 the owner popup (m_owner) with its own type; raw
};
