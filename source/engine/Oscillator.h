// engine::Oscillator: an animator that moves its component along two sine waves.
#pragma once

#include "Animator.h"
#include "Object.h"

namespace engine
{
	// vfptr +0x00 (its own two slots), vbptr +0x04, members from +0x08, then the virtual bases, each after a
	// vtordisp except Object: Interface (+0x28), Animator (+0x30), Object (+0x38): 0x44 bytes. No user-declared
	// destructor.
	class Oscillator : public virtual Animator, public virtual Object
	{
	public:
		Oscillator(float periodX, float periodY, float amplitudeX, float amplitudeY, float time);

		virtual void setHorizontal(float period, float amplitude);				// slot 0
		virtual void setVertical(float period, float amplitude);				// slot 1

		virtual void update(UpdateContext& context, Component* component);		// slot 0 (engine::Animator)

		float m_periodX;						// +0x08 a zero period becomes 1
		float m_periodY;						// +0x0C a zero period becomes 1
		float m_amplitudeX;						// +0x10
		float m_amplitudeY;						// +0x14
		float m_time;							// +0x18 phase time
		float m_offsetX;						// +0x1C offset applied last update (removed before adding the new one)
		float m_offsetY;						// +0x20

		static float s_pi;						// 0x52133C: 3.14159274 (writable data)
	};
}
