#include "StateMachine.h"

#include "StateBase.h"

namespace engine
{
	// 0x4706C0
	bool StateMachine::changeState(StateBase* state)
	{
		return setState(state);
	}

	// 0x4706D0
	StateMachine::~StateMachine()
	{
	}

	// 0x470750
	StateMachine::StateMachine()
	{
	}

	// 0x4707F0
	bool StateMachine::setState(StateBase* state)
	{
		if (m_state)
			m_state->exit();
		m_state = state;
		if (m_state)
			m_state->enter();
		return true;
	}

	// 0x470850
	void StateMachine::handleCommand(const std::string& command)
	{
		if (m_state)
			m_state->onAction(command);
	}

	// 0x470860
	void StateMachine::update(UpdateContext& ctx)
	{
		if (m_state)
			m_state->update(ctx);
	}
}
