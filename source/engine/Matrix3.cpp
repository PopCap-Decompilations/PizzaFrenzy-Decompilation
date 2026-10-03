#include "Matrix3.h"

#include <cmath>

namespace engine
{
	// 0x4D1360 (folded)
	Matrix3::Matrix3()
	{
	}

	// 0x4D0470 (folded)
	Matrix3::~Matrix3()
	{
	}

	// 0x4D1370
	void Matrix3::setIdentity()
	{
		m[0] = 1.0f;
		m[1] = 0.0f;
		m[2] = 0.0f;
		m[3] = 0.0f;
		m[4] = 1.0f;
		m[5] = 0.0f;
		m[6] = 0.0f;
		m[7] = 0.0f;
		m[8] = 1.0f;
	}

	// 0x4D13A0
	void Matrix3::rotate(float angle)
	{
		float c = cos(angle);
		float s = sin(angle);
		float t;

		t = m[0];
		m[0] = t * c - s * m[3];
		m[3] = t * s + c * m[3];

		t = m[1];
		m[1] = t * c - s * m[4];
		m[4] = t * s + c * m[4];

		t = m[2];
		m[2] = t * c - s * m[5];
		m[5] = t * s + c * m[5];
	}

	// 0x4D1410
	void Matrix3::translate(float x, float y)
	{
		m[0] += x * m[6];
		m[3] += y * m[6];
		m[1] += x * m[7];
		m[4] += y * m[7];
		m[2] += x * m[8];
		m[5] += y * m[8];
	}

	// 0x4D1460
	void Matrix3::scale(float scaleX, float scaleY)
	{
		m[0] *= scaleX;
		m[3] *= scaleY;
		m[1] *= scaleX;
		m[4] *= scaleY;
		m[2] *= scaleX;
		m[5] *= scaleY;
	}

	// 0x4D14A0
	Vector2 Matrix3::transform(float x, float y) const
	{
		return Vector2(x * m[0] + y * m[1] + m[2], x * m[3] + y * m[4] + m[5]);
	}
}
