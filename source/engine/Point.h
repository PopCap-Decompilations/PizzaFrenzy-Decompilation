// engine::Vector2 (float 2D vector) and engine::Point (int 2D point); their functions are in Point.cpp.
#pragma once

namespace engine
{
	struct Point;

	// positions, scales, offsets. The default constructor leaves x and y uninitialised; the default and copy
	// constructors and set() are out-of-line bodies that /OPT:ICF folded with Point's.
	struct Vector2
	{
		Vector2();
		Vector2(float x, float y);
		Vector2(const Vector2& p);
		Vector2(const Point& p);

		void set(float x, float y);
		void offset(float dx, float dy);
		Vector2 operator*(float s) const;
		Vector2 operator+(const Vector2& p) const;
		Vector2 operator-(const Vector2& p) const;
		Vector2& operator*=(float s);
		Vector2& operator+=(const Vector2& p);
		Vector2& operator-=(const Vector2& p);
		void rotate(float radians);
		float length() const;
		void normalize();

		float x;								// +0x00
		float y;								// +0x04
	};

	// mouse positions, map cells, image pivots
	struct Point
	{
		Point();
		Point(int x, int y);
		Point(const Point& p);
		Point(const Vector2& p);				// truncates (_ftol2)

		void set(int x, int y);
		Point operator+(const Point& p) const;
		Point operator-(const Point& p) const;
		Point& operator+=(const Point& p);
		Point& operator-=(const Point& p);
		bool operator==(const Point& p) const;
		bool operator!=(const Point& p) const;
		int lengthSquared() const;

		int x;									// +0x00
		int y;									// +0x04
	};
}
