// SpecialEventTrigger: one special event of a level, which counts the waves down to its special customer's order.
#pragma once

#include "engine/Object.h"

struct EventInfo;
class Order;

// One <event name delay firstWave> of a level (created by Level::createEvents, held by GameLogic::m_specialEvents):
// every wave counts m_wavesLeft down and, at 0, creates the order of the event's special customer. ICF merged its
// vtable into 0x4F9470 (destructor only); implicit destructor. The members end at +0x14; the vtordisp (+0x14) and the
// Interface subobject (+0x18) follow them (0x1C bytes).
class SpecialEventTrigger : public engine::Object
{
public:
	SpecialEventTrigger(const EventInfo* def);

	void reset();
	Order* createOrder();
	Order* onWave();

	const EventInfo* m_def;														// +0x0C the level's event record
	int m_wavesLeft;															// +0x10 from m_def's delay range
};
