#include "ColorPulse.h"

#include <cmath>

#include "Component.h"

namespace engine
{
	float ColorPulse::s_pi = 3.14159265f;		// 0x521344

	// 0x483A90
	void ColorPulse::setPulse(float period, const Color& color)
	{
		m_period = period;
		m_color = color;
	}

	// 0x483AC0
	ColorPulse::ColorPulse(float period, const Color& color, float time, int colorMode)
		: m_period(period), m_color(color), m_colorMode(colorMode), m_time(time)
	{
	}

	// 0x483BC0: the colour is set with green and blue swapped (sic)
	void ColorPulse::update(UpdateContext& context, Component* component)
	{
		m_time += context.elapsed;
		float angle = m_time / m_period * s_pi * 2.0f;
		Color color;
		float level = (sin(angle) + 1.0f) * 0.5f;
		color.set(level * m_color.r, level * m_color.b, level * m_color.g);
		component->setColor(color);
		component->setColorMode(m_colorMode);
	}
}
