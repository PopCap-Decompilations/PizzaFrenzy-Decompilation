// CitizenAward: the tip left when a criminal's order is settled.
#pragma once

#include "Tip.h"

class Topping;

// The tip GameLogic::spawnTip leaves instead of a cash tip when the order's special character is a criminal
// ("Citizen Award! $%d", res\tips\citizenAward.jpg, always $500). Adds no members: 0x16C bytes like Tip
// (has_slots<> +0x128, vtordisp +0x164, Interface +0x168).
class CitizenAward : public Tip
{
public:
	CitizenAward();

	// Tip
	virtual void init(float size, Topping* topping);						// slot 75: ignores its arguments
	virtual void collect();													// slot 78
};
