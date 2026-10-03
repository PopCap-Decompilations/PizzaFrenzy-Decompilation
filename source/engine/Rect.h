// engine::Rect (float rectangle) and engine::IntRect (int rectangle); their functions are in Rect.cpp.
#pragma once

namespace engine
{
	// bounds and areas in float coordinates (Component::m_bounds, Container::m_rect). The constructors, the empty
	// destructor, operator= and set() are out-of-line bodies that /OPT:ICF folded with IntRect's. operator= returns
	// nothing (0x470890 leaves the copied bottom in eax, not this).
	struct Rect
	{
		Rect();									// all zero
		Rect(float left, float top, float right, float bottom);
		~Rect();
		void operator=(const Rect& r);

		float getWidth() const;
		float getHeight() const;
		float getArea() const;
		void set(float left, float top, float right, float bottom);
		void clear();
		bool isEmpty() const;
		void scale(float sx, float sy);
		bool contains(const Rect& r) const;
		bool contains(float x, float y) const;
		bool intersects(const Rect& r) const;
		void offset(float dx, float dy);
		void intersect(const Rect& a, const Rect& b);
		void unite(const Rect& r);
		void normalize();

		float left;								// +0x00
		float top;								// +0x04
		float right;							// +0x08
		float bottom;							// +0x0C
	};

	// pixel rectangles: image source rects (engine::Image), clip and lock rects of the display code
	struct IntRect
	{
		IntRect();								// all zero
		IntRect(int left, int top, int right, int bottom);
		~IntRect();
		void operator=(const IntRect& r);

		int getWidth() const;
		int getHeight() const;
		void set(int left, int top, int right, int bottom);
		bool isEmpty() const;
		void offset(int dx, int dy);
		void intersect(const IntRect& a, const IntRect& b);

		int left;								// +0x00
		int top;								// +0x04
		int right;								// +0x08
		int bottom;								// +0x0C
	};
}
