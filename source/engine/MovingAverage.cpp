#include "MovingAverage.h"

namespace engine
{
	// 0x4C1B50
	__int64 MovingAverage::getAverage() const
	{
		return m_sum / m_samples.size();
	}

	// 0x4C1B90
	void MovingAverage::add(__int64 sample)
	{
		m_sum -= *m_next;
		m_sum += sample;
		*m_next = sample;
		++m_next;
		if (m_next == m_samples.end())
		{
			m_next = m_samples.begin();
		}
	}

	// 0x4C1CC0
	MovingAverage::MovingAverage(unsigned int count)
		: m_samples(count, 0), m_sum(0)
	{
		m_next = m_samples.begin();
	}
}
