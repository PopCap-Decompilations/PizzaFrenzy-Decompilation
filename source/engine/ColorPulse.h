// engine::ColorPulse: animator that pulses its component's colour (sine wave from black to a peak colour).
#pragma once

#include "Animator.h"
#include "Color.h"
#include "Object.h"

namespace engine
{
	class Component;

	// Run from the component's animator list; implicit destructor (0x483CC0 only destroys m_color). Layout: vfptr
	// +0x00, vbptr +0x04, members +0x08..+0x23, then the virtual bases: Interface +0x28, Animator +0x30, Object
	// +0x38 (0x44 bytes).
	class ColorPulse : public virtual Animator, public virtual Object
	{
	public:
		ColorPulse(float period, const Color& color, float time, int colorMode);

		// slot 0: m_period and m_color
		virtual void setPulse(float period, const Color& color);

		// Animator
		virtual void update(UpdateContext& context, Component* component);

		float m_period;							// +0x08 seconds per pulse
		Color m_color;							// +0x0C peak colour
		int m_colorMode;						// +0x1C passed to Component::setColorMode (1 additive, 2 modulate)
		float m_time;							// +0x20 accumulated time (start value from the constructor)

		static float s_pi;						// 0x521344: 3.14159274 (writable data, this file's own copy)
	};
}
