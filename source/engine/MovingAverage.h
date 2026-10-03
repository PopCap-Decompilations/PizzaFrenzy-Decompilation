// engine::MovingAverage: the average of the last N samples, kept in a ring (the application's frame timers).
#pragma once

#include <vector>

namespace engine
{
	class MovingAverage
	{
	public:
		MovingAverage(unsigned int count);

		__int64 getAverage() const;
		void add(__int64 sample);

		std::vector<__int64> m_samples;						// +0x00 the last count samples (count zeros at first)
		std::vector<__int64>::iterator m_next;				// +0x10 the oldest sample: add() replaces it and moves on
															// +0x14 (padding: m_sum is 8-aligned)
		__int64 m_sum;										// +0x18 sum of m_samples
	};
}
