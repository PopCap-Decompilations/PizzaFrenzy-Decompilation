// engine::StateMachine: holds the current state object and forwards updates and commands to it.
#pragma once

#include <string>

#include "Object.h"
#include "RefPtr.h"

namespace engine
{
	class StateBase;
	struct UpdateContext;

	// Object +0x00, m_state +0x0C, then the vtordisp (+0x10) and the Interface subobject (+0x14): 0x18 bytes.
	// Held by value by GameLogic and (as GameStateMachine, G10) by MusicPlayer and PizzaPopup.
	class StateMachine : public Object
	{
	public:
		StateMachine();
		virtual ~StateMachine();

		virtual bool setState(StateBase* state);					// slot 1: old state's exit, new state's enter; true
		virtual bool changeState(StateBase* state);					// slot 2: return setState(state)
		virtual void update(UpdateContext& ctx);					// slot 3: the current state's update
		virtual void handleCommand(const std::string& command);		// slot 4: the current state's slot 4

		RefPtr<StateBase> m_state;				// +0x0C current state
	};
}
