#include "Range.h"

#include <stdlib.h>

namespace engine
{
	// 0x476C20
	float randomFloat(float min, float max)
	{
		return min + rand() * (1.0f / 32767.0f) * (max - min);
	}

	// 0x476C50
	float Range::random() const
	{
		return randomFloat(min, max);
	}

	// 0x479540
	Range::Range()
		: min(0.0f), max(0.0f)
	{
	}

	// 0x479540 (folded)
	IntRange::IntRange()
		: min(0), max(0)
	{
	}

	// 0x475430 (folded)
	Range::Range(float min, float max)
		: min(min), max(max)
	{
	}

	// 0x475430 (folded)
	IntRange::IntRange(int min, int max)
		: min(min), max(max)
	{
	}

	// 0x479550: the offset is added to a even when a > b
	int randomInt(int a, int b)
	{
		return a + rand() % ((a > b ? a - b : b - a) + 1);
	}

	// 0x479580
	int IntRange::random() const
	{
		return randomInt(min, max);
	}
}
