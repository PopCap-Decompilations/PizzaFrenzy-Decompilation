// engine::Blink: an animator that toggles its component's visibility (on time, off time).
#pragma once

#include "Animator.h"
#include "Object.h"

namespace engine
{
	// No vfptr of its own (no new virtuals; the class is keyed by its Animator vtable 0x5030F8): vbptr +0x00, members
	// from +0x04, then the virtual bases, each after a vtordisp except Object: Interface (+0x14), Animator (+0x1C),
	// Object (+0x24): 0x30 bytes. No user-declared destructor.
	class Blink : public virtual Animator, public virtual Object
	{
	public:
		Blink(float onTime, float offTime);

		virtual void update(UpdateContext& context, Component* component);		// slot 0 (engine::Animator)

		float m_onTime;							// +0x04 visible time
		float m_offTime;						// +0x08 hidden time
		float m_timer;							// +0x0C time to the next toggle
	};
}
