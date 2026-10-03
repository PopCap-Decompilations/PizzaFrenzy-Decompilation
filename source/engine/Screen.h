// engine::Screen: a screen layout with delayed show/hide transitions that rebroadcasts every layout action on a
// signal (base of the game's screens).
#pragma once

#include <string>

#include "ActionListener.h"
#include "RefPtr.h"
#include "ScreenLayout.h"
#include "sigslot.h"

namespace engine
{
	class Transition;
	struct UpdateContext;

	// ScreenLayout +0x00 (ButtonListener +0x144, has_slots<> +0x148), ActionListener +0x1AC, members from +0x1B0,
	// then the vtordisp (+0x1D4) and the Interface subobject (+0x1D8): 0x1DC bytes. The constructor points
	// ScreenLayout's action listener at the screen's own ActionListener.
	// m_transitionState: 0 none, 1 hidden, 2 show delay, 3 showing, 4 shown, 5 hide delay, 6 hiding.
	class Screen : public ScreenLayout, public ActionListener
	{
	public:
		Screen();
		virtual ~Screen();

		virtual void update(UpdateContext& context);					// slot 37 (engine::Component): transitions

		virtual void setTransitions(Transition* showTransition, Transition* hideTransition);	// slot 89
		virtual void setTransitionDelay(float delay);					// slot 90
		virtual void prepareShow();										// slot 91: show animator at progress 0
		virtual void show();											// slot 92
		virtual void hide();											// slot 93
		virtual bool isIdle() const;									// slot 94: no transition pending

		// ActionListener (+0x1AC)
		virtual void onAction(std::string action);						// slot 0: m_actionSignal.emit(action)

		sigslot::signal1<const std::string&> m_actionSignal;	// +0x1B0 every action of the layout
		int m_transitionState;								// +0x1C0
		RefPtr<Transition> m_hideTransition;				// +0x1C4
		RefPtr<Transition> m_showTransition;				// +0x1C8
		float m_transitionDelay;							// +0x1CC
		float m_delayLeft;									// +0x1D0
	};
}
