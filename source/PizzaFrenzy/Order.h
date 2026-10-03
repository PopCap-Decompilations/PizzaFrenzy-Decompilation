// Order: a pizza order (plain data) queued by the popup manager and shown by PizzaOrderPopup.
#pragma once

#include "engine/Object.h"

class Character;
class CustomerTile;
class SpecialEvent;
class Topping;

// The constructor only clears m_event (callers call reset); copied memberwise by the implicit copy constructor.
class Order : public engine::Object
{
public:
	Order();
	virtual ~Order();

	void reset();

	// 0x411960 (folded): the shared getter of the member at +0x0C (GameLogic::prepareOrder)
	CustomerTile* getCustomer() const
	{
		return m_customer;
	}

	CustomerTile* m_customer;							// +0x0C the customer the order is for
	Character* m_character;								// +0x10 special-event orders
	Topping* m_topping;									// +0x14 the ordered topping
	SpecialEvent* m_event;								// +0x18 null for normal orders
	int m_numPizzas;									// +0x1C
	float m_patience;									// +0x20 seconds
	float m_deliveryTime;								// +0x24 seconds the delivery took (tip stars)
	int m_serial;										// +0x28 not touched by reset
};
