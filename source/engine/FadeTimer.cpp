#include "FadeTimer.h"

#include <algorithm>

namespace engine
{
	// 0x4810A0
	float FadeTimer::getValue() const
	{
		return m_time * m_rate;
	}

	// 0x4810B0: forwards at the duration, backwards at 0
	bool FadeTimer::isFinished() const
	{
		if (m_forward)
			return m_time >= m_duration;
		return m_time <= 0.0f;
	}

	// 0x4810E0
	void FadeTimer::resume()
	{
		m_running = true;
	}

	// 0x4810F0
	void FadeTimer::pause()
	{
		m_running = false;
	}

	// 0x481100
	void FadeTimer::setForward(bool forward)
	{
		m_forward = forward;
	}

	// 0x481110: the time is clamped to [0, m_duration] (stored after every step)
	void FadeTimer::update(float dt)
	{
		if (m_running)
		{
			m_time += m_forward ? dt : -dt;
			m_time = std::min(m_time, m_duration);
			m_time = std::max(0.0f, m_time);
		}
	}

	// 0x481170
	bool FadeTimer::restart()
	{
		m_time = 0.0f;
		m_running = true;
		m_forward = true;
		return true;
	}

	// 0x481180: the value runs from 0 to maxValue over duration (rate 0 for a zero duration)
	bool FadeTimer::start(float duration, float maxValue)
	{
		m_duration = duration;
		if (duration != 0.0f)
			m_rate = maxValue / duration;
		else
			m_rate = 0.0f;
		return restart();
	}
}
