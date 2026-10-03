#include "Point.h"

#include <cmath>
#include <string.h>

namespace engine
{
	// 0x4752F0
	Point::Point(const Vector2& p)
	{
		x = (int)p.x;
		y = (int)p.y;
	}

	// 0x475320
	Vector2::Vector2(const Point& p)
	{
		x = (float)p.x;
		y = (float)p.y;
	}

	// 0x475340 (folded)
	void Vector2::set(float x, float y)
	{
		this->x = x;
		this->y = y;
	}

	// 0x475340 (folded)
	void Point::set(int x, int y)
	{
		this->x = x;
		this->y = y;
	}

	// 0x475350
	void Vector2::offset(float dx, float dy)
	{
		x += dx;
		y += dy;
	}

	// 0x475370
	Vector2 Vector2::operator*(float s) const
	{
		return Vector2(x * s, y * s);
	}

	// 0x475390
	Vector2 Vector2::operator+(const Vector2& p) const
	{
		return Vector2(x + p.x, y + p.y);
	}

	// 0x4753B0
	Vector2 Vector2::operator-(const Vector2& p) const
	{
		return Vector2(x - p.x, y - p.y);
	}

	// 0x4753D0
	Vector2& Vector2::operator*=(float s)
	{
		x *= s;
		y *= s;
		return *this;
	}

	// 0x4753F0
	Vector2& Vector2::operator+=(const Vector2& p)
	{
		x += p.x;
		y += p.y;
		return *this;
	}

	// 0x475410
	Vector2& Vector2::operator-=(const Vector2& p)
	{
		x -= p.x;
		y -= p.y;
		return *this;
	}

	// 0x475430 (folded)
	Vector2::Vector2(float x, float y)
	{
		this->x = x;
		this->y = y;
	}

	// 0x475430 (folded)
	Point::Point(int x, int y)
	{
		this->x = x;
		this->y = y;
	}

	// 0x475450
	Point Point::operator+(const Point& p) const
	{
		return Point(x + p.x, y + p.y);
	}

	// 0x475470
	Point Point::operator-(const Point& p) const
	{
		return Point(x - p.x, y - p.y);
	}

	// 0x475490
	Point& Point::operator+=(const Point& p)
	{
		x += p.x;
		y += p.y;
		return *this;
	}

	// 0x4754B0
	Point& Point::operator-=(const Point& p)
	{
		x -= p.x;
		y -= p.y;
		return *this;
	}

	// 0x4754D0
	bool Point::operator==(const Point& p) const
	{
		return x == p.x && y == p.y;
	}

	// 0x4754F0
	bool Point::operator!=(const Point& p) const
	{
		return x != p.x || y != p.y;
	}

	// 0x475520
	void Vector2::rotate(float radians)
	{
		float c = std::cos(radians);
		float s = std::sin(radians);
		float newX = c * x - s * y;
		float newY = c * y + s * x;
		x = newX;
		y = newY;
	}

	// 0x475560
	float Vector2::length() const
	{
		return std::sqrt(x * x + y * y);
	}

	// 0x475580
	void Vector2::normalize()
	{
		float len = length();
		if (len != 0.0f)
		{
			float scale = 1.0f / len;
			x *= scale;
			y *= scale;
		}
	}

	// 0x4755C0
	int Point::lengthSquared() const
	{
		return x * x + y * y;
	}

	// 0x4755D0 (folded)
	Vector2::Vector2(const Vector2& p)
	{
		memcpy(this, &p, sizeof(Vector2));
	}

	// 0x4755D0 (folded)
	Point::Point(const Point& p)
	{
		memcpy(this, &p, sizeof(Point));
	}

	// 0x4D1360 (folded)
	Vector2::Vector2()
	{
	}

	// 0x4D1360 (folded)
	Point::Point()
	{
	}
}
