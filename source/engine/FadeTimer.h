// engine::FadeTimer: runs a value from 0 to a maximum over a duration, forwards or backwards (fades, tweens).
#pragma once

#include "Object.h"

namespace engine
{
	// Held by value in game objects (AnimatedContainer +0x350, StoryScreen +0x380/+0x3A4, DecoratePizzaGame...).
	// Object +0x00, members from +0x0C, then the vtordisp (+0x1C) and the Interface subobject (+0x20): 0x24 bytes.
	// No virtual function of its own: its vtable (only the empty destructor) was merged into 0x4F9470. Its
	// constructor is implicit (the inlined copies only construct Object and leave the members uninitialised).
	class FadeTimer : public Object
	{
	public:
		float getValue() const;						// m_time * m_rate
		bool isFinished() const;
		void resume();
		void pause();
		void setForward(bool forward);
		void update(float dt);
		bool restart();
		bool start(float duration, float maxValue);

		float m_duration;						// +0x0C
		float m_rate;							// +0x10 maximum value / duration
		float m_time;							// +0x14 clamped to [0, m_duration]
		bool m_running;							// +0x18
		bool m_forward;							// +0x19
	};
}
