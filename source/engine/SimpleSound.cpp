#include "SimpleSound.h"

namespace engine
{
	// 0x497800
	SimpleSound::SimpleSound()
	{
		for (int i = 0; i < 64; i++)
			m_categoryVolumes[i] = 1.0f;
	}

	// 0x497820
	void SimpleSound::setCategoryVolume(int category, float volume)
	{
		if (category < 64 && category >= 0)
		{
			if (volume > 1.0f)
				volume = 1.0f;
			else if (volume < 0.0f)
				volume = 0.0f;
			m_categoryVolumes[category] = volume;
		}
	}

	// 0x497870
	float SimpleSound::getCategoryVolume(int category)
	{
		if (category < 64 && category >= 0)
			return m_categoryVolumes[category];
		return 1.0f;
	}
}
