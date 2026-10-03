// engine::ActionListener: receives a screen layout's actions (button commands, radio selections, keys) by name.
#pragma once

#include <string>

namespace engine
{
	// A plain interface with no base: in engine::Screen it sits at +0x1AC with only its vfptr (the next member,
	// m_actionSignal, is at +0x1B0) and the Screen constructor stores no vbptr for it. Its abstract vtable {_purecall}
	// was merged into 0x503A70 (Animator's); constructor always inlined.
	class ActionListener
	{
	public:
		virtual void onAction(std::string action) = 0;					// slot 0
	};
}
