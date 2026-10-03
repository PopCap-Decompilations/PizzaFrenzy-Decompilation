#include "Rect.h"

#include <algorithm>

namespace engine
{
	// 0x470870
	int IntRect::getWidth() const
	{
		return right - left;
	}

	// 0x470880
	int IntRect::getHeight() const
	{
		return bottom - top;
	}

	// 0x470890 (folded)
	void Rect::operator=(const Rect& r)
	{
		left = r.left;
		top = r.top;
		right = r.right;
		bottom = r.bottom;
	}

	// 0x470890 (folded)
	void IntRect::operator=(const IntRect& r)
	{
		left = r.left;
		top = r.top;
		right = r.right;
		bottom = r.bottom;
	}

	// 0x4708B0
	bool IntRect::isEmpty() const
	{
		return left >= right || top >= bottom;
	}

	// 0x4708D0
	void IntRect::offset(int dx, int dy)
	{
		left += dx;
		right += dx;
		top += dy;
		bottom += dy;
	}

	// 0x4708F0
	float Rect::getWidth() const
	{
		return right - left;
	}

	// 0x470900
	float Rect::getHeight() const
	{
		return bottom - top;
	}

	// 0x470910
	float Rect::getArea() const
	{
		return (bottom - top) * (right - left);
	}

	// 0x470920 (folded)
	void Rect::set(float left, float top, float right, float bottom)
	{
		this->left = left;
		this->top = top;
		this->right = right;
		this->bottom = bottom;
	}

	// 0x470920 (folded)
	void IntRect::set(int left, int top, int right, int bottom)
	{
		this->left = left;
		this->top = top;
		this->right = right;
		this->bottom = bottom;
	}

	// 0x470940
	void Rect::clear()
	{
		left = 0.0f;
		top = 0.0f;
		right = 0.0f;
		bottom = 0.0f;
	}

	// 0x470950
	bool Rect::isEmpty() const
	{
		return left >= right || top >= bottom;
	}

	// 0x470980
	void Rect::scale(float sx, float sy)
	{
		left *= sx;
		right *= sx;
		top *= sy;
		bottom *= sy;
	}

	// 0x4709B0
	bool Rect::contains(const Rect& r) const
	{
		return r.right <= right && r.bottom <= bottom && r.left >= left && r.top >= top;
	}

	// 0x470A00
	bool Rect::contains(float x, float y) const
	{
		return x >= left && x < right && y >= top && y < bottom;
	}

	// 0x470A50: false when either rectangle has no area
	bool Rect::intersects(const Rect& r) const
	{
		if (r.getWidth() <= 0.0f || r.getHeight() <= 0.0f || getWidth() <= 0.0f || getHeight() <= 0.0f)
			return false;
		return r.right > left && r.bottom > top && right > r.left && bottom > r.top;
	}

	// 0x470AF0
	void Rect::offset(float dx, float dy)
	{
		left += dx;
		right += dx;
		top += dy;
		bottom += dy;
	}

	// 0x470B20 (folded)
	Rect::Rect()
		: left(0.0f), top(0.0f), right(0.0f), bottom(0.0f)
	{
	}

	// 0x470B20 (folded)
	IntRect::IntRect()
		: left(0), top(0), right(0), bottom(0)
	{
	}

	// 0x470B30
	void IntRect::intersect(const IntRect& a, const IntRect& b)
	{
		left = std::max(a.left, b.left);
		right = std::min(a.right, b.right);
		top = std::max(a.top, b.top);
		bottom = std::min(a.bottom, b.bottom);
	}

	// 0x470B80 (folded)
	Rect::Rect(float left, float top, float right, float bottom)
		: left(left), top(top), right(right), bottom(bottom)
	{
	}

	// 0x470B80 (folded)
	IntRect::IntRect(int left, int top, int right, int bottom)
		: left(left), top(top), right(right), bottom(bottom)
	{
	}

	// 0x470BA0
	void Rect::intersect(const Rect& a, const Rect& b)
	{
		left = std::max(a.left, b.left);
		right = std::min(a.right, b.right);
		top = std::max(a.top, b.top);
		bottom = std::min(a.bottom, b.bottom);
	}

	// 0x470C30: this becomes the bounding box of this and r
	void Rect::unite(const Rect& r)
	{
		left = std::min(left, r.left);
		right = std::max(right, r.right);
		top = std::min(top, r.top);
		bottom = std::max(bottom, r.bottom);
	}

	// 0x470CB0
	void Rect::normalize()
	{
		if (right < left)
			std::swap(left, right);
		if (bottom < top)
			std::swap(top, bottom);
	}

	// 0x4D0470 (folded)
	Rect::~Rect()
	{
	}

	// 0x4D0470 (folded)
	IntRect::~IntRect()
	{
	}
}
