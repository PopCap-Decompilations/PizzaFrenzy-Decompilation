#include "Order.h"

// 0x419660
Order::~Order()
{
}

// 0x419690
void Order::reset()
{
	m_deliveryTime = 0;
	m_customer = 0;
	m_numPizzas = 0;
	m_topping = 0;
	m_character = 0;
	m_patience = 0;
	m_event = 0;
}

// 0x4196E0
Order::Order()
	: m_event(0)
{
}
