// engine::Matrix3: a 3x3 row-major 2D affine transform (the rotated blits of drawImage).
#pragma once

#include "Point.h"

namespace engine
{
	// No vtable. rotate/translate/scale pre-multiply (they apply after what the matrix already does);
	// drawImage builds identity, translate(-pivot), scale, rotate(angle), translate(x, y).
	struct Matrix3
	{
		Matrix3();								// 0x4D1360 (folded): empty, the elements are left uninitialised
		~Matrix3();								// 0x4D0470 (folded): empty

		void setIdentity();
		void rotate(float angle);				// radians
		void translate(float x, float y);
		void scale(float scaleX, float scaleY);
		Vector2 transform(float x, float y) const;

		float m[9];								// +0x00 rows (m[0] m[1] m[2]) x' and (m[3] m[4] m[5]) y'; (m[6] m[7] m[8]) = (0 0 1)
	};
}
