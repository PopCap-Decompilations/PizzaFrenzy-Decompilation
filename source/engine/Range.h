// engine::Range (float), engine::IntRange (int) and the random number helpers; their functions are in Range.cpp.
#pragma once

namespace engine
{
	// min + rand() / 32767 * (max - min)
	float randomFloat(float min, float max);
	// a + rand() % (|b - a| + 1)
	int randomInt(int a, int b);

	// a range for random values: "( min , max )" or one number in the XML files
	struct Range
	{
		Range();								// min = max = 0
		Range(float min, float max);			// body folded with Vector2(float, float) (0x475430)

		float random() const;

		float min;								// +0x00
		float max;								// +0x04
	};

	struct IntRange
	{
		IntRange();								// min = max = 0 (body folded with Range's)
		IntRange(int min, int max);				// body folded with Vector2(float, float) (0x475430)

		int random() const;

		int min;								// +0x00
		int max;								// +0x04
	};
}
