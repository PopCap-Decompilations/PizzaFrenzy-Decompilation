#include "WaveEffect.h"

namespace engine
{
	// 0x411960 (folded)
	int WaveEffect::getType() const
	{
		return m_type;
	}

	// 0x4D1330
	float WaveEffect::getAmplitude() const
	{
		return m_amplitude;
	}

	// 0x4D1340
	float WaveEffect::getFrequency() const
	{
		return m_frequency;
	}

	// 0x4D1350
	float WaveEffect::getPhase() const
	{
		return m_phase;
	}
}
