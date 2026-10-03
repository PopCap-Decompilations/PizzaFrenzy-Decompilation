#include "Oscillator.h"

#include <cmath>

#include "Component.h"

namespace engine
{
	float Oscillator::s_pi = 3.14159265f;		// 0x52133C

	// 0x47F410
	void Oscillator::setHorizontal(float period, float amplitude)
	{
		m_periodX = period;
		m_amplitudeX = amplitude;
	}

	// 0x47F430
	void Oscillator::setVertical(float period, float amplitude)
	{
		m_periodY = period;
		m_amplitudeY = amplitude;
	}

	// 0x47F450
	Oscillator::Oscillator(float periodX, float periodY, float amplitudeX, float amplitudeY, float time)
		: m_periodX(periodX), m_periodY(periodY), m_amplitudeX(amplitudeX), m_amplitudeY(amplitudeY), m_time(time),
		  m_offsetX(0.0f), m_offsetY(0.0f)
	{
		if (periodX == 0.0f)
			m_periodX = 1.0f;
		if (periodY == 0.0f)
			m_periodY = 1.0f;
	}

	// 0x47F550
	void Oscillator::update(UpdateContext& context, Component* component)
	{
		m_time += context.elapsed;
		float offsetX = sin(m_time / m_periodX * s_pi * 2.0f) * m_amplitudeX;
		float offsetY = sin(m_time / m_periodY * s_pi * 2.0f) * m_amplitudeY;
		Vector2 position(component->getPosition());
		position.x += offsetX - m_offsetX;
		m_offsetX = offsetX;
		position.y += offsetY - m_offsetY;
		m_offsetY = offsetY;
		component->setPosition(position);
	}
}
