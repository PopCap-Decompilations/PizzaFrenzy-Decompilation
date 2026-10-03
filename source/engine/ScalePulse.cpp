#include "ScalePulse.h"

#include <cmath>

#include "Component.h"

namespace engine
{
	float ScalePulse::s_pi = 3.14159265f;		// 0x521348

	// 0x483CD0
	void ScalePulse::setPulseX(float period, float minScale, float maxScale)
	{
		m_periodX = period;
		m_minX = minScale;
		m_maxX = maxScale;
	}

	// 0x483CF0
	void ScalePulse::setPulseY(float period, float minScale, float maxScale)
	{
		m_periodY = period;
		m_minY = minScale;
		m_maxY = maxScale;
	}

	// 0x483D10
	ScalePulse::ScalePulse(float periodX, float minX, float maxX, float periodY, float minY, float maxY, float time)
		: m_periodX(periodX), m_periodY(periodY), m_minX(minX), m_maxX(maxX), m_minY(minY), m_maxY(maxY), m_time(time)
	{
	}

	// 0x483DE0
	void ScalePulse::update(UpdateContext& context, Component* component)
	{
		m_time += context.elapsed;
		component->setScale(m_minX + (m_maxX - m_minX) * (sin(m_time / m_periodX * s_pi * 2.0f) + 1.0f) * 0.5f,
			m_minY + (m_maxY - m_minY) * (sin(m_time / m_periodY * s_pi * 2.0f) + 1.0f) * 0.5f);
	}
}
