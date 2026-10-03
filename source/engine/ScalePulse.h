// engine::ScalePulse: animator that pulses its component's scale between a minimum and a maximum per axis.
#pragma once

#include "Animator.h"
#include "Object.h"

namespace engine
{
	class Component;

	// Run from the component's animator list; implicit destructor (its deleting destructor is folded with
	// Oscillator's). Layout: vfptr +0x00, vbptr +0x04, members +0x08..+0x23, then the virtual bases: Interface
	// +0x28, Animator +0x30, Object +0x38 (0x44 bytes).
	class ScalePulse : public virtual Animator, public virtual Object
	{
	public:
		ScalePulse(float periodX, float minX, float maxX, float periodY, float minY, float maxY, float time);

		// slot 0: horizontal oscillation
		virtual void setPulseX(float period, float minScale, float maxScale);
		// slot 1: vertical oscillation
		virtual void setPulseY(float period, float minScale, float maxScale);

		// Animator
		virtual void update(UpdateContext& context, Component* component);

		float m_periodX;						// +0x08 seconds per horizontal pulse
		float m_periodY;						// +0x0C seconds per vertical pulse
		float m_minX;							// +0x10 smallest horizontal scale
		float m_maxX;							// +0x14 largest horizontal scale
		float m_minY;							// +0x18 smallest vertical scale
		float m_maxY;							// +0x1C largest vertical scale
		float m_time;							// +0x20 accumulated time

		static float s_pi;						// 0x521348: 3.14159274 (writable data, this file's own copy)
	};
}
