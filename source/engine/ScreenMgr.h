// engine::ScreenMgr: the screen manager, a Container whose child is the current screen.
#pragma once

#include <deque>
#include <string>

#include "Container.h"
#include "RefPtr.h"

namespace engine
{
	class Screen;
	struct UpdateContext;

	// Container +0x00, members from +0x128, then the vtordisp (+0x148) and the Interface subobject (+0x14C): 0x150
	// bytes. Screens queued by showScreen() are swapped in one per update(), using the screens' show and hide
	// transitions.
	class ScreenMgr : public Container
	{
	public:
		ScreenMgr();
		virtual ~ScreenMgr();

		virtual std::string getTypeName() const;	// slot 24 (engine::Component): "ScreenMgr"
		virtual void activate();					// slot 34 (engine::Component): unmaskFlags(2) (body folded: 0x47A4C0)
		virtual void deactivate();					// slot 35 (engine::Component): maskFlags(2), children untouched
		virtual void update(UpdateContext& context);	// slot 37 (engine::Component)

		void updateTransition(UpdateContext& ctx);
		bool isTransitioning() const;
		void clear();
		void showScreen(Screen* screen, float delay);

		RefPtr<Screen> m_currentScreen;					// +0x128 the screen shown (child of the manager)
		RefPtr<Screen> m_nextScreen;					// +0x12C screen being transitioned in
		std::deque<RefPtr<Screen> > m_pendingScreens;	// +0x130 queued by showScreen(), one taken per update
		bool m_isTransitioning;							// +0x144 set when a switch starts, cleared when both screens are idle
	};
}
