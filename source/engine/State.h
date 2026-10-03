// engine::State<T>: base of every game state and action: a StateBase that knows the object it drives.
#pragma once

#include "StateBase.h"

namespace engine
{
	// StateBase +0x00, m_owner +0x0C, then the vtordisp (+0x10) and the Interface subobject (+0x14): 0x18 bytes. Its
	// vtable and Interface thunks were merged into StateBase's (0x4FC1E8) and 0x504AE8: every inlined constructor
	// stores 0x4FC1E8 twice (StateBase, then State<T>). The constructor is always inlined; the destructor is implicit.
	template <class T>
	class State : public StateBase
	{
	public:
		State(T* owner)
			: m_owner(owner)
		{
		}

		T* m_owner;											// +0x0C the game, popup, tip, vehicle... (no reference)
	};
}
