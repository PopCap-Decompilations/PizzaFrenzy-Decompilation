// engine::StateBase: root of the states and actions run by engine::StateMachine (enter/exit/update/onAction with
// empty defaults).
#pragma once

#include <string>

#include "Object.h"

namespace engine
{
	struct UpdateContext;

	// Object +0x00, no members, then the vtordisp (+0x0C) and the Interface subobject (+0x10): 0x14 bytes. Its vtable
	// (0x4FC1E8) is shared with engine::State<T>; constructor and destructor are implicit (the constructor is inlined
	// everywhere; the folded complete destructor 0x430060 is a jmp to Object::~Object). The four empty defaults are
	// ICF-folded bodies.
	class StateBase : public Object
	{
	public:
		// 0x4D0470 (folded)
		virtual void enter()											// slot 1: StateMachine::setState, new state
		{
		}

		// 0x4D0470 (folded)
		virtual void exit()												// slot 2: StateMachine::setState, old state
		{
		}

		// 0x492310 (folded)
		virtual void update(UpdateContext& context)						// slot 3
		{
		}

		// 0x492310 (folded)
		virtual void onAction(const std::string& action)				// slot 4: the screens' actions
		{
		}
	};
}
