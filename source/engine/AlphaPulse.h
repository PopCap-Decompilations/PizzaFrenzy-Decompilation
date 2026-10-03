// engine::AlphaPulse: an animator that oscillates its component's alpha between a minimum and a maximum.
#pragma once

#include "Animator.h"
#include "Object.h"

namespace engine
{
	// vfptr +0x00 (its own slot), vbptr +0x04, members from +0x08, then the virtual bases, each after a vtordisp
	// except Object: Interface (+0x1C), Animator (+0x24), Object (+0x2C): 0x38 bytes. No user-declared destructor.
	class AlphaPulse : public virtual Animator, public virtual Object
	{
	public:
		AlphaPulse(float period, float minAlpha, float maxAlpha, float time);

		virtual void set(float period, float minAlpha, float maxAlpha);		// slot 0: folded with HsvFilter::set (0x47B110)

		virtual void update(UpdateContext& context, Component* component);		// slot 0 (engine::Animator)

		float m_period;							// +0x08
		float m_minAlpha;						// +0x0C
		float m_maxAlpha;						// +0x10
		float m_time;							// +0x14 phase time

		static float s_pi;						// 0x521340: 3.14159274 (writable data, a copy separate from Oscillator's)
	};
}
