#include "AlphaPulse.h"

#include <cmath>

#include "Component.h"

namespace engine
{
	float AlphaPulse::s_pi = 3.14159265f;		// 0x521340

	// 0x47B110 (folded)
	void AlphaPulse::set(float period, float minAlpha, float maxAlpha)
	{
		m_period = period;
		m_minAlpha = minAlpha;
		m_maxAlpha = maxAlpha;
	}

	// 0x481A90
	AlphaPulse::AlphaPulse(float period, float minAlpha, float maxAlpha, float time)
		: m_period(period), m_minAlpha(minAlpha), m_maxAlpha(maxAlpha), m_time(time)
	{
	}

	// 0x481B50
	void AlphaPulse::update(UpdateContext& context, Component* component)
	{
		m_time += context.elapsed;
		component->setAlpha((sin(m_time / m_period * s_pi * 2.0f) + 1.0f) * (m_maxAlpha - m_minAlpha) * 0.5f + m_minAlpha);
	}
}
